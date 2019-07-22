#ifndef DDTP_SESSION_H
#define DDTP_SESSION_H

#include "block-state.h"

class ddtp_Session {
public:
  ddtp_Session(unsigned int src, unsigned int dst, unsigned int length);

  unsigned int id;
  unsigned int src;
  unsigned int dst;
  unsigned int length;
  ddtp_BlockState * blocks;

  void destroy();
};

#endif