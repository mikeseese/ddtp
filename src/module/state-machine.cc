#include "ddtp.h"

void DDTP::GetNextState() {
  switch (state) {
    case STANDBY: {
      if (flag_StartingTransmission) {
        state = PRE_SEND_SESSION_INFO;
        flag_StartingTransmission = false;

        // we need to send a message to make sure a receiving
        // node moves their state machine
        ddtp_Status * status = new ddtp_Status();
        status->setCode(DDTP_ACCEPT);
        send(status, "out"); // TODO: is this the right gate?

        return;
      } else if (flag_ReceivedTransmission) {
        state = PRE_RECV_SESSION_INFO;
        flag_ReceivedTransmission = false;
        return;
      }
      break;
    }
    case PRE_SEND_SESSION_INFO: {
      ddtp_SessionInfo * sessionInfo = new ddtp_SessionInfo();
      sessionInfo->setId(session->id);
      sessionInfo->setSrc(session->src);
      sessionInfo->setDst(session->dst);
      sessionInfo->setPriority(0);
      sessionInfo->setIsControl(session->isControl);

      send(sessionInfo, "out");

      state = session->isControl ? PRE_SEND_CTRL : PRE_SEND_METADATA;
      break;
    }
    case PRE_RECV_SESSION_INFO: {
      if (session == nullptr) {
        return;
      }

      numSessions++;

      state = session->isControl ? PRE_RECV_CTRL : PRE_RECV_METADATA;
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
    case PRE_RECV_CTRL: {
      break;
    }
    default: {
      state = STANDBY;
      return;
    }
  }
}
