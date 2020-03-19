# Sample - NOS3 Simulator

This repository contains the Sample NOS3 Simulator.

## Overview
The example assumes a UART based device that streams telemetry at a fixed rate.
A single configuration command is recognized that allows modifying the streaming rate.
The device confirms receipt of command by echoing the data back.
The latest streamed data is available upon request via a command.
All streamed data is packed into a larger telemetry packet to be sent to the ground.

## Documentation
Currently support exists for the following versions:
* [Sample Link](https://cfs.gsfc.nasa.gov/)

Vendor repositories: 
* https://code.nasa.gov/

### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
