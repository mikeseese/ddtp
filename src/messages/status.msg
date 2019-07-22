cplusplus {{
  #include "packet-type_m.h"
  #include "ddtp_packet_m.h"
}}

enum ddtp_PacketType;
packet ddtp_packet;

enum ddtp_StatusCodes {
  DDTP_ACCEPT = 0;
  DDTP_REJECT_NO_SPACE = 1;
  DDTP_REJECT_POWER_SAVING_MODE = 2;
  DDTP_REJECT_BUSY = 3;
};

packet ddtp_Status extends ddtp_packet {
  packetType = STATUS;
  char code;
};
