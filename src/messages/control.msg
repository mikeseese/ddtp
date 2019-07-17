packet ddtp_Control {
  bool fin;
  unsigned int numSnack;
  unsigned int numAck;
  unsigned int snack[];
  unsigned int ack[];
};
