#include "data.h"
#include <cstdlib>
#include <cstring>
#include "../utils/CRC.h"

ddtp_Data::ddtp_Data(unsigned int length, bool initializeData) {
  this->length = length;
  blocks.resize(length);
  checksums.resize(length);

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

    blocks.at(i) = block;
    checksums.at(i) = crc;
  }
}

char * ddtp_Data::blockAt(unsigned int number) {
  if (number >= blocks.size()) {
    return nullptr;
  }

  return blocks.at(number);
}

void ddtp_Data::setBlockAt(unsigned int number, char * block) {
  memcpy(&blocks.at(number), block, sizeof(char) * BYTES_PER_BLOCK);
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
