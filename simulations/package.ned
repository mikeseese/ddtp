package ddtp.simulations;

@license(LGPL);

simple DDTP {
  parameters:
    int address;
  gates:
    inout up @loose;
    inout down @loose;
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
    satellite.down <--> { delay = 10ms; } <--> user.up;
}
