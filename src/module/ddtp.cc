#include "ddtp.h"
#include "../utils/CRC.h"

void DDTP::initialize() {
  state = ddtp_State::STANDBY;
  session = nullptr;
  numSessions = 0;
  address = par("address").intValue();
  data = nullptr;

  flag_StartingTransmission = false;
  flag_ReceivedTransmission = false;

  scheduleAt(SimTime(500, SIMTIME_MS), new ddtp_StateHeartbeat());
}

void DDTP::handleMessage(cMessage *msg) {
  ddtp_packet * pk = check_and_cast<ddtp_packet *>(msg);

  if (
    session == nullptr &&
    pk->getPacketType() != SESSION_INFO &&
    pk->getPacketType() != STATE_HEARTBEAT
  ) {
    // we don't have a session as of yet
    flag_ReceivedTransmission = true;
    GetNextState();
    return;
  }

  switch (pk->getPacketType()) {
    case ACK: {
      ddtp_ACK * ack = check_and_cast<ddtp_ACK *>(msg);

      if (session != nullptr) {
        if (ack->getNack()) {
          // we need to send the block again
          unsigned int blockNumber = ack->getBlock();
          char * blockData = data->blockAt(blockNumber);

          ddtp_Block * block = new ddtp_Block();
          block->setNumber(blockNumber);
          block->setDataArraySize(BYTES_PER_BLOCK);
          for (int i = 0; i < BYTES_PER_BLOCK; i++) {
            block->setData(i, blockData[i]);
          }

          send(block, "down$o");
        } else {
          // mark as acked
          for (auto itr = session->pendingBlocks.begin(); itr != session->pendingBlocks.end(); itr++) {
            if (itr->number == ack->getBlock()) {
              session->pendingBlocks.erase(itr);
              break;
            }
          }

          if (address != session->src) {
            // bubble it up
            send(ack, "up$o");
          }
        }
      }

      break;
    }
    case CONTROL: {
      ddtp_Control * control = check_and_cast<ddtp_Control *>(msg);
      break;
    }
    case METADATA: {
      ddtp_Metadata * metadata = check_and_cast<ddtp_Metadata *>(msg);

      if (session != nullptr) {
        session->offset = metadata->getOffset();
        session->length = metadata->getLength();

        if (data == nullptr) {
          data = new ddtp_Data(metadata->getTotalLength(), false);
        }

        for (unsigned int i = 0; i < metadata->getChecksumsArraySize(); i++) {
          data->setChecksumAt(i + session->offset, metadata->getChecksums(i));
        }

        if (address != session->dst) {
          // bubble it down
          send(metadata, "down$o");
        }
      }

      break;
    }
    case SESSION_INFO: {
      ddtp_SessionInfo * sessionInfo = check_and_cast<ddtp_SessionInfo *>(msg);

      if (session) {
        session->destroy();
        delete session;
        session = nullptr;
      }

      session = new ddtp_Session(sessionInfo->getSrc(), sessionInfo->getDst(), 0, 0, sessionInfo->getIsControl());

      if (address != session->dst) {
        // bubble it down
        send(sessionInfo, "down$o");
      }

      break;
    }
    case STATUS: {
      ddtp_Status * status = check_and_cast<ddtp_Status *>(msg);

      if (session != nullptr) {
        session->status = static_cast<ddtp_SessionStatus>(status->getCode());

        if (address != session->src) {
          // bubble it up
          send(status, "up$o");
        }
      }

      break;
    }
    case BLOCK: {
      ddtp_Block * block = check_and_cast<ddtp_Block *>(msg);

      if (session != nullptr) {
        // mark the data
        char * data = new char [BYTES_PER_BLOCK];
        for (unsigned int i = 0; i < BYTES_PER_BLOCK; i++) {
          data[i] = block->getData(i);
        }
        unsigned int crc = CRC::Calculate(data, sizeof(char) * BYTES_PER_BLOCK, CRC::CRC_32());

        ddtp_ACK * ack = new ddtp_ACK();
        if (crc != this->data->checksumAt(block->getNumber())) {
          // crc didnt match, nack it
          ack->setNack(true);
        } else {
          // crc matched, mark it
          this->data->setBlockAt(block->getNumber(), data);

          if (address != session->dst) {
            // bubble it down
            send(block, "down$o");
          }
        }

        send(ack, "up$o");
      }
    }
    case STATE_HEARTBEAT: {
      switch (state) {
        case STANDBY: {
          if (data == nullptr && session == nullptr && address == SATELLITE_ADDR) {
            StartRandomTransmission(USER_ADDR, PAYLOAD_LENGTH);
          }

          // TODO: check if we have a new connection?
          break;
        }
        case SEND_DATA: {
          // check if its been awhile since we've seen any blocks
          for (auto itr = session->pendingBlocks.begin(); itr != session->pendingBlocks.end(); itr++) {
            if (itr->time <= simTime() - SimTime(1000, SIMTIME_MS)) {
              // let's resend the block
              char * blockData = data->blockAt(itr->number);

              ddtp_Block * block = new ddtp_Block();
              block->setNumber(itr->number);
              block->setDataArraySize(BYTES_PER_BLOCK);
              for (int i = 0; i < BYTES_PER_BLOCK; i++) {
                block->setData(i, blockData[i]);
              }

              send(block, "down$o");
              break;
            }
          }
          break;
        }
      }

      ddtp_StateHeartbeat * nextBeat = new ddtp_StateHeartbeat();
      scheduleAt((simTime() + SimTime(HEARTBEAT_MS, SIMTIME_MS)), nextBeat);
    }
  }

  GetNextState();
}

bool DDTP::StartRandomTransmission(unsigned int dstAddress, unsigned int length) {
  if (state != ddtp_State::STANDBY) {
    // we're not in a valid state to do this
    return false;
  }

  data = new ddtp_Data(length, true);

  if (session) {
    session->destroy();
    delete session;
    session = nullptr;
  }

  session = new ddtp_Session(address, dstAddress, 0, length, false);
  session->id = ++numSessions;

  flag_StartingTransmission = true;

  return true;
}
