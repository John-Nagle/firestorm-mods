#
#   parsefslog.py -- Parser for Firestorm log lines from Animats debug code.
#
#   Animats
#   March, 2020
#
import re
import argparse
import numpy
import time
import datetime

#   Test case for unit test. One log line.
SAMPLELINE = '''
2020-03-17T21:20:22Z INFO # newview/llviewerobject.cpp(2471) processUpdateMessage : Vehicle/seat update msg from 216.82.49.146 type: 1 compressed: 1 update type: 1 Vehicle UUID: fdb6774e-0eac-4947-fc52-262960648d5f Parenting change: 0 Region: Vine PositionAgent: { -1.067578, 150.288254, 109.260979 } Velocity: { -13.595909, 6.591904, -0.162109 } Ang. Vel: { 0.192383, -0.405281, -0.010742 } Accel: { -0.344730, 0.403328, -5.628017 }
'''

#
#   class Logread
#
class Logread :
    '''
    Reader for Animats object update log from modified Firestorm SL viewer
    '''
    #   Regular expressions for parsing log lines
    UPDATEMSGRE = re.compile(r"(\d\d\d\d-\d\d-\d\d)T(\d\d\:\d\d:\d\d)Z\s*INFO\s*#\s*newview.*processUpdateMessage : (.*)\s+update msg from\s*([a-z0-9\.\-]+)\s(type: .*$)")

    UPDATEMSGFIELDSRE = re.compile(r"type:\s+(\d+)\s+compressed:\s+(\d+)\s+update type:\s+(\d+)\s+Vehicle UUID:\s+(.+)\s+Parenting change:\s+(\d+)\s+Region:\s(.+)\s(PositionAgent.*)")

    UPDATEMSGVECTORRE = re.compile(r"\s*([\w .]+):\s*{([\d\s., -]+[} ])\s*$") # name: {vector} 

    VECTORRE = re.compile(r"\s*([\d.-]+)\s*,\s*([\d.-]+)\s*,\s*([\d.-]+)\s*")    # float, float, float
    
    def __init__(self, verbose) :
        self.verbose = verbose                                              # more print output
                                
    def parseline(self, s) :
        '''
        Parse log line into a dictionary of fields. 
        '''
        outputfields = {}                                           # accum output fields here
        fields = Logread.UPDATEMSGRE.match(s)                               # break off date, 
        if fields :
            ####print(fields.groups(0))
            date = fields.group(1)
            time = fields.group(2)
            outputfields["date"] = fields.group(1)
            outputfields["time"] = fields.group(2)
            dataarea = fields.group(5)
            #### print("Data area: %s" %  (dataarea,))
            datafields = Logread.UPDATEMSGFIELDSRE.match(dataarea)
            if datafields :
                #### print("Data fields: %s" % (datafields.groups(0),) )
                outputfields["region"] = datafields.group(6)                # name of region     
                vectorarea = datafields.group(7)                            # values which are vectors
                ####print("Vectors: %s" % (vectorarea,))
                vectorexprs = vectorarea.strip().split("}")                 # split off each vector expression; no repeating capturing REs in standard Python
                ####print("Vector exprs: %s" % (vectorexprs,))         # ***TEMP***
                vectorfields = [Logread.UPDATEMSGVECTORRE.match(g) for g in vectorexprs if g] # extract name/value vector pairs
                for field in vectorfields :
                    ####print("Vector: %s" % (field.groups(0),))
                    vectoritem = Logread.VECTORRE.match(field.group(2))            # extract 3 numbers
                    if vectoritem :
                        ####print("Vector data: %s = (%s,%s,%s)" % (field.group(1),vectoritem.group(1),vectoritem.group(2),vectoritem.group(3)))
                        outputfields[field.group(1)] = numpy.array([float(vectoritem.group(1)), float(vectoritem.group(2)), float(vectoritem.group(3))])
                ####print("Vector fields: %s" % (vectorfields))           
                return outputfields
        return None                                                         # did not match
    
    def dologline(self, line, lineno) :
        try :
            item = self.parseline(line)                                    # parse if possible
            if item :                                                      # if got something
                pos = item["PositionAgent"]
                vel = item["Velocity"]
                time = item["time"]
                region = item["region"]
                print("%s %s %s %s" % (time, region, pos, vel))
            return item
                    
        except ValueError as err :
            print("Line %d: ERROR %s" % (lineno, err))
        
    def fixtimestamp(self, items) :
        """
        Fix timestamp on items
    
        These items all have the same timestamp. Space them out in time
        """
        cnt = len(items)
        if (cnt == 0) :
            return                          # nothing to do
        interval = 1.0/cnt                  # time interval
        timeoffset = 0.0
        for item in items :                 # timestamp each item
            itemtime = datetime.datetime.combine(
                datetime.datetime.strptime(item["date"],"%Y-%m-%d").date(),
                datetime.datetime.strptime(item["time"],"%H:%M:%S").time(),
                tzinfo=datetime.timezone.utc)
            item['timestamp'] = itemtime.timestamp() + timeoffset           
            timeoffset += interval                                          # advance fraction of second
        
    def addtimestamp(self, items) :
        """
        Add timestamp to each item.
    
        This isn't really sound.  We only have 1 second resolution on times,
        so if we have multiple items from the same second, we give them
        timestamps equally spaced across the seconds.
        """
        secitems = []                                                       # items from this second
        for item in items :
            time = item["time"]                                             # get timestamp
            if secitems == [] or secitems[0]["time"] == time :              # if same second
                secitems.append(item)                                       # save for this second
            else :                                                          # done with this second
                self.fixtimestamp(secitems)                                 # space out in time
                secitems = [item]                                           # done with this second
        self.fixtimestamp(secitems)                                         # do final items
        return 
    
    
    def dologfile(self, filename) :
        """
        Process one logfile
        """
        try :
            with open(filename,"r") as fd :                                 # open file
                print("Reading \"%s\"." % (filename,))                      # processing this file
                lineno = 0                                                  # for messages
                items = []
                for line in fd :                                            # for each line in file
                    item = self.dologline(line,++lineno)                    # do this line
                    if item :
                        items.append(item)                                  # all items in memory
                # Done reading and parsing.
                self.addtimestamp(items)                                    # add a timestamp field to each item
                self.analyze1(items)                                        # position from velocity
                
        except IOError as err :
            print("Unable to open  \"%s\": %s" % (filename,err))
        
    def analyze1(self, items) :
        """
        Log item analysis 1: velocity from position differences
        """
        if (len(items) == 0):
            return
        startitem = items[0]
        t0 = items[0]['timestamp']
        starttime = t0
        p0 = items[0]['PositionAgent']
        for item in items[1:] :
            ####print(item)
            t1 = item['timestamp']
            p1 = item['PositionAgent']
            dt = t1 - t0                            # time delta
            dp = p1 - p0
            calcvel = dp * (1/dt)
            pos = item['PositionAgent']
            vel = item['Velocity']
            region = item["region"]
            print("%6.2f %s %s %s %s" % (t1-starttime, region, pos, vel, calcvel))
            t0 = t1
            p0 = p1       
    
def unittest(s) :
    s = s.strip()
    print("Input: %s" % (s,))
    parsed = parseline(s);
    print("Parsed: %s" % (parsed,))
    
def main() :
    ### unittest(SAMPLELINE);
    #   Parse command line
    parser = argparse.ArgumentParser(description='Process Firestorm log with Animats object update extensions.')
    parser.add_argument("--verbose", "-v", help="Verbose mode", action="store_true")
    parser.add_argument('logfile', metavar='LOGFILE', type=str, nargs='*',
                    help='Files to process')
    args = parser.parse_args()                                          # parse command line
    verbose = args.verbose                                              # verbose flag
    files = args.logfile                                                # files to do
    for filename in args.logfile :                                      # for filenames given
        lr = Logread(args.verbose)
        lr.dologfile(filename)                                          # do the log file
    
main()
