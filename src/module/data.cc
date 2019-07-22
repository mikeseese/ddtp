#include "data.h"
#include <cstdlib>
#include <cstring>
#include "../utils/CRC.h"

ddtp_Data::ddtp_Data(unsigned int length, bool initializeData) {
  this->length = length;
  blocks.resize(length);

  for (unsigned int i = 0; i < length; i++) {
    char * block = new char [BYTES_PER_BLOCK];
    unsigned int crc = 0;
    if (initializeData) {
      for (unsigned int j = 0; j < BYTES_PER_BLOCK; j++) {
        block[j] = rand();
      }

      crc = CRC::Calculate(block, sizeof(char) * BYTES_PER_BLOCK, CRC::CRC_32());
    } else {
      memset(block, 0, sizeof(char) * BYTES_PER_BLOCK);
    }

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

void ddtp_Data::setChecksumAt(unsigned int number, unsigned int crc) {
  checksums.at(number) = crc;
}

void ddtp_Data::destroy() {
  for (unsigned int i = 0; i < blocks.size(); i++) {
    delete blocks.at(i);
  }
}