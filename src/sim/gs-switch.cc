#include "gs-switch.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include "../messages/messages.h"

void GSSwitch::initialize() {
  numGS = par("numGS").intValue();
  current = par("initialGS").intValue();
  losesConnection = par("losesConnection").boolValue();
  gsGatePrefix = par("gsGatePrefix").stdstringValue();
  endGate = par("endGate").stdstringValue();
  frameCorruptRate = par("frameCorruptRate").doubleValue();
  frameLostRate = par("frameLostRate").doubleValue();
  deterministicErrors = par("deterministicErrors").boolValue();

  if (deterministicErrors) {
    srand(1337);
  }
  else {
    srand(time(NULL));
  }

  DisplayConnection();
}

void GSSwitch::handleMessage(cMessage *msg) {
  std::string gateName = msg->getArrivalGate()->getName();

  bool isCorrupt = ((double) rand() / RAND_MAX) < frameCorruptRate;
  bool isLost = ((double) rand() / RAND_MAX) < frameLostRate;

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

  if (gateName.compare(endGate + "$i") == 0) {
    send(msg, (gsGatePrefix + "$o").c_str(), current);
  }
  else if (gateName.compare(gsGatePrefix + "$i") == 0) {
    send(msg, (endGate + "$o").c_str(), 0);
  }
}

void GSSwitch::DisplayConnection() {
  if (losesConnection) {
    for (int i = 0; i < numGS; i++) {
      cDisplayString & inStr = gateHalf(gsGatePrefix.c_str(), cGate::INPUT, i)->getDisplayString();
      cDisplayString & outStr = gateHalf(gsGatePrefix.c_str(), cGate::OUTPUT, i)->getDisplayString();
      if (i == current) {
        inStr.parse("ls=,1");
        outStr.parse("ls=,1");
      }
      else {
        inStr.parse("ls=,0");
        outStr.parse("ls=,0");
      }
    }
  }
}

unsigned int GSSwitch::ActivateSwitch() {
  current = current % numGS;

  DisplayConnection();

  return current;
}

unsigned int GSSwitch::ActivateSwitch(unsigned int next) {
  current = next;

  DisplayConnection();

  return current;
}
