#include "session.h"

ddtp_Session::ddtp_Session(unsigned int src, unsigned int dst, unsigned int length) {
  this->src = src;
  this->dst = dst;
  this->length = length;
  this->blocks = new ddtp_BlockState[length];
}

void ddtp_Session::destroy() {
  delete this->blocks;
}
