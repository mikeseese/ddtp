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
        bool losesConnection;
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

network OneSatOneUserTwoGS
{
    @display("bgb=413.53668,467.9267");

    submodules:
        satellite: DDTP {
            @display("p=193.38667,42.303333;i=device/satellite;is=vl");
            address = 0;
        }

        user: DDTP {
            address = 1;
            @display("p=193.38667,426.4867;i=device/laptop;is=l");
        }

        sat2gsInterface: GSSwitch {
            parameters:
                numGS = 2;
                initialGS = 0;
                losesConnection = true;
                endGate = "up";
                gsGatePrefix = "down";
                frameCorruptRate = 0;
                frameLostRate = 0;
                deterministicErrors = true;

                @display("p=193,135;i=block/ifcard");
            gates:
                up[1];
                down[2];
        }

        user2gsSwitch: GSSwitch {
            parameters:
                numGS = 2;
                initialGS = 0;
                losesConnection = false;
                endGate = "down";
                gsGatePrefix = "up";
                frameCorruptRate = 0;
                frameLostRate = 0;
                deterministicErrors = true;

                @display("p=193,319");
            gates:
                up[2];
                down[1];
        }

        gs0: DDTP {
            @display("p=75,319;i=device/receiverdish;is=l");
            address = 2;
        }

        gs1: DDTP {
            @display("p=310,319;i=device/receiverdish;is=l");
            address = 3;
        }

    connections:
        satellite.down <--> {  delay = 1.4ms; } <--> sat2gsInterface.up[0];

        sat2gsInterface.down[0] <--> {  delay = 0ms; } <--> gs0.up;
        sat2gsInterface.down[1] <--> {  delay = 0ms; } <--> gs1.up;

        gs0.down <--> {  delay = 0ms; } <--> user2gsSwitch.up[0];
        gs1.down <--> {  delay = 0ms; } <--> user2gsSwitch.up[1];

        user2gsSwitch.down[0] <--> {  delay = 0ms; } <--> user.up;
}
