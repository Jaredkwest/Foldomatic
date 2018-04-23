#include <FoldStepper.h>
#include <FeedStepper.h>
#include <Servo.h>


const int enPinLeft = 13;
const int dirPinLeft = 12; 
const int stepPinLeft = 11; 
const int enPinRight = 10;
const int dirPinRight = 9; 
const int stepPinRight = 8; 
const int enPinCenter = 7;
const int dirPinCenter = 6; 
const int stepPinCenter = 4;
const int steamPin = 40; 




const int relayLeft1 = 22;
const int relayLeft2 = 24;
const int relayLeft3 = 26; 
const int relayLeft4 = 28; 
const int relayRight1 = 23; 
const int relayRight2 = 25;
const int relayRight3= 27; 
const int relayRight4 = 29;

const int startStatePin = 31; 


void(*resetFunc)(void) = 0;

//pin 30 sensorLeft data
#define CONFIG_PC30_AS_DIG_INPUT() (DDRC &= 0x80)
#define READ_PC30() ((PINC & 0x80) >> 7)

//pin 38 sensorRight data
#define CONFIG_PC38_AS_DIG_INPUT() (DDRD &= 0x80)
#define READ_PC38() ((PIND & 0x80) >> 7)


int posLeft = 0;// variable to store the servo position
int posRight = 0;
unsigned long stepsLeft = 0; 
unsigned long stepsRight = 0;
unsigned long leftStep = 0;
unsigned long rightStep = 0;
unsigned long centerStep = 0;  
int printOnce = 0;
int printThree = 0; 
int startState;
int prevState = HIGH;


FeedStepper MotorLeft(stepPinLeft);
FeedStepper MotorRight(stepPinRight);
FoldStepper Left(stepPinLeft, 2050, 0);
FoldStepper Right(stepPinRight, 2050, 0);
FoldStepper Center(stepPinCenter, 2050, 0);
Servo leftServo;  
Servo rightServo;



void setup(){
  leftServo.attach(5);  
  rightServo.attach(3);
  Serial.begin(9600);
  pinMode(relayLeft1, OUTPUT);
  pinMode(relayLeft2, OUTPUT);
  pinMode(relayLeft3, OUTPUT);
  pinMode(relayLeft4, OUTPUT);
  pinMode(relayRight1, OUTPUT);
  pinMode(relayRight2, OUTPUT);
  pinMode(relayRight3, OUTPUT);
  pinMode(relayRight4, OUTPUT);
  pinMode(stepPinLeft,OUTPUT); 
  pinMode(dirPinLeft,OUTPUT);
  pinMode(enPinLeft,OUTPUT);
  pinMode(stepPinRight,OUTPUT); 
  pinMode(dirPinRight,OUTPUT);
  pinMode(enPinRight,OUTPUT);
  pinMode(stepPinCenter,OUTPUT); 
  pinMode(dirPinCenter,OUTPUT);
  pinMode(enPinCenter,OUTPUT);
  pinMode(steamPin, OUTPUT);

digitalWrite(steamPin, HIGH);
Serial.println("setup");
digitalWrite(enPinLeft, LOW);
digitalWrite(enPinRight, LOW);
digitalWrite(enPinCenter, LOW);


}
void loop(){
//Serial.print("RIGHT SIDE: ");   //checks to verify that
//Serial.println(READ_PC38());    //the ultrasonice sensors
//delay(50);                      //are working correctly 
//Serial.print("LEFT SIDE: ");    //at startup. 
//Serial.println(READ_PC30());

  
 //-------------------------start of Folding cycle--------------------- 

  
  startState = digitalRead(startStatePin);
  if(startState == HIGH && prevState == LOW)
  {
      prevState == HIGH;
      relayFeed();
      if (stepsLeft == 0 && stepsRight == 0)
      {
          GrabShirt();
      }
      digitalWrite(steamPin, HIGH);
      delay(2000);
      closeClaw();
      delay(1500);
      digitalWrite(steamPin,LOW);
      delay(3000);
      
      TripSensor();
      digitalWrite(steamPin, HIGH);
      ChooseCase();
      relayFold();
      FoldCycle();

    resetFunc();
  }
  
  if((startState == HIGH || startState == LOW) && prevState == HIGH)
  {
      prevState = LOW;
      if(printOnce == 0 && startState == LOW){
        Serial.println("Swith -> High to start Fold");
        printOnce = 1; 
      }
      while(startState == HIGH){
        if(printThree== 0)
        {
        Serial.println("Swith -> LOW then Switch -> HIGH to start Fold");
        printThree = 1;  
        }//end if      
        startState = digitalRead(startStatePin);
        }//end while     
       
      delay(50);
   }// end if

   
}//end loop()



