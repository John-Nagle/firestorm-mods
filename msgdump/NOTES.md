# Notes on message dumping

# 2018-09-11

Object updates of interest are processed in:

llvovolume.cpp: 
    LLVOVolume::processUpdateMessage
    (Beq Janus has been in there.)
    
llvoavatar.cpp
    calls object update?
    
llviewerobject.cpp:
    Main object update at line 1093:
    LLViewerObject::processUpdateMessage
    
    At line 1205, fan out on full or terse update message.
    At line 1258, fan out on message length for full update.
    There are multiple message formats defined only by length.
    The decoding setup is awful.
    At line 1553, fan out on message length for ImprovedTerseObjectUpdate.
    
    This format is simpler than it looks.See "fsmsgdump.h" for what it really is.
    
    Potential buffer overflow in llviewerobject.cpp at lines 1235 and 1549:
    
    	length = mesgsys->getSizeFast(_PREHASH_ObjectData, block_num, _PREHASH_ObjectData);
		mesgsys->getBinaryDataFast(_PREHASH_ObjectData, _PREHASH_ObjectData, data, length, block_num);
		
    "data" is a 76-byte array on the stack.  "length" comes from the incoming packet. 
    getBinaryDataFast does not check bounds. An oversized value from the net will clobber the stack.
    
    TerseObjectUpdate is described here: http://wiki.secondlife.com/wiki/ImprovedTerseObjectUpdate
    The section marked "data" there is what we have in fsmsgdump.h. That's documented in
    for a full update at http://wiki.secondlife.com/wiki/ObjectUpdate#ObjectData_Format
    Full update and terse update have the same positional data.


 
    
