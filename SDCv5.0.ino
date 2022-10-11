/**
 * Naive method --> Does not take advantage of function calling (i.e. brute coding)
 * Drive Wheel & Flywheel Code combined
 * *Commented
 * //Callibrate???
 */

//Define pins to receive signal from Spektrum DXe Receiver (NOTE: MUST BE DIGITAL INTERRUPT PINS SPECIFIED BY ARDUINO MEGA)
#define CHANNEL_1_PIN 2 //RDW - ELEV
#define CHANNEL_2_PIN 3 //LDW - THRO 
#define CHANNEL_3_PIN 19 //Flywheel - GEAR wire same way
#define CHANNEL_4_PIN 20 //Flipper

/**********************NAMING CONVENTION***********************
 * RDW: Right Drive Wheel
 * LDW: Left Drive Wheel
 * RFW: Right Flywheel
 * LFW: Left Flywheel
 **************************************************************/

volatile unsigned long timer_start;
volatile unsigned long timer_start2;
volatile unsigned long timer_start3;
volatile unsigned long timer_start4;

volatile int last_interrupt_time;
volatile int last_interrupt_time2;  
volatile int last_interrupt_time3; 
volatile int last_interrupt_time4; 

volatile int pulse_time; 
volatile int pulse_time2; 
volatile int pulse_time3;
volatile int pulse_time4;

const uint8_t address = 128;                        //Identifier for Motor Driver 1 (Drive wheels)
const uint8_t address2 = 129;                       //Identifier for Motor Driver 2 (Flywheels)
const uint8_t address3 = 130;  

uint8_t speed_RDW = 0;                                //Defines magnitude (i.e. speed) of motor) of right drive wheel motor
uint8_t speed_LDW = 0;                                //Defines magnitude of left drive wheel motor
uint8_t speed_FW = 0;                                 //Defines magnitude of flywheel motors (2)- same speed for both motors
uint8_t speed_flipper = 0;                                 //Defines magnitude of flipper

uint8_t dirRDW = 0;                                  //Defines direction (0 or 1) of right drive wheel motor: MOTOR 1
uint8_t dirLDW = 0;                                  //Defines direction (4 or 5) of left drive wheel motor: MOTOR 1
uint8_t dirRFW = 4;                                  //Defines direction (0 or 1) of right flywheel motor: MOTOR 2
uint8_t dirLFW = 4;                                  ////Defines direction (4 or 5) of left flywheel motor: MOTOR 2
uint8_t dirFlipper = 0;                               //Defines direction (0 or 1) of right flywheel motor

int speed1;
int speed2;
int speed3;

void calcSignal() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(CHANNEL_1_PIN) == HIGH) 
    { 
        timer_start = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(timer_start != 0)
        { 
            //record the pulse time
            pulse_time = ((volatile int)micros() - timer_start);
            //restart the timer
            timer_start = 0;
        }
    } 
} 

void calcSignal2() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(CHANNEL_2_PIN) == HIGH) 
    { 
        timer_start2 = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(timer_start2 != 0)
        { 
            //record the pulse time
            pulse_time2 = ((volatile int)micros() - timer_start2);
            //restart the timer
            timer_start2 = 0;
        }
    } 
}

void calcSignal3() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(CHANNEL_3_PIN) == HIGH) 
    { 
        timer_start3 = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(timer_start3 != 0)
        { 
            //record the pulse time
            pulse_time3 = ((volatile int)micros() - timer_start3);
            //restart the timer
            timer_start3 = 0;
        }
    } 
}

void calcSignal4() 
{
    //record the interrupt time so that we can tell if the receiver has a signal from the transmitter 
    last_interrupt_time = micros(); 
    //if the pin has gone HIGH, record the microseconds since the Arduino started up 
    if(digitalRead(CHANNEL_4_PIN) == HIGH) 
    { 
        timer_start4 = micros();
    } 
    //otherwise, the pin has gone LOW 
    else
    { 
        //only worry about this if the timer has actually started
        if(timer_start4 != 0)
        { 
            //record the pulse time
            pulse_time4 = ((volatile int)micros() - timer_start4);
            //restart the timer
            timer_start4 = 0;
        }
    } 
}

