#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <arduino-timer.h>

#define KNOCK_PIN PA0        //analog channel for measuring knock amount
#define KNOCK_TRIG_PIN PB0   //edge trigger for knock detection
#define RETARD_PIN PA7
#define RETARD_PIN_HOLD 1000000 //how long to hold retard pin HIGH for after knock is detected

#define KNOCK_AVG_NUM 20

#define KNOCK_DAC_SAMPLE_INTERVAL 40000  //long enough such that the datalogger will catch the value

int knockPtr = 0;
uint16_t knock_buf = 0;

uint16_t knocks[KNOCK_AVG_NUM];
bool pauseSampling = false; //puases adc sampling
Adafruit_MCP4725 dac;

Timer<5, micros> s_timer;

void setup() {
  Serial.begin(115200);
  Serial.println("hello");
  dac.begin(0x60);
  pinMode(RETARD_PIN, OUTPUT);
  pinMode(KNOCK_TRIG_PIN, INPUT);
  pinMode(PC13, OUTPUT);
  attachInterrupt(KNOCK_TRIG_PIN, knock_ISR, HIGH);
  s_timer.every(1, sampleKnock);
  s_timer.every(KNOCK_DAC_SAMPLE_INTERVAL, outputKnockMag);
  digitalWrite(PC13, HIGH);
}

void loop() {
  s_timer.tick();
}

bool activateTimers(void*)
{
  s_timer.every(1, sampleKnock);
  s_timer.every(KNOCK_DAC_SAMPLE_INTERVAL, outputKnockMag);
  return true;
}

/*
   outputKnockMag
   writes magnitude of knock to DAC
*/
bool outputKnockMag(void*)
{
  float knock_t = 0;
  for (int i = 0; i < KNOCK_AVG_NUM; i++)
  {
    knock_t += knocks[i];
  }
  knock_t = knock_t / KNOCK_AVG_NUM;
  dac.setVoltage(knock_t, false);
  Serial.println(knock_t, 6);
  return true;
}

bool retardReset(void*)
{
  digitalWrite(RETARD_PIN, LOW);   //retard pin low
  return true;
}

bool unpauseSampling(void*)
{
  //refill buffer with current
  for (int i = 0; i < KNOCK_AVG_NUM; i++)
  {
    knocks[i] = analogRead(KNOCK_PIN);
  }
  pauseSampling = false;
  return true;
}
/*
   knock edge ISR
   mag is written to DAC, average array is filled and calculated instantly
   enable RETARD_PIN
*/
void knock_ISR()
{
  /*
  pauseSampling = true;
  digitalWrite(RETARD_PIN, HIGH);   //retard pin
  //Serial.println("ISR");
  uint16_t knock_t = analogRead(KNOCK_PIN);
  //fill all of the knock buffer
  for (int i = 0; i < KNOCK_AVG_NUM; i++)
  {
    knocks[i] = knock_t;
  }
  outputKnockMag(0);
  s_timer.in(RETARD_PIN_HOLD, retardReset);  //resets retard pin
  s_timer.in(KNOCK_DAC_SAMPLE_INTERVAL, unpauseSampling);
  */
}
/*
   knock sample
*/
bool sampleKnock(void *)
{
  if (!pauseSampling)
  {
    knocks[getKnockPtr()] = analogRead(KNOCK_PIN);
  }
  return true;
}
/*
   updates current ptr and returns
*/
int getKnockPtr()
{
  if (knockPtr < KNOCK_AVG_NUM - 1)
  {
    knockPtr++;
  }
  else
  {
    knockPtr = 0;
  }
  return knockPtr;
}