void relayFold(){
  digitalWrite(relayLeft1, HIGH);
  digitalWrite(relayLeft2, HIGH);
  digitalWrite(relayLeft3, HIGH);
  digitalWrite(relayLeft4, HIGH);
  digitalWrite(relayRight1, HIGH);
  digitalWrite(relayRight2, HIGH);
  digitalWrite(relayRight3, HIGH);
  digitalWrite(relayRight4, HIGH);
  return;
}
void relayFeed(){
  digitalWrite(relayLeft1, LOW);
  digitalWrite(relayLeft2, LOW);
  digitalWrite(relayLeft3, LOW);
  digitalWrite(relayLeft4, LOW);
  digitalWrite(relayRight1, LOW);
  digitalWrite(relayRight2, LOW);
  digitalWrite(relayRight3, LOW);
  digitalWrite(relayRight4, LOW);
  return;
}

void GrabShirt()
{
  digitalWrite(enPinRight, HIGH);
  digitalWrite(enPinLeft, HIGH);

  
  digitalWrite(dirPinLeft, LOW);
  digitalWrite(dirPinRight, HIGH);

  
  for(int x = 0; x != 10000; x++) {
    digitalWrite(stepPinLeft,LOW);
    digitalWrite(stepPinRight,LOW);
    delayMicroseconds(15); 
    digitalWrite(stepPinLeft,HIGH);
    digitalWrite(stepPinRight, HIGH);
    delayMicroseconds(15); 
    stepsLeft++;
    stepsRight++;
    }
  for(long x = 0; x != 117000; x++) {
    digitalWrite(stepPinLeft,LOW);
    digitalWrite(stepPinRight,LOW);
    delayMicroseconds(3); 
    digitalWrite(stepPinLeft,HIGH);
    digitalWrite(stepPinRight, HIGH);
    delayMicroseconds(3); 
    stepsLeft++;
    stepsRight++;
    }
}

void TripSensor()
{
    digitalWrite(dirPinLeft, HIGH);
    digitalWrite(dirPinRight, LOW);
   
    while((READ_PC30() != 1 && READ_PC38() != 1) && (stepsLeft != 0 && stepsRight != 0))
    {
      stepsLeft = MotorLeft.FeedLeft(stepsLeft, 10);      //steps count down in feed cycle
      stepsRight = MotorRight.FeedRight(stepsRight, 10);
    }
       
    if(stepsLeft == 0 && stepsRight == 0)
    {
      resetFunc();
    } 


}

void ChooseCase()
{
   boolean rightSide = false; 
   boolean leftSide = false ;
   READ_PC30();
   READ_PC38();
   digitalWrite(dirPinLeft, HIGH);
   digitalWrite(dirPinRight, LOW);

    if (stepsLeft == 0 && stepsRight != 0)
    {
      while(stepsRight != 0)
      {
      stepsRight = MotorRight.FeedRight(stepsRight, 50);
      }
      resetFunc();      
    }

    if (stepsRight == 0 && stepsLeft != 0)
    {
      while(stepsLeft != 0)
      {
        stepsLeft = MotorLeft.FeedLeft(stepsLeft, 50);
      }
      resetFunc();
    }
   
                          //PC38 == RIGHT
    if(READ_PC30() == 1)
    {
      leftSide = true; 
    }
    if(READ_PC38() == 1)
    {
      rightSide = true;
    }
         
    if (rightSide == false && leftSide == true)
    {
         Serial.println("LEFT = 1, RIGHT = 0");
         while(READ_PC38() == 0 && stepsRight != 0)
         {
            stepsRight = MotorRight.FeedRight(stepsRight, 100);
            READ_PC30();
            READ_PC38();
         }
         leftSide = true; 
         ChooseCase();
    }
    if (rightSide == true && leftSide == false)
    {
         Serial.println("RIGHT = 1, LEFT = 0");
         while(READ_PC30() == 0 && stepsLeft !=0)
         {
           stepsLeft = MotorLeft.FeedLeft(stepsLeft, 100);      //steps count down in feed cycle
           READ_PC30();
           READ_PC38();
         }
         rightSide = true; 
         ChooseCase();

    }

     if (leftSide == true && rightSide == true)
    {
          openClaw();
          Serial.println("RIGHT = 1, LEFT = 1");
          while(stepsLeft != 0 && stepsRight != 0)
          {
                  stepsLeft = MotorLeft.FeedLeft(stepsLeft, 15);      //steps count down in feed cycle
                  stepsRight = MotorRight.FeedRight(stepsRight, 15);
          }
          while(stepsLeft == 0 && stepsRight != 0)
          {
                   stepsRight = MotorRight.FeedRight(stepsRight, 15);
          }
          while(stepsLeft != 0 && stepsRight == 0)
          {
                  stepsLeft = MotorLeft.FeedLeft(stepsLeft, 15);      //steps count down in feed cycle                  
          }
         READ_PC30();
         READ_PC38();
         return; 
    }
}

