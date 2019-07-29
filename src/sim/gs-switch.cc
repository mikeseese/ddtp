#include "gs-switch.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include "../messages/messages.h"

void GSSwitch::initialize() {
  numGS = par("numGS").intValue();
  current = par("initialGS").intValue();
  gsGatePrefix = par("gsGatePrefix").stringValue();
  endGate = par("endGate").stringValue();
  frameCorruptRate = par("frameCorruptRate").doubleValue();
  frameLostRate = par("frameLostRate").doubleValue();
  deterministicErrors = par("deterministicErrors").boolValue();

  if (deterministicErrors) {
    srand(1337);
  }
  else {
    srand(time(NULL));
  }
}

void GSSwitch::handleMessage(cMessage *msg) {
  std::string port = gsGatePrefix + std::to_string(current);
  std::string gateName = msg->getArrivalGate()->getName();

  bool isCorrupt = ((double) rand() / RAND_MAX) <= frameCorruptRate;
  bool isLost = ((double) rand() / RAND_MAX) <= frameLostRate;

  if (isLost) {
    return;
  }

  if (isCorrupt) {
    ddtp_packet * pk = check_and_cast<ddtp_packet *>(msg);
    if (pk->getPacketType() == BLOCK) {
      ddtp_Block * block = check_and_cast<ddtp_Block *>(msg);
      char c = block->getData(0);
      block->setData(0, c + 1);
    }
  }

  if (gateName.compare(endGate) == 0) {
    send(msg, port.c_str());
  }
  else if (gateName.compare(port) == 0) {
    send(msg, endGate.c_str());
  }
}

unsigned int GSSwitch::ActivateSwitch() {
  current = current % numGS;

  return current;
}

unsigned int GSSwitch::ActivateSwitch(unsigned int next) {
  current = next;

  return current;
}
