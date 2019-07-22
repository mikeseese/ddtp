cplusplus {{
  #include "packet-type_m.h"
  #include "ddtp_packet_m.h"
}}

enum ddtp_PacketType;
packet ddtp_packet;

packet ddtp_Metadata extends ddtp_packet {
  packetType = METADATA;
  unsigned int offset;
  unsigned int length;
  unsigned int totalLength;
  unsigned int checksums[];
};
