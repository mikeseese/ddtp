#include "gs-switch.h"
#include <string>

void GSSwitch::initialize() {
  numGS = par("numGS").intValue();
  current = par("initialGS").intValue();
  gsGatePrefix = par("gsGatePrefix").stringValue();
  endGate = par("endGate").stringValue();
}

void GSSwitch::handleMessage(cMessage *msg) {
  std::string port = gsGatePrefix + std::to_string(current);
  std::string gateName = msg->getArrivalGate()->getName();
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
}
