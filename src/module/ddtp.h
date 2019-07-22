#ifndef DDTP_H
#define DDTP_H

#include <omnetpp.h>
#include <string.h>
#include "state.h"
#include "session.h"
#include "../messages/messages.h"

using namespace omnetpp;

class DDTP : public cSimpleModule {
 protected:
  virtual void initialize() override;
  virtual void handleMessage(cMessage *msg) override;
  // virtual void activity() override;

 private:
  unsigned int numSessions;
  unsigned int address;
  ddtp_State state;
  ddtp_Session * session;

  void GetNextState();

 public:
  bool StartRandomTransmission(unsigned int dstAddress, unsigned int length);

  bool flag_StartingTransmission;
  bool flag_ReceivedTransmission;
};

Define_Module(DDTP);

#endif
