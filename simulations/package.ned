package ddtp.simulations;

@license(LGPL);

simple DDTP
{
    parameters:
        int address;

    gates:
        inout up @loose;
        inout down @loose;
}

simple GSSwitch
{
    parameters:
        int numGS;
        int initialGS;
        string gsGatePrefix;
        string endGate;
        double frameCorruptRate;
        double frameLostRate;
        bool deterministicErrors;
        @display("i=block/switch;q=queue");

    gates:
        inout up[];
        inout down[];
}

network OneToOne
{
    submodules:
        satellite: DDTP {
            address = 0;
        }
        user: DDTP {
            address = 1;
        }

    connections:
        satellite.down <--> {  delay = 1.4ms; } <--> user.up;
}

network OneSatOneUserTwoGS
{
    @display("bgb=413.53668,467.9267");

    submodules:
        satellite: DDTP {
            @display("p=193.38667,42.303333");
            address = 0;
        }

        user: DDTP {
            address = 1;
            @display("p=193.38667,426.4867");
        }

        sat2gsSwitch: GSSwitch {
            parameters:
                numGS = 2;
                initialGS = 0;
                endGate = "up[0]";
                gsGatePrefix = "down";
                frameCorruptRate = 0;
                frameLostRate = 0;
                deterministicErrors = true;

                @display("p=193.38667,141.58667");
            gates:
                up[1];
                down[2];
        }

        user2gsSwitch: GSSwitch {
            parameters:
                numGS = 2;
                initialGS = 0;
                endGate = "down[0]";
                gsGatePrefix = "up";
                frameCorruptRate = 0;
                frameLostRate = 0;
                deterministicErrors = true;

                @display("p=193.38667,329.79333");
            gates:
                up[2];
                down[1];
        }

        gs0: DDTP {
            @display("p=88.92334,233.96335");
            address = 2;
        }

        gs1: DDTP {
            @display("p=305.62003,233.96335");
            address = 3;
        }

    connections:
        satellite.down <--> { delay = 1.4ms; } <--> sat2gsSwitch.up[0];

        sat2gsSwitch.down[0] <--> { delay = 0ms; } <--> gs0.up;
        sat2gsSwitch.down[1] <--> { delay = 0ms; } <--> gs1.up;

        gs0.down <--> { delay = 0ms; } <--> user2gsSwitch.up[0];
        gs1.down <--> { delay = 0ms; } <--> user2gsSwitch.up[1];

        user2gsSwitch.down[0] <--> { delay = 0ms; } <--> user.up;
}
