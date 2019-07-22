#include "session.h"

ddtp_Session::ddtp_Session(
  unsigned int src,
  unsigned int dst,
  unsigned int offset,
  unsigned int length,
  bool isControl
) {
  this->src = src;
  this->dst = dst;
  this->offset = offset;
  this->length = length;
  this->isControl = isControl;
  this->blocks = new ddtp_BlockState[length];
  this->status = ddtp_SessionStatus::UNKNOWN;

  this->nextBlockToSend = offset;
}

void ddtp_Session::destroy() {
  delete this->blocks;
}
