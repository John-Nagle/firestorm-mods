# Notes on message dumping

# 2018-09-11

TerseObjectUpdate is described here: http://wiki.secondlife.com/wiki/ImprovedTerseObjectUpdate
The section marked "data" there is what we have in fsmsgdump.h. That's documented in
for a full update at http://wiki.secondlife.com/wiki/ObjectUpdate#ObjectData_Format
Full update and terse update have the same positional data format.

Object updates of interest are processed in:

### llvovolume.cpp: 
Update for ordinary objects.

See LLVOVolume::processUpdateMessage (Beq Janus has been in there making changes.)
    
### llvoavatar.cpp
Calls main object update?
    
### llviewerobject.cpp:
Main object update at line 1093: LLViewerObject::processUpdateMessage
    
At line 1205, fan out on full or terse update message.

At line 1258, fan out on message length for full update.

There are multiple message formats defined only by length. The decoding setup is awful.

At line 1553, fan out on message length for ImprovedTerseObjectUpdate.
    
This format is simpler than it looks.See "fsmsgdump.h" for what it really is.

### llvocavatarself.cpp

Avatar region crossings happen at 

LLVOAvatarSelf::updateRegion

which ought to unconditionally put in the log

    LL_INFOS() << "Region crossing took " << (F32)(delta * 1000.0).value() << " ms " << LL_ENDL;
    
but, in fact, that message appears only for some region crossings. Do not know why yet.

In the "all region crossings take 10s" state, the messages show numbers like 10000 ms.

At line 2362, there's a check for a significant position change. Good place to call object dump when logging messages.

### llcircuit.cpp

LLCircuitData::checkPacketInID

The message

    2018-09-12T05:00:53Z INFO: llmessage/llcircuit.cpp(767) : checkPacketInID: packet_out_of_order - got packet 2 expecting 3701 from 216.82.56.75:13001
    2018-09-12T05:00:53Z INFO: llmessage/llcircuit.cpp(768) : checkPacketInID: 0 since last log entry
    
appears in some runs with region crossing failures. But it also appears in ones with no region crossing failures. It even appears in ones where no vehicle is used.
It's always "got packet 2". And there are no "packet gap" error messages. Is something wrong there?


#### Trouble spots
    
Potential buffer overflow in llviewerobject.cpp at lines 1235 and 1549:
    
    length = mesgsys->getSizeFast(_PREHASH_ObjectData, block_num, _PREHASH_ObjectData);
    mesgsys->getBinaryDataFast(_PREHASH_ObjectData, _PREHASH_ObjectData, data, length, block_num);
		
"data" is a 76-byte array on the stack.  "length" comes from the incoming packet. 
getBinaryDataFast does not check bounds. An oversized value from the net will overwrite the stack.
    
Some update sizes, the ones with the "foot vector", apply only to avatars. If sent to a regular
object, an unchecked downcast will result in calling an avatar-specific function on a non avatar
object.

The code around region change recognition, at the beginning of object update, is suspicious.
Need to dump info from there. 

# 2018-09-13 

Added a dump call to llviewerobject.cpp to dump object state when an avatar moves more than 0.5 m in one update.
This triggers on half-unsits. Looking at logs for anomalies. 

## Suspicious:

Here's our half-unsit:

    2018-09-13T03:17:26Z WARNING: newview/llviewerobject.cpp(2363) : processUpdateMessage: Avatar moved more than 0.5m in one update.
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(486) : dump: Type: avatar
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(487) : dump: Drawable: 0x19cff4d0
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(488) : dump: Update Age: 18.088038s
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(490) : dump: Parent: 0x2a8eafe0
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(491) : dump: ID: dadec334-539a-4875-ad0e-d9654705f437
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(492) : dump: LocalID: 311103751
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(493) : dump: PositionRegion: { 121.871002, 510.847351, 71.908188 }
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(494) : dump: PositionAgent: { 121.871002, 510.847351, 71.908188 }
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(495) : dump: PositionGlobal: { 463225.871002, 307198.847351, 71.908188 }
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(496) : dump: Velocity: { 0.000000, 0.000000, 0.000000 }
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(504) : dump: Pool: 0x19d01fe0
    2018-09-13T03:17:26Z INFO: newview/llviewerobject.cpp(505) : dump: Pool reference count: 8
    ===> Half-unsit. Avatar is in air, and one region out of position. Update took 18 seconds. <===
    
Avatar is stuck in the air, a region away from where it's supposed to be.

