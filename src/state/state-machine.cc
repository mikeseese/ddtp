#include "state.h";

State GetNextState(State state) {
  switch (state) {
    case STANDBY: {
      // if received a message, return PRE_RECV_METADATA
      // if want to send a message, return PRE_SEND_SESSION_INFO
      // else return STANDBY
      break;
    }
    case PRE_SEND_SESSION_INFO: {
      // SendSessionInfo()
      // if sending data blocks, return PRE_SEND_METADATA
      // else, return PRE_SEND_CTRL
      break;
    }
    case PRE_RECV_SESSION_INFO: {
      // if received session info
      //   if receiving data blocks, return PRE_RECV_METADATA
      //   else, return PRE_RECV_CTRL
      // else, return PRE_RECV_SESSION_INFO
      break;
    }
    case PRE_SEND_METADATA: {
      // SendMetadata()
      return PRE_SEND_DATA;
      break;
    }
    case PRE_RECV_METADATA: {
      // if received metadata, return PRE_RECV_DATA
      // else return PRE_RECV_METADATA
      break;
    }
    case PRE_SEND_DATA: {
      // received a `Status`?
      // if yes
      //   accepted? return SEND_DATA
      //   else return STANDBY
      // else return PRE_SEND_DATA
      break;
    }
    case PRE_RECV_DATA: {
      // sendstatus of accept/reject
      // if reject, return STANDBY
      // else, return RECV_DATA
      break;
    }
    case SEND_DATA: {
      // SendNextBlock()
      // if not the last block, return PRE_SEND_DATA
      // else return STANDBY
      break;
    }
    case RECV_DATA: {
      // send ACKs for received blocks
      // send SNACKS for blocks that i would have expected by now
      // if i received all blocks, return STANDBY
      // else, return RECV_DATA
      break;
    }
    case PRE_SEND_CTRL: {
      // SendCtrl()
      return SEND_CTRL;
      break;
    }
    case SEND_CTRL: {
      // if received status?
      //   if () // TODO: i'm here
      // else return SEND_CTRL
      break;
    }
    case RECV_CTRL: {
      break;
    }
    default: {
      return STANDBY;
      break;
    }
  }
}
