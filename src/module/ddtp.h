#ifndef DDTP_H
#define DDTP_H

#include <omnetpp.h>
#include <string.h>
#include "state.h"
#include "session.h"
#include "../messages/ddtp_packet_m.h"
#include "../messages/packet-type_m.h"

using namespace omnetpp;

class DDTP : public cSimpleModule {
 protected:
  virtual void initialize() override;
  virtual void handleMessage(cMessage *msg) override;
  // virtual void activity() override;

 private:
  unsigned int address;
  ddtp_State state;
  void GetNextState();
  ddtp_Session * session;

 public:
  bool StartRandomTransmission(unsigned int dstAddress, unsigned int length);

  bool flag_StartingTransmission;
  bool flag_ReceivedTransmission;
};

Define_Module(DDTP);

#endif
