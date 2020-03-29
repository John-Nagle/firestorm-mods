#
#   parsefslog.py -- Parser for Firestorm log lines from Animats debug code.
#
#   Animats
#   March, 2020
#
import re
import argparse
import numpy
import math
import time
import datetime
import ntpath

#   Test case for unit test. One log line.
SAMPLELINE = '''
2020-03-17T21:20:22Z INFO # newview/llviewerobject.cpp(2471) processUpdateMessage : Vehicle/seat update msg from 216.82.49.146 type: 1 compressed: 1 update type: 1 Vehicle UUID: fdb6774e-0eac-4947-fc52-262960648d5f Parenting change: 0 Region: Vine PositionAgent: { -1.067578, 150.288254, 109.260979 } Velocity: { -13.595909, 6.591904, -0.162109 } Ang. Vel: { 0.192383, -0.405281, -0.010742 } Accel: { -0.344730, 0.403328, -5.628017 }
'''
SAMPLELINE = '''
2020-03-29T18:20:27Z INFO # newview/llviewerobject.cpp(2471) processUpdateMessage : Vehicle/seat update msg from 216.82.51.209 type: 1 compressed: 1 update type: 1 Vehicle UUID: 18b0043f-1a90-f4c6-5b2c-24d15eba8cef Parenting change: 0 Region: Burns Position: { 113.457893, 79.910339, 70.149063 } Rotation: { 0.001532, -0.001669, -0.741905, 0.670501 } Velocity: { 0.000000, 0.000000, -0.115234 } Ang. Vel: { -0.053711, 0.063477, 0.000000 } Accel: { 0.000000, 0.000000, -1.715847 }
'''

MINPERIOD = 1/45.0                                                  # shortest possible sample interval, SL physics rate
DEG_TO_RAD = math.pi / 180.0                                        # Degrees to radians conversion constant


#
#   class Lowpassfilter
#
class Lowpassfilter :
    '''
    Low-pass filter for smoothing
    '''
    
    def __init__(self, filterconstant) :
        '''
        Set up very simple filter
        '''
        self.filterconstant = filterconstant                        # from 0 to 1
        self.filtered = None
        
    def clear(self) :
        '''
        Clear filter, start fresh from next value
        '''
        self.filtered = None
        
    def update(self, value) :
        '''
        Add value to filter
        '''
        if self.filtered is None :
            self.filtered = value
        else :
            self.filtered = self.filtered * (1.0 - self.filterconstant) + value*self.filterconstant # add value to filter. Can be vector or scalar
            
    def updatetimed(self, value, dt) :
        '''
        Add value to filter, weighted by time
        '''
        if self.filtered is None :
            self.filtered = value
        else :
            ####   ***NOT MATHEMATICALLY CORRECT*** Smoothing for longer dt is not linear.
            periods = math.ceil(dt / MINPERIOD)     # do this many times
            for i in range(periods) :               # inefficient way to handle variable sample times
                self.filtered = self.filtered * (1.0 - self.filterconstant) + value * self.filterconstant
            ###filterval = self.filterconstant * dt
            ####if (filterval > 1.0) :
            ####    filterval = 1.0
            ####self.filtered = self.filtered * (1.0 - filterval) + value*filterval # add value to filter. Can be vector or scalar

        
             
    def get(self) :
        '''
        Return filtered value
        '''
        return self.filtered                    
        
