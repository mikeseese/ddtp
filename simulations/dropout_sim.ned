//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

package ddtp.simulations;

simple cubesat
{
    gates:
        inout port;
}
simple ground_station
{
    gates:
        inout port[];
}
simple Switch
{
    parameters:
        double pkRate;
        int queueMaxLen;
        @display("i=block/switch;q=queue");
    gates:
        inout port[];
}
//
//
network dropout_sim
{
    @display("bgb=255,304");
    submodules:
        switch1: Switch {
            parameters:
                //pkRate = 1.5*numClients; // pkRate should be >= numClients, otherwise switch will become the bottleneck
                //queueMaxLen = 20; // buffer max 20 packets
            gates:
                //port[numClients+1];
                port[3];
        }
        switch2: Switch {
            parameters:

            gates:
                port[3];
        }

        sat: cubesat {
            @display("p=112,91");
        }
        GS1: ground_station {
            @display("p=40,233");
            gates:
                port[2];
        }
        GS2: ground_station {
            @display("p=175,222");
            gates:
                port[2];
        }

    connections:
        sat.port <--> {  delay = 10ms; } <--> switch1.port[1];
        //sat.out --> {  delay = 10ms; } --> GS2.in;
        switch1.port[2] <--> {  delay = 10ms; } <--> GS1.port[1];
        switch1.port[3] <--> {  delay = 10ms; } <--> GS2.port[1];

        GS1.port[2] <--> {  delay = 10ms; } <--> switch2.port[1];
        GS2.port[2] <--> {  delay = 10ms; } <--> switch2.port[2];
        //sat.in <-- {  delay = 10ms; } <-- GS2.out;
}