void FoldCycle()
{
  digitalWrite(enPinLeft, HIGH);
  digitalWrite(enPinRight, LOW);
  digitalWrite(dirPinLeft, HIGH);

  if(leftStep != 2050) 
  { 
    while(leftStep < 500)
    {
      leftStep = Left.FoldLeft(leftStep, 750);
    }    
    while(leftStep >= 500 && leftStep < 1350)
    {
      leftStep = Left.FoldLeft(leftStep, 350);
    }
    while(leftStep >= 1350 && leftStep < 2050)
    {
      leftStep = Left.FoldLeft(leftStep, 200); 
    }
  }//end while
  
  digitalWrite(enPinRight, HIGH);
  digitalWrite(dirPinLeft, LOW);
  digitalWrite(dirPinRight, HIGH);
  leftStep = leftStep - 10; 
  if(rightStep != 2050)
  {
    while(rightStep < 500)
    {
      rightStep = Right.FoldRight(rightStep, 750);
      leftStep =  Left.UnfoldLeft(leftStep, 500);
    }    
    while(rightStep >= 500 && rightStep < 1350)
    {
      rightStep = Right.FoldRight(rightStep, 350);
      leftStep =  Left.UnfoldLeft(leftStep, 250);
    }
    while(rightStep >= 1350 && rightStep < 2050)
    {
      rightStep = Right.FoldRight(rightStep, 150); 
      leftStep =  Left.UnfoldLeft(leftStep, 400);
      }
    }
    digitalWrite(enPinCenter, HIGH);
    digitalWrite(enPinLeft, LOW);
    digitalWrite(dirPinRight, LOW);
    digitalWrite(dirPinCenter, LOW);
  if(centerStep != 2050)
  {
  
    while(rightStep > 1250 && rightStep <= 2050)
    {
      rightStep =  Right.UnfoldRight(rightStep, 500);     
    }    
    while(rightStep > 800 && rightStep <= 1250)
    {
      rightStep =  Right.UnfoldRight(rightStep, 250);
      centerStep = Center.FoldCenter(centerStep, 600);
    }
    while(rightStep != 0)
    {       
      rightStep =  Right.UnfoldRight(rightStep, 400);
      centerStep = Center.FoldCenter(centerStep, 300);       
    }
    while(rightStep == 0 && centerStep != 2050)
    {
      centerStep = Center.FoldCenter(centerStep, 200);
    }     
      
  }
  


  digitalWrite(enPinRight, LOW);
  digitalWrite(dirPinCenter, HIGH);

  Serial.print("center STEP: ");
  Serial.println(centerStep);

  centerStep = centerStep + 200;
  if(centerStep != 0)
  {
    while(centerStep > 1350)
    {
      centerStep = Center.UnfoldCenter(centerStep, 500);
    }
    while(centerStep > 500 && centerStep <= 1350)
    {
      centerStep = Center.UnfoldCenter(centerStep, 250);
    }
    while(centerStep != 0)
    {
      centerStep = Center.UnfoldCenter(centerStep, 400);
    }    


  }
    
 
}

void closeClaw() {
  Serial.println("closeClaw");  
        while(posLeft != 180 && posRight != 180)
        {
          leftServo.write(posLeft); 
          rightServo.write(posRight);
          delay(10); 
          posLeft +=1;
          posRight += 1; 
        } 
}

void openClaw() {
  Serial.println("openClaw");    
        while(posLeft != 0 && posRight != 0)
        {
          leftServo.write(posLeft); 
          rightServo.write(posRight);
          delay(10); 
          posLeft -=1;
          posRight -= 1; 
        }
}
