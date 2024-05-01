#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <arduino-timer.h>


#define rpm_pin 2
#define NOISE 20 //amount of background "knock" to add per 100 rpm, in 12 bit DAC steps
#define DAC_RESOLUTION (12)
#define KNOCK_CHANCE 100  //chance of knock per revolution e.g. 10 would be 1/10 chance, 20 would be 1/20 etc.
#define KNOCK_AMT_MAX 3000 //max knock signal (random)

uint32_t tHigh = 66;
uint32_t tLow = 16600;
uint32_t rpm = 2000;
float period = 16666;

Adafruit_MCP4725 dac;

Timer<3, micros> u_timer;


/*
   generate tachometer and knock signals
   2 pulses per crank revolution
*/
void setup()
{
  Serial.begin(115200);
  Serial.println("hello");
  dac.begin(0x60);
  Serial.println("hello");

  pinMode(rpm_pin, OUTPUT);
  Serial.println("enter rpm");
  u_timer.every(period, RpmSigPer);
  delay(tHigh);
  u_timer.every(period, setLow);
}

void loop()
{
  u_timer.tick();
}

void serialEvent() {
  if (Serial.available())
  {
    u_timer.cancel();
    String instr = Serial.readString();
    rpm = instr.toInt();
    period = 30000000.0 / float(rpm);
    tHigh = uint32_t(period * 0.01);
    tLow = uint32_t(period * 0.99);
    dac.setVoltage(constrain(NOISE * (rpm / 100), 0, 4095), false);
    u_timer.every(period, RpmSigPer);
    delay(tHigh);
    u_timer.every(period, setLow);
  }
}
/*
   run once every period
*/
bool RpmSigPer()
{
  if (1 == random(1, KNOCK_CHANCE))
  {
    //knock
    dac.setVoltage(constrain((NOISE * (rpm / 100)) + random(1000, KNOCK_AMT_MAX), 0, 4095), false);

  }
  digitalWrite(rpm_pin, HIGH);
  return true;
}

bool setLow()
{
  dac.setVoltage(constrain(NOISE * (rpm / 100), 0, 4095), false);
  digitalWrite(rpm_pin, LOW);
  return true;
}
