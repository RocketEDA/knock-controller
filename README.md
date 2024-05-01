# knock-controller
Open source knock controller

Phase I: Primitive knock system. 
- use decoupling capacitor to remove the DC component from the piezo sensor, ambient noise from the engine running
- sample and hold knock long enough for speeduino to datalog, output via MCP DAC
- comparator setup to:
  - instantaneously grab and output the signal to DAC
  - output a retarder signal for ignition retard, like nitrous retard on speeduino, to protect engine when knock

- non essential:
- Measure RPM
