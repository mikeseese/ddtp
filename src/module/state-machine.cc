#include "ddtp.h"

void DDTP::GetNextState() {
  switch (state) {
    case STANDBY: {
      if (flag_StartingTransmission) {
        state = PRE_SEND_SESSION_INFO;
        flag_StartingTransmission = false;
        return;
      } else if (flag_ReceivedTransmission) {
        state = PRE_RECV_SESSION_INFO;
        flag_ReceivedTransmission = false;
        return;
      }
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
      state = PRE_SEND_DATA;
      return;
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
      state = SEND_CTRL;
      return;
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
      state = STANDBY;
      return;
    }
  }
}
