cplusplus {{
  #include "packet-type_m.h"
  #include "ddtp_packet_m.h"
}}

enum ddtp_PacketType;
packet ddtp_packet;

packet ddtp_SessionInfo extends ddtp_packet {
  packetType = SESSION_INFO;
  unsigned int id;
  unsigned int src;
  unsigned int dst;
  unsigned int priority;
  bool isControl;
};