#
#   class Logread
#
class Logread :
    '''
    Reader for Animats object update log from modified Firestorm SL viewer
    '''
    #   Regular expressions for parsing log lines
    UPDATEMSGRE = re.compile(r"(\d\d\d\d-\d\d-\d\d)T(\d\d\:\d\d:\d\d)Z\s*INFO\s*#\s*newview.*processUpdateMessage : (.*)\s+update msg from\s*([a-z0-9\.\-]+)\s(type: .*$)")

    UPDATEMSGFIELDSRE = re.compile(r"type:\s+(\d+)\s+compressed:\s+(\d+)\s+update type:\s+(\d+)\s+Vehicle UUID:\s+(.+)\s+Parenting change:\s+(\d+)\s+Region:\s(.+)\s(Position:.*)")

    UPDATEMSGVECTORRE = re.compile(r"\s*([\w .]+):\s*{([\d\s., -]+[} ])\s*$") # name: {vector} 

    VECTORRE = re.compile(r"\s*([\d.-]+)\s*,\s*([\d.-]+)\s*,\s*([\d.-]+)\s*$")    # float, float, float
    
    QUATERNIONRE = re.compile(r"\s*([\d.-]+)\s*,\s*([\d.-]+)\s*,\s*([\d.-]+)\s*,\s*([\d.-]+)\s*$")    # float, float, float, float
    
    def __init__(self, verbose, filterconstant) :
        self.verbose = verbose                                              # more print output
        self.starttime = None                                               # starting time of this data set
        self.items = None                                                   # the data items
        self.filteredvel = Lowpassfilter(filterconstant)                    # filtered velocity
        self.filteredangvel = Lowpassfilter(filterconstant)                 # filtered square error
                                
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
                    quatitem = Logread.QUATERNIONRE.match(field.group(2))       # extract 4 numbers
                    if quatitem :
                        outputfields[field.group(1)] = numpy.array([float(quatitem.group(1)), float(quatitem.group(2)), float(quatitem.group(3)), 
                                float(quatitem.group(4))])
                    else:
                        vectoritem = Logread.VECTORRE.match(field.group(2))            # extract 3 numbers
                        if vectoritem :
                            ####print("Vector data: %s = (%s,%s,%s)" % (field.group(1),vectoritem.group(1),vectoritem.group(2),vectoritem.group(3)))
                            outputfields[field.group(1)] = numpy.array([float(vectoritem.group(1)), float(vectoritem.group(2)), float(vectoritem.group(3))])
                        else :
                            print("Failed to parse vector/quaternion: " + field.group(2))
                            raise RuntimeError("Invalid input")
                    
                            
                ####print("Vector fields: %s" % (vectorfields))           
                return outputfields
        return None                                                        # did not match
    
    def dologline(self, line, lineno) :
        try :
            item = self.parseline(line)                                    # parse if possible
            if item :                                                      # if got something
                pos = item["Position"]
                rot = item["Rotation"]
                vel = item["Velocity"]
                time = item["time"]
                region = item["region"]
                if self.verbose :                                           # print basic input data
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
                self.items = self.analyze1(items)                           # position from velocity
                
        except IOError as err :
            print("Unable to open  \"%s\": %s" % (filename,err))
            
    def analyzeitem(self, item, dt, regionchange) :
        '''
        Analysis of one individual log item
        '''
        if regionchange :                                                   # on region change, reset filters
            self.filteredvel.clear()
            self.filteredangvel.clear()
        vel = item['Velocity']
        self.filteredvel.updatetimed(vel,dt)
        smoothedvel = self.filteredvel.get()
        velerr = numpy.linalg.norm(vel - smoothedvel) 
        
        angvel = item['Ang. Vel']
        self.filteredangvel.updatetimed(angvel,dt)
        smoothedangvel = self.filteredangvel.get()
        angvelerr = numpy.linalg.norm(angvel - smoothedangvel)
        item['velerr'] = velerr
        item['angvelerr'] = angvelerr
        #   Calculate maximum project-ahed time
        ALLOWEDDISTERR = 0.5                            # Maximum allowed distance error, meters
        ALLOWEDANGERR = 20.0 * DEG_TO_RAD               # Maximum allowed angular error, radians
        secslin = math.inf
        if velerr > 0.001 :
            secslin = ALLOWEDDISTERR / velerr
        secsang = math.inf
        if angvelerr > 0.001 :
            secsang = ALLOWEDANGERR / angvelerr
        projectmax = min(secslin, secsang)
        item['projectmax'] = projectmax
        t1 = item['timestamp']
        pos = item['Position']
        region = item['region']
        print("%6.2f %s %s %s %s %s %s %1.2f %1.2f %1.2f" % (t1-self.starttime, region, pos, vel, smoothedvel, 
            angvel, smoothedangvel, velerr, angvelerr, projectmax))
        return item
              
    def analyze1(self, items) :
        """
        Log item analysis 1: velocity from position differences
        """
        if (len(items) == 0):
            return
        t0 = items[0]['timestamp']
        r0 = items[0]['region']
        self.starttime = t0
        p0 = items[0]['Position']
        items[0]['velerr'] = 0.0                    # no error at start
        items[0]['angvelerr'] = 0.0
        items[0]['projectmax'] = math.inf
        newitems = [items[0]]
        for item in items[1:] :
            ####print(item)
            t1 = item['timestamp']
            p1 = item['Position']
            r1 = item['region']
            dt = t1 - t0                            # time delta
            newitem = self.analyzeitem(item, dt, r0 != r1)   
            t0 = t1
            p0 = p1
            r0 = r1
            newitems.append(newitem)
        return newitems
        
    def vectostring(self,vec) :
        '''
        Vector to comma-separated formatted string
        '''
        s = "  "
        for val in vec :
            s += " %9.4f," % (val,)
        return s
                
    def writecsv1(self, filename) :
        '''
        Write CSV file of raw Z values
        '''
        with open(filename,"w") as wf :
            print("#  T            Position                    Rotation                    Velocity          Ang. Vel  Region",file=wf)      # file header
            for item in self.items :
                print(item); # ***TEMP***
                t = item['timestamp']
                ####s = "%6.3f,%6.3f,%6.3f,%6.3f,%6.3f, %s" % (t-self.starttime,item['Velocity'][2],item['velerr'],item['angvelerr'],item['projectmax'],item['region'])
                s = "%7.3f,    " % (t-self.starttime)
                s += self.vectostring(item['Position'])
                s += self.vectostring(item['Rotation'])
                s += self.vectostring(item['Velocity'])
                s += self.vectostring(item['Ang. Vel'])
                s += item['region']
                print(s,file=wf)
    
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
    filterconstant = 2.0*MINPERIOD                                      # shortest possible interval
                                                                        # ***TEMP***
    print("Filter constant: %1.2f" % (filterconstant,))
    for filename in args.logfile :                                      # for filenames given
        lr = Logread(args.verbose, filterconstant)
        lr.dologfile(filename)                                          # do the log file
        outfilename = ntpath.basename(filename) + "z.csv"               # Z values CSV file
        lr.writecsv1(outfilename)                                       # write CSV file
    
main()
