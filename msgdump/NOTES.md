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
    There are 
    multiple message formats defined only by length.
    The decoding setup is awful.
    At line 1553, fan out on message length for ImprovedTerseObjectUpdate.
