package ddtp.simulations;

@license(LGPL);

simple DDTP {
  parameters:
    int address;
  gates:
    inout up @loose;
    inout down @loose;
}

simple GSSwitch {
  parameters:
    int numGS;
    int initialGS;
    string gsGatePrefix;
    string endGate;
  gates:
    inout up0 @loose;
    inout up1 @loose;
    inout up2 @loose;
    inout up3 @loose;
    inout up4 @loose;
    inout up5 @loose;
    inout up6 @loose;
    inout up7 @loose;
    inout up8 @loose;
    inout up9 @loose;
    inout down0 @loose;
    inout down1 @loose;
    inout down2 @loose;
    inout down3 @loose;
    inout down4 @loose;
    inout down5 @loose;
    inout down6 @loose;
    inout down7 @loose;
    inout down8 @loose;
    inout down9 @loose;
}

network OneToOne {
  submodules:
    satellite : DDTP {
      address = 0;
    }
    user : DDTP {
      address = 1;
    }
  connections:
    satellite.down <--> { delay = 1.4ms; } <--> user.up;
}

network OneSatOneUserTwoGS {
  submodules:
    satellite : DDTP {
      address = 0;
    }
    user : DDTP {
      address = 1;
    }

    sat2gsSwitch : GSSwitch {
      numGS = 2;
      initialGS = 0;
      endGate = "up0";
      gsGatePrefix = "down";
    }

    user2gsSwitch : GSSwitch {
      numGS = 2;
      initialGS = 0;
      endGate = "down0";
      gsGatePrefix = "up";
    }

    gs0 : DDTP {
      address = 2;
    }
    gs1 : DDTP {
      address = 3;
    }
  connections:
    satellite.down <--> { delay = 1.4ms; } <--> sat2gsSwitch.up0;

    sat2gsSwitch.down0 <--> { delay = 0ms; } <--> gs0.up;
    sat2gsSwitch.down1 <--> { delay = 0ms; } <--> gs1.up;

    gs0.down <--> { delay = 0ms; } <--> user2gsSwitch.up0;
    gs1.down <--> { delay = 0ms; } <--> user2gsSwitch.up1;

    user2gsSwitch.down0 <--> { delay = 0ms; } <--> user.up;
}
