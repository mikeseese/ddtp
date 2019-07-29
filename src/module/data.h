#ifndef DDTP_DATA_H
#define DDTP_DATA_H

#include <vector>

const unsigned int BYTES_PER_BLOCK = 32;

class ddtp_Data {
 public:
  ddtp_Data(unsigned int length, bool initializeData);
  unsigned int length;
  char * blockAt(unsigned int number);
  bool blockRecieved(unsigned int number);
  void setBlockAt(unsigned int number, char * block);
  unsigned int checksumAt(unsigned int number);
  void setChecksumAt(unsigned int number, unsigned int crc);
  void destroy();

 private:
  std::vector<char *> blocks;
  std::vector<bool> blocksReceived;
  std::vector<unsigned int> checksums;
};

#endif
