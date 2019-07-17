enum ddtp_StatusCodes {
  DDTP_ACCEPT = 0;
  DDTP_REJECT_NO_SPACE = 1;
  DDTP_REJECT_POWER_SAVING_MODE = 2;
  DDTP_REJECT_BUSY = 3;
};

packet ddtp_Status {
  char code;
};
