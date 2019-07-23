#ifndef DDTP_H
#define DDTP_H

#include <omnetpp.h>
#include <string.h>
#include "state.h"
#include "session.h"
#include "data.h"
#include "../messages/messages.h"

using namespace omnetpp;

#define SATELLITE_ADDR 0
#define USER_ADDR 1
#define PAYLOAD_LENGTH 100
#define HEARTBEAT_US 10000

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
  ddtp_Data * data;
  simtime_t nextHeartbeat;

  void GetNextState();

 public:
  bool StartRandomTransmission(unsigned int dstAddress, unsigned int length);

  bool flag_StartingTransmission;
  bool flag_ReceivedTransmission;
};

Define_Module(DDTP);

#endif
