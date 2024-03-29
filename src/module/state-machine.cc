#include "ddtp.h"

const unsigned int MAX_PENDING_BLOCKS = 5;

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
        send(status, "down$o");

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

      send(sessionInfo, "down$o");

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
      ddtp_Metadata * metadata = new ddtp_Metadata();
      metadata->setOffset(session->offset);
      metadata->setLength(session->length);
      metadata->setTotalLength(data->length);
      metadata->setChecksumsArraySize(session->length);

      for (unsigned int i = session->offset; i < session->offset + session->length; i++) {
        metadata->setChecksums(i - session->offset, data->checksumAt(i));
      }

      send(metadata, "down$o");

      state = PRE_SEND_DATA;
      return;
    }
    case PRE_RECV_METADATA: {
      if (session == nullptr || session->length == 0) {
        // haven't received the metadata yet
        return;
      }

      state = PRE_RECV_DATA;
      break;
    }
    case PRE_SEND_DATA: {
      if (session->status == ddtp_SessionStatus::UNKNOWN) {
        // haven't received the status message yet
        return;
      }

      state = session->status == ddtp_SessionStatus::ACCEPTED ? SEND_DATA : STANDBY;
      break;
    }
    case PRE_RECV_DATA: {
      if (session->dst == address) {
        // only let the user accept a session
        // TODO: reject logic?
        ddtp_Status * status = new ddtp_Status();
        status->setCode(ddtp_StatusCodes::DDTP_ACCEPT);
        send(status, "up$o");
        state = RECV_DATA;
      }
      else if (flag_ForwardedStatus) {
        // TODO: reject logic?
        if (session->status == ddtp_SessionStatus::ACCEPTED) {
          state = RECV_DATA;
        }

        flag_ForwardedStatus = false;
      }
      break;
    }
    case SEND_DATA: {
      if (session->pendingBlocks.size() >= MAX_PENDING_BLOCKS) {
        // don't send any more blocks
        return;
      }

      while (session->pendingBlocks.size() < MAX_PENDING_BLOCKS) {
        char * blockData = data->blockAt(session->nextBlockToSend);

        if (blockData == nullptr) {
          if (session->pendingBlocks.size() == 0) {
            // we have no more blocks to send, we're done!
            session->destroy();
            delete session;
            session = nullptr;

            data->destroy();
            delete data;
            data = nullptr;

            state = STANDBY;
          }
          return;
        }

        ddtp_Block * blockMessage = new ddtp_Block();
        blockMessage->setNumber(session->nextBlockToSend);
        blockMessage->setDataArraySize(BYTES_PER_BLOCK);
        for (int i = 0; i < BYTES_PER_BLOCK; i++) {
          blockMessage->setData(i, blockData[i]);
        }
        send(blockMessage, "down$o");

        struct ddtp_PendingBlock pendingBlock;
        pendingBlock.number = session->nextBlockToSend;
        pendingBlock.time = simTime();
        session->pendingBlocks.push_back(pendingBlock);

        session->nextBlockToSend++;
      }

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
