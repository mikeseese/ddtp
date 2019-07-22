cplusplus {{
  #include "packet-type_m.h"
  #include "ddtp_packet_m.h"
}}

enum ddtp_PacketType;
packet ddtp_packet;

packet ddtp_ACK extends ddtp_packet {
  packetType = ACK;
  bool nack = false;
  unsigned int block;
};
