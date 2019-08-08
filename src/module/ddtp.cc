#include "ddtp.h"
#include "../utils/CRC.h"
#include <stdio.h>

void DDTP::initialize() {
  state = ddtp_State::STANDBY;
  session = nullptr;
  numSessions = 0;
  address = par("address").intValue();
  data = nullptr;

  flag_StartingTransmission = false;
  flag_ReceivedTransmission = false;
  flag_ForwardedStatus = false;

  nextHeartbeat = SimTime(500, SIMTIME_MS);
  scheduleAt(nextHeartbeat, new ddtp_StateHeartbeat());
}

void DDTP::handleMessage(cMessage *msg) {
  ddtp_packet * pk = check_and_cast<ddtp_packet *>(msg);

  if (
    session == nullptr &&
    pk->getPacketType() != SESSION_INFO &&
    pk->getPacketType() != STATE_HEARTBEAT
  ) {
    if (state == PRE_RECV_SESSION_INFO) {
      // this is some extra data that got sent before we sent
      // the reject, ignore it
      return;
    }

    // we don't have a session as of yet
    flag_ReceivedTransmission = true;

    // make sure we don't keep broadcasint someone elses reject/no session message
    if (
      pk->getPacketType() != STATUS ||
      static_cast<ddtp_StatusCodes>(check_and_cast<ddtp_Status *>(msg)->getCode()) != DDTP_REJECT_NO_SESSION
    ) {
      // let others know this session is unknown by this node
      if (gate("down$o")->isConnected()) {
        ddtp_Status * reject = new ddtp_Status();
        reject->setCode(ddtp_StatusCodes::DDTP_REJECT_NO_SESSION);
        send(reject, "down$o");
      }
      if (gate("up$o")->isConnected()) {
        ddtp_Status * reject = new ddtp_Status();
        reject->setCode(ddtp_StatusCodes::DDTP_REJECT_NO_SESSION);
        send(reject, "up$o");
      }
    }

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
        char statusCode = static_cast<ddtp_StatusCodes>(status->getCode());

        if (statusCode == DDTP_REJECT_NO_SESSION) {
          // a node is telling us the session is borked
          // because it doesnt have it. we need a new session
          // lets make sure our data stays entact though

          // although, lets not make a new session if we're already
          // doing this, duh
          if (state == PRE_SEND_SESSION_INFO) {
            GetNextState();
            return;
          }

          // make sure others know about this
          if (strcmp(msg->getArrivalGate()->getName(), "down$i") == 0) {
            if (gate("up$o")->isConnected()) {
              send(status, "up$o");
            }
          }
          else {
            if (gate("down$o")->isConnected()) {
              send(status, "down$o");
            }
          }

          ddtp_Session * nextSession = nullptr;

          if (session->src == address) {
            // we are the source, lets craft a new session
            unsigned int offset = session->nextBlockToSend;
            if (session->pendingBlocks.size() > 0) {
              // we may have not received ack's for some blocks yet
              // the front of the list should be the earliest block
              offset = session->pendingBlocks.at(0).number;
            }
            nextSession = new ddtp_Session(session->src, session->dst, offset, session->length - offset, false);
            nextSession->id = ++numSessions;
            flag_StartingTransmission = true;
            state = STANDBY;
          }
          else {
            state = PRE_RECV_SESSION_INFO;
          }

          if (session) {
            session->destroy();
            delete session;
            session = nextSession;
          }

          GetNextState();
          return;
        }

        if (statusCode == DDTP_ACCEPT) {
          session->status = ACCEPTED;
        }
        else {
          session->status = REJECTED;
        }

        if (address != session->src) {
          // bubble it up
          send(status, "up$o");

          flag_ForwardedStatus = true;
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
        ack->setBlock(block->getNumber());
        if (crc != this->data->checksumAt(block->getNumber())) {
          // crc didnt match, nack it
          ack->setNack(true);
          send(ack, "up$o");
        } else {
          // crc matched, mark it
          this->data->setBlockAt(block->getNumber(), data);

          if (address != session->dst) {
            // bubble it down
            send(block, "down$o");
          }
          else {
            // send an ack that we got it
            std::cout << "Need Blocks: ";
            for (unsigned int i = 0; i < this->data->length; i++) {
              if (!this->data->blockRecieved(i)) {
                std::cout << i << ", ";
              }
            }
            std::cout << std::endl;
            send(ack, "up$o");
          }
        }
      }
    }
    case STATE_HEARTBEAT: {
      switch (state) {
        case STANDBY: {
          if (data == nullptr && session == nullptr && address == SATELLITE_ADDR && simTime() < SimTime(510, SIMTIME_MS)) {
            StartRandomTransmission(USER_ADDR, PAYLOAD_LENGTH);
          }

          // TODO: check if we have a new connection?
          break;
        }
        case SEND_DATA: {
          // check if its been awhile since we've seen any blocks
          for (auto itr = session->pendingBlocks.begin(); itr != session->pendingBlocks.end(); itr++) {
            if (itr->time <= simTime() - SimTime(5, SIMTIME_MS)) {
              // let's resend the block
              char * blockData = data->blockAt(itr->number);

              ddtp_Block * block = new ddtp_Block();
              block->setNumber(itr->number);
              block->setDataArraySize(BYTES_PER_BLOCK);
              for (int i = 0; i < BYTES_PER_BLOCK; i++) {
                block->setData(i, blockData[i]);
              }

              char buf[50];
              sprintf(buf, "Block #%d Timeout; Resending!", block->getNumber());
              bubble(buf);
              send(block, "down$o");
              break;
            }
          }
          break;
        }
      }

      simtime_t potentialNextHeartbeat = simTime() + SimTime(HEARTBEAT_US, SIMTIME_US);
      if (potentialNextHeartbeat - nextHeartbeat >= SimTime(HEARTBEAT_US, SIMTIME_US)) {
        nextHeartbeat = potentialNextHeartbeat;
        ddtp_StateHeartbeat * nextBeat = new ddtp_StateHeartbeat();
        scheduleAt(nextHeartbeat, nextBeat);
      }
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
