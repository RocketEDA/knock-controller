# knock-controller
Open source knock controller


## How it works
At the front, signals are converted to MCU compatible (0-5V) levels. C1 decouples the signal such that rpm noise is removed ("windowing"). The total resulting gain is 0.1
![image](https://github.com/RocketEDA/knock-controller/assets/38387138/ef4cfa9b-611e-4aa8-b145-d4a6e383f3c6)
The upper section is a peak detector. This allows the small pulse to be held long enough such that the ecu's logger has a chance to log it. log_sync requires a pulse every log cycle to reset the peak detector. For speeduino this can be done [here](https://github.com/noisymime/speeduino/blob/df78f5109c86cd2a1e9314138959738d1a33f039/speeduino/logger.cpp#L20) by adding digitalWrite(high) and subsequent digitalWrite(low) at line 40, in case 14.
<img width="799" alt="image" src="https://github.com/RocketEDA/knock-controller/assets/38387138/6a8d200b-f862-4178-8001-0986ce66afa8">
During runtime, the retarder signal is what saves the engine. Taking the output directly from the INA, should the signal be greater than the adjustable "retard trigger level" it will instantly fire a signal at retarder_out. RV2 allows adjusting the duration the retarder will be held on, at 500k it should be around 70s. Once C10 discharges past 1V (pin 4 of U6), the signal will turn off.
<img width="1112" alt="image" src="https://github.com/RocketEDA/knock-controller/assets/38387138/6628e9aa-f5ed-48f2-8071-2ea13db0106f">


V1 design work has been completed, waiting on parts for debugging

goal setting

Phase I: Primitive knock system. 
- use decoupling capacitor to remove the DC component from the piezo sensor, ambient noise from the engine running
- sample and hold knock long enough for speeduino to datalog
- comparator setup to:
  - output a retarder signal for ignition retard, like nitrous retard on speeduino, to protect engine instantly when knock
