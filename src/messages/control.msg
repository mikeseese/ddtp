cplusplus {{
  #include "packet-type_m.h"
  #include "ddtp_packet_m.h"
}}

enum ddtp_PacketType;
packet ddtp_packet;

packet ddtp_Control extends ddtp_packet {
  packetType = CONTROL;
  bool fin;
  unsigned int snack[];
  unsigned int ack[];
};
