/*
  For use with a 4 Pin Ultrasonic Ranging Sensor, see: http://www.micropik.com/PDF/HCSR04.pdf
  & an ATtiny85 Microprocessor
    
        Power can be supplied to the ATtiny85 via the Arduino
  VCC   //  5v  (+)
  GND   //  GND (-)
  
  TRIG  //  Analog Input 3 (Pin 3)
  ECHO  //  Analog Input 2 (Pin 4)
  LED //  Analog Input 1 (Pin 2)
  LED1  //  PWM (Pin 1)
  LED2  //  PWM (Pin 0)
        (c) Jacob Clark 2013 - http://fusionstrike.com
*/

//LEFT SIDE SENSOR ATTINY85

const int ledPin  =   0;
//const int ledPin1 =   1;
//const int ledPin2   =   2;
const int trigPin   =   3;
const int echoPin   =       4;


void setup() {
  pinMode(ledPin, OUTPUT);
 }

void loop(){


        long duration, inches, cm;
        
        pinMode(trigPin, OUTPUT);
        digitalWrite(trigPin, LOW);
        
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        pinMode(echoPin, INPUT);
        duration = pulseIn(echoPin, HIGH);
        
        
        cm = duration*0.034/2;
      
        if(cm <= 15){
          delay(420);
          digitalWrite(ledPin, HIGH);
       
        }
        
        else
        {
          digitalWrite(ledPin, LOW);
        }

}



