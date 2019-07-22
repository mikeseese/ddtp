#include "ddtp.h"

void DDTP::initialize() {
  state = ddtp_State::STANDBY;
  session = nullptr;
  numSessions = 0;
  // TODO: address

  flag_StartingTransmission = false;
  flag_ReceivedTransmission = false;
}

void DDTP::handleMessage(cMessage *msg) {
  ddtp_packet * pk = check_and_cast<ddtp_packet *>(msg);

  if (session == nullptr && pk->getPacketType() != SESSION_INFO) {
    // we don't have a session as of yet
    flag_ReceivedTransmission = true;
    GetNextState();
    return;
  }

  switch (pk->getPacketType()) {
    case ACK: {
      ddtp_ACK * ack = check_and_cast<ddtp_ACK *>(msg);
      break;
    }
    case CONTROL: {
      ddtp_Control * control = check_and_cast<ddtp_Control *>(msg);
      break;
    }
    case METADATA: {
      ddtp_Metadata * metadata = check_and_cast<ddtp_Metadata *>(msg);
      break;
    }
    case SESSION_INFO: {
      ddtp_SessionInfo * sessionInfo = check_and_cast<ddtp_SessionInfo *>(msg);
      break;
    }
    case STATUS: {
      ddtp_Status * status = check_and_cast<ddtp_Status *>(msg);
      break;
    }
  }
}

bool DDTP::StartRandomTransmission(unsigned int dstAddress, unsigned int length) {
  if (state != ddtp_State::STANDBY) {
    // we're not in a valid state to do this
    return false;
  }

  if (session) {
    session->destroy();
    delete session;
    session = nullptr;
  }

  session = new ddtp_Session(address, dstAddress, length, false);
  session->id = ++numSessions;

  flag_StartingTransmission = true;

  GetNextState();

  return true;
}
