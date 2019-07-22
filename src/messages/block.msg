cplusplus {{
  #include "packet-type_m.h"
  #include "ddtp_packet_m.h"
}}

enum ddtp_PacketType;
packet ddtp_packet;

packet ddtp_Block extends ddtp_packet {
  packetType = BLOCK;
  char data[];
};