void setup() {
  
    pinMode(CHANNEL_1_PIN, INPUT_PULLUP); // Setting up input pin from receiver
    pinMode(CHANNEL_2_PIN, INPUT_PULLUP);
    pinMode(CHANNEL_3_PIN, INPUT_PULLUP);
    pinMode(CHANNEL_4_PIN, INPUT_PULLUP);
    
    timer_start = 0; 
    timer_start2 = 0;
    timer_start3 = 0;
    timer_start4 = 0;
    
    attachInterrupt(digitalPinToInterrupt(CHANNEL_1_PIN), calcSignal, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CHANNEL_2_PIN), calcSignal2, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CHANNEL_3_PIN), calcSignal3, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CHANNEL_4_PIN), calcSignal4, CHANGE);
    
    Serial.begin(9600);
    Serial1.begin(9600);
    
    delay(3000);          //Mandatory 3 second delay before any information is to be sent to the motor drivers
    
    Serial1.write(0xAA);
    
    Serial1.flush();
    Serial.flush();
    
    delay(1000);
}

void loop() {
    delay(20);

    if (pulse_time < 1080)
    {
      speed1 = 0;
    }
    else if (pulse_time > 1896)
    {
      speed1 = 100;
    }
    else if (pulse_time < 1096)
    {
      speed1 = -100;
    }
    else
    {
      int delta = pulse_time - 1496; // UPDATE: 1496 [p denotes nominal pulse time 
      speed1 = (delta/400.0) * 100.0; // UPDATE: 416 (previously 400) is the difference between max/min and nominal rating
    }
    
    //Serial.println(speed1);
    
    if (pulse_time2 < 1080)
    {
      speed2 = 0;
    }
    else if (pulse_time2 > 1896)
    {
      speed2 = 100;
    }
    else if(pulse_time2 < 1096)
    {
      speed2 = -100;
    }
    else
    {
      int delta2 = pulse_time2 - 1496; // UPDATE: 1504 [previously 1508] denotes nominal pulse time 
      speed2 = (delta2/400.0) * 100.0; // UPDATE: 416 (previously 400) is the difference between max/min and nominal rating
    }
    
    if (pulse_time3 < 1080)
    {
      speed3 = 0;
    }
    else if (pulse_time3 > 1900)
    {
      speed3 = 100;
    }
    else if (pulse_time3 < 1400)
    {
      speed3 = -100;
    }
    else{
      int delta3 = pulse_time3 - 1500; // Check nominal pulse time 
      speed3 = (delta3/400.0) * 100.0; // check +/- from nominal pulse time
    }

/*
    int delta4 = pulse_time4 - 1500; // Check nominal pulse time 
    int speed4 = (delta4/400.0) * 100.0; // check +/- from nominal pulse time
   
    //Serial.println(pulse_time4);
  */  
    if (speed1 > 0){
      speed1 = map(speed1,0,100,0,127);
      dirRDW = 0;
    }
    else if(speed1 < 0){
      speed1 = map(speed1,-100,0,127,0);
      dirRDW = 1;
    }
    else{
      speed1 = 0;
    }

    //Serial.println(speed1);
    speed_RDW = (uint8_t)speed1;

    if (speed2 > 0){
      speed2 = map(speed2,0,100,0,127);
      dirLDW = 1;
    }
    else if(speed2 < 0){
      speed2 = map(speed2,-100,0,127,0);
      dirLDW = 0;
    }
    else{
      speed2 = 0;
    }

    //Serial.println(speed2); 
    speed_LDW = (uint8_t)speed2;

    if (speed3 > 0){
      speed3 = map(speed3,0,100,0,127);
      dirRFW = 4;
      dirLFW = 4;
    }
    else if(speed3 < 0 ){
      speed3 = map(speed3,-100,0,127,0);
      speed3 *= .4;
      dirRFW = 5;
      dirLFW = 5;
    }
    else{
      speed3 = 0;
    }
    Serial.println(speed3);
    speed_FW = (uint8_t)speed3;

    

// *************** FLIPPER MOTOR ***************************
    if (speed4 > 2){
      speed4 = map(speed4,0,100,0,127);
      //speed4 *= .25;
      dirFlipper = 0;
    }
    else if(speed4 < 2){
      speed4 = map(speed4,-100,0,127,0);
      //speed4 *= .25;
      dirFlipper = 1;
    }
    else{
      speed4 = 0;
    }
    speed_flipper = (uint8_t)speed4;

   
/*
  Serial1.write(address);                              // determines which motor to control starting from 128; each address gives you access to 2 motors
  Serial1.write(dirLDW);                                    // 4 - command number means move 2nd motor forward, 5 - second motor backward (same for 1st motor - (0 & 1)
  Serial1.write(speed_LDW);                                // range from 0 - 127 (determines amount of power from battery that is transferred)
  Serial1.write((address + dirLDW + speed_LDW) & uint8_t(127));// REQUIREMENT: checkSUM - safety check; if incorrect, motor driver will ignore signal
  
  Serial1.write(address);                              // determines which motor to control starting from 128; each address gives you access to 2 motors
  Serial1.write(dirLFW);                                    // 4 - command number means move 2nd motor forward, 5 - second motor backward (same for 1st motor - (0 & 1)
  Serial1.write(speed_FW);                                // range from 0 - 127 (determines amount of power from battery that is transferred)
  Serial1.write((address + dirLFW + speed_FW) & uint8_t(127));// REQUIREMENT: checkSUM - safety check; if incorrect, motor driver will ignore signal

  Serial1.write(address2);                              // determines which motor to control starting from 128; each address gives you access to 2 motors
  Serial1.write(dirRDW);                                    // 4 - command number means move 2nd motor forward, 5 - second motor backward (same for 1st motor - (0 & 1)
  Serial1.write(speed_RDW);                                // range from 0 - 127 (determines amount of power from battery that is transferred)
  Serial1.write((address2 + dirRDW + speed_RDW) & uint8_t(127));// REQUIREMENT: checkSUM - safety check; if incorrect, motor driver will ignore signal

  Serial1.write(address2);                              // determines which motor to control starting from 128; each address gives you access to 2 motors
  Serial1.write(dirRFW);                                    // 4 - command number means move 2nd motor forward, 5 - second motor backward (same for 1st motor - (0 & 1)
  Serial1.write(speed_FW);                                // range from 0 - 127 (determines amount of power from battery that is transferred)
  Serial1.write((address2 + dirRFW + speed_FW) & uint8_t(127));// REQUIREMENT: checkSUM - safety check; if incorrect, motor driver will ignore signal
*/
/*
  Serial1.write(address3);                              // determines which motor to control starting from 128; each address gives you access to 2 motors
  Serial1.write(dirFlipper);                                    // 4 - command number means move 2nd motor forward, 5 - second motor backward (same for 1st motor - (0 & 1)
  Serial1.write(speed_flipper);                                // range from 0 - 127 (determines amount of power from battery that is transferred)
  Serial1.write((address3 + dirFlipper+ speed_flipper) & uint8_t(127));// REQUIREMENT: checkSUM - safety check; if incorrect, motor driver will ignore signal
*/

/*
    Serial.println("RJ: ");
    Serial.print("  DIR: ");
    Serial.println(dirRJ);
    Serial.print("  Speed: ");
    Serial.println(speed1);
 
    Serial.println("LJ :");
    Serial.print("  DIR: ");
    Serial.println(dirLJ);
    Serial.print("  Speed: ");
    Serial.println(speed2);
*/
}
