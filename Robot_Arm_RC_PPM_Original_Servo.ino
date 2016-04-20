#include <VarSpeedServo.h>

#include <RCArduinoFastLib.h>

 // MultiChannels
//
// rcarduino.blogspot.com
//
// A simple approach for reading three RC Channels using pin change interrupts
//
// See related posts -
// http://rcarduino.blogspot.co.uk/2012/01/how-to-read-rc-receiver-with.html
// http://rcarduino.blogspot.co.uk/2012/03/need-more-interrupts-to-read-more.html
// http://rcarduino.blogspot.co.uk/2012/01/can-i-control-more-than-x-servos-with.html
//
// rcarduino.blogspot.com
// 
#define SERVO_FRAME_SPACE 6
/* 
 * 0 = Alieron 
   1 = Elevator
   2 = Throttle
   3 = Rudder
   4 = Aux A
   5 = Aux C
 */
uint8_t channels[] = {0,1,2,3,4,5};
/*
 *  4 = Base
 *  6 = Shoulder
 *  8 = Elbow
 *  10 = Wrist
 *  12 = Wrist Rotate
 *  13 = Gripper
 */

uint8_t servoPins[] = {12,8,6,4,10,13};
VarSpeedServo servos[6];
int numConnections = sizeof(channels)/sizeof(uint8_t) ;
uint16_t input = 0;

int prevPWM[] = {0,0,0,0,0,0};
int curPWM[] = {1500,1500,1500,1500,1500,1500};
int deltaPWM;


int servoMin[] = {500,  2100,  750,  2500, 2500, 1307};
int servoMax[] = {2500, 809, 2445, 500,  500, 2486};

int baseVal = 1448;



volatile uint32_t ulCounter = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Testing PPM.");
  Serial.print("Number of Connections : ");
  Serial.println(numConnections);

  // attach servo objects, these will generate the correct
  // pulses for driving Electronic speed controllers, servos or other devices
  // designed to interface directly with RC Receivers 

  for(int i = 0; i < numConnections; i++){
    servos[i].attach(servoPins[i],500,2500);
  }
  CRCArduinoPPMChannels::begin();
}

void loop()
{
  // Pass the signals straight through - 
  for(int i = 0; i < numConnections; i++){
    if(prevPWM[i] == 0){
      prevPWM[i] = map(CRCArduinoPPMChannels::getChannel(channels[i]),1000,2000,500,2500);
      continue;
    } else if(CRCArduinoPPMChannels::getChannel(channels[i])) {
      deltaPWM = map(CRCArduinoPPMChannels::getChannel(channels[i]),1000,2000,500,2500) - prevPWM[i];
      curPWM[i] = (abs(deltaPWM) > 5) ? curPWM[i] + deltaPWM : (i == 3) ? baseVal : curPWM[i];
      Serial.print("Sending PWM to servo with channel : ");
      Serial.print(channels[i]);
      Serial.print(" with PWM of : ");
      Serial.println(curPWM[i]);
      prevPWM[i] = curPWM[i];
      servos[i].writeMicroseconds(curPWM[i]);
    }
  }
  if(CRCArduinoPPMChannels::getSynchErrorCounter()){
    Serial.print("Synch Error Counter: ");
    Serial.println(CRCArduinoPPMChannels::getSynchErrorCounter());
  }
  delay(50);
}

