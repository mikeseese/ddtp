#ifndef DDTP_SESSION_H
#define DDTP_SESSION_H

#include "block-state.h"
#include "session-status.h"
#include <vector>
#include <omnetpp.h>

struct ddtp_PendingBlock {
  unsigned int number;
  omnetpp::simtime_t time;
};

class ddtp_Session {
public:
  ddtp_Session(
    unsigned int src,
    unsigned int dst,
    unsigned int offset,
    unsigned int length,
    bool isControl
  );

  unsigned int id;
  unsigned int src;
  unsigned int dst;
  unsigned int offset;
  unsigned int length;
  bool isControl;
  ddtp_SessionStatus status;
  ddtp_BlockState * blocks;
  unsigned int nextBlockToSend;
  std::vector<ddtp_PendingBlock> pendingBlocks;

  void destroy();
};

#endif