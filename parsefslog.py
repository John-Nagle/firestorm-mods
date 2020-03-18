#
#   parsefslog.py -- Parser for Firestorm log lines from Animats debug code.
#
#   Animats
#   March, 2020
#
import re
SAMPLELINE = '''
2020-03-17T21:20:22Z INFO # newview/llviewerobject.cpp(2471) processUpdateMessage : Vehicle/seat update msg from 216.82.49.146 type: 1 compressed: 1 update type: 1 Vehicle UUID: fdb6774e-0eac-4947-fc52-262960648d5f Parenting change: 0 Region: Vine PositionAgent: { -1.067578, 150.288254, 109.260979 } Velocity: { -13.595909, 6.591904, -0.162109 } Ang. Vel: { 0.192383, -0.405281, -0.010742 } Accel: { -0.344730, 0.403328, -5.628017 }
'''

UPDATEMSGRE = re.compile(r"(\d\d\d\d-\d\d-\d\d)T(\d\d\:\d\d:\d\d)Z\s*INFO\s*#\s*newview.*processUpdateMessage : (.*)\s+update msg from\s*([a-z0-9\.\-]+)\s(type: .*$)")


### UPDATEMSGFIELDS = re.compile(r"type:\s+(\d+)\s+compressed:\s+(\d+)\s+update type:\s+(\d+).*$")
UPDATEMSGFIELDSRE = re.compile(r"type:\s+(\d+)\s+compressed:\s+(\d+)\s+update type:\s+(\d+)\s+Vehicle UUID:\s+(.+)\s+Parenting change:\s+(\d+)\s+Region:\s(.+)\s(PositionAgent.*)")

UPDATEMSGVECTORRE = re.compile(r"\s*([\w .]+):\s*{([\d\s., -]+[} ])\s*$") # name: {vector} 
VECTORRE = re.compile(r"\s*([\d.-]+)\s*,\s*([\d.-]+)\s*,\s*([\d.-]+)\s*")    # float, float, float

def parseline(s) :
    '''
    Parse log line into fields.
    '''   
    fields = UPDATEMSGRE.match(s)                               # break off date, 
    if fields :
        print(fields.groups(0))
        date = fields.group(1)
        time = fields.group(2)
        dataarea = fields.group(5)
        print("Data area: %s" %  (dataarea,))
        datafields = UPDATEMSGFIELDSRE.match(dataarea)
        if datafields :
            print("Data fields: %s" % (datafields.groups(0),) )        
            vectorarea = datafields.group(7)
            print("Vectors: %s" % (vectorarea,))
            vectorexprs = vectorarea.strip().split("}")                # split off each vector expression; no repeating capturing REs in standard Python
            print("Vector exprs: %s" % (vectorexprs,))         # ***TEMP***
            ####vectorfields = UPDATEMSGVECTORSRE.match(vectorarea)
            vectorfields = [UPDATEMSGVECTORRE.match(g) for g in vectorexprs if g] # extract name/value vector pairs
            for field in vectorfields :
                print("Vector: %s" % (field.groups(0),))
                vectoritem = VECTORRE.match(field.group(2))            # extract 3 numbers
                if vectoritem :
                    print("Vector data: %s = (%s,%s,%s)" % (field.group(1),vectoritem.group(1),vectoritem.group(2),vectoritem.group(3)))
            print("Vector fields: %s" % (vectorfields))           
    return (str(fields))
    
    
def unittest(s) :
    s = s.strip()
    print("Input: %s" % (s,))
    parsed = parseline(s);
    print("Parsed: %s" % (parsed,))
    
def main() :
    unittest(SAMPLELINE);
    
main()
