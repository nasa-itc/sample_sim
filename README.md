# Sample - NOS3 Simulator

This repository contains the Sample NOS3 Simulator.

## Overview
The example assumes a UART based device that streams telemetry at a fixed rate.
A single configuration command is recognized that allows modifying the streaming rate.
The device confirms receipt of a valid command by echoing the data back.

## Example Configuration
The default configuration returns a payload that is two times the request count:
```
<simulator>
    <name>sample_sim</name>
    <active>true</active>
    <library>libsample_sim.so</library>
    <hardware-model>
        <type>SAMPLE</type>
        <connections>
            <connection><type>command</type>
                <bus-name>command</bus-name>
                <node-name>sample-sim-command-node</node-name>
            </connection>
            <connection><type>usart</type>
                <bus-name>usart_29</bus-name>
                <node-port>29</node-port>
            </connection>
            <connection><type>period</type>
                <init-time-seconds>5.0</init-time-seconds>
                <ms-period>1000</ms-period>
            </connection>
        </connections>
        <data-provider>
            <type>SAMPLE_PROVIDER</type>
        </data-provider>
    </hardware-model>
</simulator>
```

Optionally the 42 data provider can be used:
```
        <data-provider>
            <type>SAMPLE_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4242</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
```

## Documentation
Currently support exists for the following versions:
* [Sample Link](https://cfs.gsfc.nasa.gov/)

Vendor repositories: 
* https://code.nasa.gov/

### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
