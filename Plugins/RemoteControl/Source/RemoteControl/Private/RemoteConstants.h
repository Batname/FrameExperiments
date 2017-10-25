#pragma once

// Seconds. When game starts, wait this time to start broadcast. Because crashing...
//@todo: Currently Map/Unmap removed because of random crashes. This variable not necessary at the moment.
#define WAIT_TO_START_BROADCAST 0.2f

// Seconds
#define REMOTE_PING_RATE 0.1f

// Seconds. When server change controller id of client, we need to wait because clients gets new controller id delayed
#define REMOTE_WAIT_CLIENT_CONTROLLER 2.f

/*
 * Actions
 */

// Required for UDP but it is also good for TCP. When stopping game send this action to clients. So they will know game stopped (instead of socket read timeout)
#define REMOTE_ACTION_CLOSE 0

// When one controller wants to change own id but new id is already in use by another controller? Swap ids and send this action to another controller. 
#define REMOTE_ACTION_SYNC 1

// Stream
#define REMOTE_ACTION_COMPRESSED_IMAGE 2

// Touch information from controller
#define REMOTE_ACTION_TOUCH 3

// Motion information form controller
#define REMOTE_ACTION_MOTION 4

// Ping - Game & Controller
#define REMOTE_ACTION_PING 5

// Pong - Game & Controller
#define REMOTE_ACTION_PONG 6

// Reset Head Tracker
#define REMOTE_ACTION_RESET_HEAD_TRACKER 7


