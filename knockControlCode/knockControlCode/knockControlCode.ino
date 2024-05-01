#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <arduino-timer.h>

#define KNOCK_PIN PA0        //analog channel for measuring knock amount
#define KNOCK_TRIG_PIN PB0   //edge trigger for knock detection
#define RETARD_PIN PA7
#define RETARD_PIN_HOLD 500000 //how long to hold retard pin HIGH for after knock is detected

#define KNOCK_AVG_NUM 20

#define KNOCK_DAC_SAMPLE_INTERVAL 40000  //long enough such that the datalogger will catch the value

int knockPtr = 0;

float knock_buf = 0; //running average that gets reset once KNOCK_DAC_SAMPLE_INTERVAL is met
bool knockSampleRead = false;
uint16_t knocks[KNOCK_AVG_NUM];
Adafruit_MCP4725 dac;

Timer<5, micros> s_timer;

void setup() {

  Serial.begin(115200);
  analogReadResolution(12);
  Serial.println("hello");
  dac.begin(0x60);
  pinMode(RETARD_PIN, OUTPUT);
  pinMode(KNOCK_TRIG_PIN, INPUT);
  pinMode(PC13, OUTPUT);
  attachInterrupt(KNOCK_TRIG_PIN, knock_ISR, HIGH);
  s_timer.every(KNOCK_DAC_SAMPLE_INTERVAL, outputKnockMag);
  digitalWrite(PC13, HIGH);
}

void loop() {
  s_timer.tick();
}

bool activateTimers(void*)
{
  s_timer.every(KNOCK_DAC_SAMPLE_INTERVAL, outputKnockMag);
  return true;
}

/*
   outputKnockMag
   writes magnitude of knock to DAC
*/
bool outputKnockMag(void*)
{
  dac.setVoltage(knock_buf, false);
  Serial.println(knock_buf, 6);

  knock_buf = 0;
  return true;
}

bool retardReset(void*)
{
  digitalWrite(RETARD_PIN, LOW);   //retard pin low
  return true;
}

/*
   knock edge ISR
   toggles RETARD_PIN
*/
void knock_ISR()
{
  digitalWrite(RETARD_PIN, HIGH);   //retard pin
  s_timer.in(RETARD_PIN_HOLD, retardReset);  //resets retard pin
  sampleKnock();
}
/*
   knock sample
*/
void sampleKnock()
{
  knock_buf = analogRead(KNOCK_PIN);
}
