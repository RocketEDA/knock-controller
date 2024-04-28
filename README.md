# knock-controller
Open source knock controller

Phase I: Primitive knock system. 
- Measure RPM
- Measure knock
- Monitor knock with noise floor filtering via RPM
- create noise floor reference table, "record" noise vs rpm
- output a retarder signal for ignition retard, like nitrous retard on speeduino, to protect engine when knock
- output a knock amount signal in conjunction with retarder signal for tuning
