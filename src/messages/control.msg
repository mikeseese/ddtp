packet ddtp_Control {
  bool fin;
  unsigned int snack[];
  unsigned int ack[];
};
