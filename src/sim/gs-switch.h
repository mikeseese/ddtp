#ifndef GS_SWITCH_H
#define GS_SWITCH_H

#include <omnetpp.h>
#include <string.h>
#include <string>

using namespace omnetpp;

class GSSwitch : public cSimpleModule {
 protected:
  virtual void initialize() override;
  virtual void handleMessage(cMessage *msg) override;
  // virtual void activity() override;

 private:
  unsigned int numGS;
  unsigned int current;
  std::string gsGatePrefix;
  std::string endGate;
  double frameCorruptRate;
  double frameLostRate;
  bool deterministicErrors;

  void DisplayConnection();

 public:
  unsigned int ActivateSwitch();
  unsigned int ActivateSwitch(unsigned int next);
};

Define_Module(GSSwitch);

#endif
