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

  // TODO: uhhhh. ya. this is not ideal lol
  scheduleAt(SimTime(550, SIMTIME_MS), new cMessage("switch"));
}

void GSSwitch::handleMessage(cMessage *msg) {
  if (strcmp(msg->getName(), "switch") == 0) {
    ActivateSwitch();
    return;
  }

  std::string gateName = msg->getArrivalGate()->getName();

  ddtp_packet * pk = check_and_cast<ddtp_packet *>(msg);
  if (pk->getPacketType() == BLOCK) {
    bool isCorrupt = ((double) rand() / RAND_MAX) < frameCorruptRate;
    bool isLost = ((double) rand() / RAND_MAX) < frameLostRate;
    ddtp_Block * block = check_and_cast<ddtp_Block *>(msg);

    if (isLost) {
      char buf[50];
      sprintf(buf, "Block #%d Lost!", block->getNumber());
      bubble(buf);
      return;
    }

    if (isCorrupt) {
      char buf[50];
      sprintf(buf, "Block #%d Corrupted!", block->getNumber());
      bubble(buf);
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
  current = (current + 1) % numGS;

  DisplayConnection();

  char buf[50];
  sprintf(buf, "Emulating data link switch to GS%d", current);
  bubble(buf);

  return current;
}

unsigned int GSSwitch::ActivateSwitch(unsigned int next) {
  current = next;

  DisplayConnection();

  return current;
}
