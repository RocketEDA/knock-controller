#include <arduino-timer.h>
#define TRIG_EDGE HIGH
#define RPM_PIN PB1
#define KNOCK_PIN A0
#define PPR 2.0 //pulse per revolution
#define RPM_AVG_NUM 30  //

uint32_t prevMicros = 0; //previous micros time
uint32_t rpm_period = 0; //rpm period
uint32_t rpm_pers[RPM_AVG_NUM];
int rpmPtr = 0;
float rpm = 0;    //current RPM
uint32_t t_micros = 0;  //buffer

Timer<1, micros> u_timer;
void setup() {
  Serial.begin(2000000);
  attachInterrupt(RPM_PIN, rpm_ISR, TRIG_EDGE);
  //run at the end of setup
  prevMicros = micros();
  u_timer.every(50000, printRPM);
}

void loop() {
  u_timer.tick();
}
bool printRPM(void *)
{
  calculateRPM();
  Serial.println(rpm, 6);
  return true;
}


/*
   calculateRPM
   RPM will be calculated into rpm
   take average from rpm_pers[]
*/
void calculateRPM()
{
  rpm = 0;
  for (int i = 0; i < RPM_AVG_NUM; i++)
  {
    rpm += rpm_pers[i];
  }
  rpm = ((60000000.0 / PPR) * RPM_AVG_NUM) / rpm;
}
/*
   ISR for rpm pulse
*/
void rpm_ISR()
{
  t_micros = micros();
  rpm_pers[getRpmPtr()] = t_micros - prevMicros;
  if (t_micros < prevMicros)
  {
    //overflow every 70 minutes. getRpmPtr updates once more. might cause issues in the future
    //we'll see
    rpm_pers[getRpmPtr()] = (0xffffffff - prevMicros) + t_micros;
  }
  prevMicros = t_micros;
}
/*
   updates current ptr and returns
*/
int getRpmPtr()
{
  if (rpmPtr < RPM_AVG_NUM - 1)
  {
    rpmPtr++;
  }
  else
  {
    rpmPtr = 0;
  }
  return rpmPtr;
}
