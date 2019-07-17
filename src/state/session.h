#ifndef DDTP_SESSION_H
#define DDTP_SESSION_H

#include "block-state.h";

class ddtp_Session {
public:
  unsigned int id;
  unsigned int src;
  unsigned int dst;
  unsigned int length;
  ddtp_BlockState * blocks;
};

#endif