package ddtp.simulations;

@license(LGPL);

DDTP node {
  gates:
    inout up;
    inout down;
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
    satellite.down <-> { delay = 10ms } <=>user.up;
}