Lots of packet losses around trouble spot.

    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(767) : checkPacketInID: packet_out_of_order - got packet 2 expecting 215 from 216.82.46.80:13020
    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(768) : checkPacketInID: 0 since last log entry
    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(782) : checkPacketInID: MSG: <- 216.82.46.80:13020	PACKET GAP:	2 expected 215
    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(214) : ackReliablePacket: MSG: <- 216.82.46.80:13020	RELIABLE ACKED:	51
    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(1006) : updateWatchDogTimers: MSG: <- 216.82.51.165:12035	LOST PACKET:	435
    
But that's after the half-unsit.

Also seeing a few entries like this, after the half-unsit:

    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(782) : checkPacketInID: MSG: <- 216.82.46.80:13020	PACKET GAP:	2 expected 215
    
The "2 expected 215" comes from line 779 of llcircuit.cpp and indicates that we got message sequence #2 when expecting #215. Maybe.
That's not right. It's like the sim reset its outgoing message number counter.  Or something was sent on the wrong circuit.
It's always #2. We never see another number. 

Context:

    2018-09-13T03:17:31Z INFO: newview/llworld.cpp(529) : addRegion: Add region with handle: 1986817511698432 on host 216.82.46.80:13020
    2018-09-13T03:17:31Z INFO: newview/llworld.cpp(539) : addRegion: Region already exists and is alive, using existing region
    2018-09-13T03:17:31Z INFO: newview/llworld.cpp(1682) : process_enable_simulator: simulator_enable() Enabling 216.82.46.80:13020 with code 910903779
    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(214) : ackReliablePacket: MSG: <- 216.82.46.89:13003	RELIABLE ACKED:	2
    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(767) : checkPacketInID: packet_out_of_order - got packet 2 expecting 215 from 216.82.46.80:13020
    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(768) : checkPacketInID: 0 since last log entry
    2018-09-13T03:17:31Z INFO: llmessage/llcircuit.cpp(782) : checkPacketInID: MSG: <- 216.82.46.80:13020	PACKET GAP:	2 expected 215
    
From the messages like

    2018-09-13T04:56:27Z INFO: #Messaging; llmessage/message.cpp(1542) : disableCircuit: LLMessageSystem::disableCircuit for 216.82.50.52:13003
    2018-09-13T04:56:27Z WARNING: #Messaging; llmessage/message.cpp(1581) : disableCircuit: Couldn't find circuit code for 216.82.50.52:13003

it would appear that all circuits have "circuit code" 0, which isn't kept in the table of circuit codes. Maybe that's left over from the days of sending
geometry and textures over UDP. 


## non-problem

Note that one incoming message can produce multiple PACKET GAP log entries. Those are OK.  It's PACKET LOST that matters.

# 2018-09-18

More progress. Half-unsits seem to be associated with taking a specific path through LLVOAvatarSelf::updateRegion in llvoavatarself.cpp. 
When called with regionp non-null, trouble. This seems backwards, since calling it with null results in a set region to null.
But a failed region cross on the beta grid (failedregioncrossbeta1.txt) yielded:

    2018-09-19T04:05:47Z INFO: newview/llvoavatarself.cpp(1281) : LLVOAvatarSelf::updateRegion: pos_from_old_region is { 205591.526035, 178708.042101, 22.315372 } while pos_from_new_region is { 205335.526035, 178708.042101, 22.315372 }
    2018-09-19T04:05:47Z INFO: #Avatar; newview/llvoavatarself.cpp(297) : LLVOAvatarSelf::setHoverIfRegionEnabled:  Avatar 'animats' full  set hover height from debug setting -0.021000
    2018-09-19T04:05:47Z INFO: newview/llvoavatarself.cpp(1306) : LLVOAvatarSelf::updateRegion: Region crossing took 10114.257812 ms 
    2018-09-19T04:05:47Z INFO: newview/llviewerobject.cpp(4252) : setPositionParent: set position parent: Mesh Sandbox 1 (254.578201,0.129310,22.959385)
    2018-09-19T04:05:47Z WARNING: newview/llviewerobject.cpp(2396) : processUpdateMessage: Avatar moved more than 0.5m in one update.

Things went downhill from there.
Now this is after a 10 second delay at the region cross. So something has already gone wrong. 

But sometimes, not often, that message appears on a successful region cross. In "failedregioncrossbeta2.txt", it appears several times driving across a double
region cross. On the last time, 1s of network delay was inserted and that forced a fail.

Got another one on the main grid, where bike just disappears. (failedregioncrossmain1.txt). Has the "pos from old region" message, so it took the suspicious path.
Taking that path always follows a delay. But there's no log message at info level to show what ended that delay. Need to try debug level.

# 2018-09-19

Who calls updateRegion as the avatar comes out of the 10 second stall?
Don't know yet.
It looks like there was a 10 second wait and then an object update message caused the region crossing. But this happens at every region crossing once in this
state. Even regions not involved in the original trouble.

# 2018-09-20

Moving log to documents/logs.




    


 
    
