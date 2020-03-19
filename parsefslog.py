#
#   parsefslog.py -- Parser for Firestorm log lines from Animats debug code.
#
#   Animats
#   March, 2020
#
import re
import argparse
import numpy

#   Test case for unit test. One log line.
SAMPLELINE = '''
2020-03-17T21:20:22Z INFO # newview/llviewerobject.cpp(2471) processUpdateMessage : Vehicle/seat update msg from 216.82.49.146 type: 1 compressed: 1 update type: 1 Vehicle UUID: fdb6774e-0eac-4947-fc52-262960648d5f Parenting change: 0 Region: Vine PositionAgent: { -1.067578, 150.288254, 109.260979 } Velocity: { -13.595909, 6.591904, -0.162109 } Ang. Vel: { 0.192383, -0.405281, -0.010742 } Accel: { -0.344730, 0.403328, -5.628017 }
'''


#   Regular expressions for parsing log lines
UPDATEMSGRE = re.compile(r"(\d\d\d\d-\d\d-\d\d)T(\d\d\:\d\d:\d\d)Z\s*INFO\s*#\s*newview.*processUpdateMessage : (.*)\s+update msg from\s*([a-z0-9\.\-]+)\s(type: .*$)")

UPDATEMSGFIELDSRE = re.compile(r"type:\s+(\d+)\s+compressed:\s+(\d+)\s+update type:\s+(\d+)\s+Vehicle UUID:\s+(.+)\s+Parenting change:\s+(\d+)\s+Region:\s(.+)\s(PositionAgent.*)")

UPDATEMSGVECTORRE = re.compile(r"\s*([\w .]+):\s*{([\d\s., -]+[} ])\s*$") # name: {vector} 

VECTORRE = re.compile(r"\s*([\d.-]+)\s*,\s*([\d.-]+)\s*,\s*([\d.-]+)\s*")    # float, float, float

def parseline(s) :
    '''
    Parse log line into a dictionary of fields. 
    '''
    outputfields = {}                                           # accum output fields here
    fields = UPDATEMSGRE.match(s)                               # break off date, 
    if fields :
        ####print(fields.groups(0))
        date = fields.group(1)
        time = fields.group(2)
        outputfields["date"] = fields.group(1)
        outputfields["time"] = fields.group(2)
        dataarea = fields.group(5)
        #### print("Data area: %s" %  (dataarea,))
        datafields = UPDATEMSGFIELDSRE.match(dataarea)
        if datafields :
            #### print("Data fields: %s" % (datafields.groups(0),) )
            outputfields["region"] = datafields.group(6)                # name of region     
            vectorarea = datafields.group(7)                            # values which are vectors
            ####print("Vectors: %s" % (vectorarea,))
            vectorexprs = vectorarea.strip().split("}")                 # split off each vector expression; no repeating capturing REs in standard Python
            ####print("Vector exprs: %s" % (vectorexprs,))         # ***TEMP***
            ####vectorfields = UPDATEMSGVECTORSRE.match(vectorarea)
            vectorfields = [UPDATEMSGVECTORRE.match(g) for g in vectorexprs if g] # extract name/value vector pairs
            for field in vectorfields :
                ####print("Vector: %s" % (field.groups(0),))
                vectoritem = VECTORRE.match(field.group(2))            # extract 3 numbers
                if vectoritem :
                    ####print("Vector data: %s = (%s,%s,%s)" % (field.group(1),vectoritem.group(1),vectoritem.group(2),vectoritem.group(3)))
                    outputfields[field.group(1)] = numpy.array([float(vectoritem.group(1)), float(vectoritem.group(2)), float(vectoritem.group(3))])
            ####print("Vector fields: %s" % (vectorfields))           
            return outputfields
    return None                                                         # did not match
    
def dologline(line, lineno) :
    try :
        items = parseline(line)                                         # parse if possible
        if items :                                                      # if got something
            pos = items["PositionAgent"]
            vel = items["Velocity"]
            time = items["time"]
            region = items["region"]
            print("%s %s %s %s" % (time, region, pos, vel))
                    
    except ValueError as err :
        print("Line %d: ERROR %s" % (lineno, err))
    
    
def dologfile(filename, verbose) :
    """
    Process one logfile
    """
    try :
        with open(filename,"r") as fd :                                 # open file
            print("Reading \"%s\"." % (filename,))                      # processing this file
            lineno = 0                                                  # for messages
            for line in fd :                                            # for each line in file
                dologline(line,++lineno)                                # do this line
                
    except IOError as err :
        print("Unable to open  \"%s\": %s" % (filename,err))           
    
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
        dologfile(filename,args.verbose)                                # do the log file
    
main()
