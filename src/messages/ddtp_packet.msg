cplusplus {{
  #include "packet-type_m.h"
}}

enum ddtp_PacketType;

packet ddtp_packet {
  int packetType @enum(ddtp_PacketType);
}