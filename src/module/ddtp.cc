#include "ddtp.h"

// send() to send stuff

void DDTP::initialize() {
  state = ddtp_State::STANDBY;

  flag_StartingTransmission = false;
  flag_ReceivedTransmission = false;
}

void DDTP::handleMessage(cMessage *msg) {
  ddtp_packet * pk = check_and_cast<ddtp_packet *>(msg);

  switch (pk->getPacketType()) {
    case ACK: {
      break;
    }
    case CONTROL: {
      break;
    }
    case METADATA: {
      break;
    }
    case SESSION_INFO: {
      break;
    }
    case STATUS: {
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
  }

  session = new ddtp_Session(address, dstAddress, length);

  flag_StartingTransmission = true;

  GetNextState();

  return true;
}
