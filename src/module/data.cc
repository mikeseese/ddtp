#include "data.h"
#include <cstdlib>
#include "../utils/CRC.h"

ddtp_Data::ddtp_Data(unsigned int length) {
  blocks.resize(length);

  for (unsigned int i = 0; i < length; i++) {
    char * block = new char [BYTES_PER_BLOCK];
    for (unsigned int j = 0; j < BYTES_PER_BLOCK; j++) {
      block[j] = rand();
    }

    unsigned int crc = CRC::Calculate(block, sizeof(char) * BYTES_PER_BLOCK, CRC::CRC_32());

    blocks.push_back(block);
    checksums.push_back(crc);
  }
}

char * ddtp_Data::blockAt(unsigned int number) {
  return blocks.at(number);
}

unsigned int ddtp_Data::checksumAt(unsigned int number) {
  return checksums.at(number);
}

void ddtp_Data::destroy() {
  for (unsigned int i = 0; i < blocks.size(); i++) {
    delete blocks.at(i);
  }
}
