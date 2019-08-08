# Distributed Delay Tolerant Protocol (DDTP)

This repository contains OMNeT++ modules for the DDTP protocol outlined by P. Gamiero and R. Rocha.

## Reference Paper
P. M. Gameiro and R. M. Rocha, "Distributed Delay Tolerant Protocol," 2018 15th International Symposium on Wireless Communication Systems (ISWCS), Lisbon, 2018, pp. 1-6.
doi: 10.1109/ISWCS.2018.8491228

## How to Use

### OMNeT++
You'll need to install OMNeT++ which requires building from source. You can read more how to do this in the [official installation guide](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf).

### Importing this Project
Once you have built OMNeT++, you can run/open the OMNeT++ IDE (instructions are in the [official installation guide](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf)). You can import the project in the IDE by clicking `File>Import>General>Existing Projets into Workspace`. Then click `Browse...` next to `Select root directory:` and select the root of the `ddtp` directory.

### Building
You should be able to compile by selecting `Project>Build Project`. If you cannot select `Build Project`, you may need to select the project in the `Project Explorer` pane (`Window>Show View>Project Explorer`).

### Running
You can run the simulation easily by clicking `Run>Debug`. You will be prompted for the first time what kind of launch you want. Select the OMNeT option.

From there, another window (titled `OMNeT++/Qtenv...`) will open which has a picture of the network. Run the simulation with `Simulate>Run`.

### Configuration
You can adjust the configuration by editing `simulations>package.ned`. Select the `Source` tab at the bottom of the editor window. Then you can change the `frameCorruptRate`, `frameLostRate`, `deterministicErrors` in the `GSSwitch` modules. You will need to stop and rerun the simulation with `Run>Debug` for the changes to be applied.
