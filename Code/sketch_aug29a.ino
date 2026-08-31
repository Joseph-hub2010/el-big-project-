

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,16,2);
int direction_button=3;
volatile bool isReversed=false;
volatile unsigned long lastTime = 0;
const unsigned long debounce = 50;

bool PrintRequested=false;
int printButton=A2;
float vehicleSpeed=0;
int echo1=4;
int echo2=5;
int trig1=6;
int trig2=7;
int ENA=10;
int ENB=9;
int in1=8;
int in2=11;
int in3=12;
int in4=13;
int steeringpotPin=A0;
int speedPotPin=A1;
int servo=2;
Servo myservo;
void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.clear();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(direction_button,INPUT_PULLUP);
  pinMode(printButton, INPUT_PULLUP);
  pinMode(echo1,INPUT);
  pinMode(echo2,INPUT);
  pinMode(trig1,OUTPUT);
  pinMode(trig2,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  myservo.attach(servo);
  attachInterrupt(digitalPinToInterrupt(direction_button),ISR_changedirection,FALLING);
}
float ultrasonicDist(int trigPin,int echoPin){
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPin,LOW); 
  long duration=pulseIn(echoPin,HIGH);
  float distance=(duration*0.0343/2);
  
return distance;
}
void ISR_changedirection(){
  
   unsigned long currentTime=millis();
  if(currentTime-lastTime>debounce){
    isReversed = !isReversed;
    lastTime=currentTime;
  }
}


void moveForward(int motorSpeed){
digitalWrite(in1,HIGH);
digitalWrite(in2,LOW);
digitalWrite(in3,HIGH);
digitalWrite(in4,LOW);
for(int i=0;i<=motorSpeed;i+=5){
  analogWrite(ENA,i);
  analogWrite(ENB,i);
  delay(10);
}
}
void moveBackward(int motorSpeed){
digitalWrite(in1,LOW);
digitalWrite(in2,HIGH);
digitalWrite(in3,LOW);
digitalWrite(in4,HIGH);
for(int i=0;i<=motorSpeed;i+=5){
  analogWrite(ENA,i);
  analogWrite(ENB,i);
  delay(10);
}
}




void calcSpeed(){
  int speedpotVal=analogRead(speedPotPin);
  int rpm=map(speedpotVal ,0,1023,0,600);
   vehicleSpeed=2*3.14159*0.0325*(rpm/60);
}

void stop(int motorSpeed){
for(int i=motorSpeed;i>=0;i--){
  analogWrite(ENA,i);
  analogWrite(ENB,i);
  delay(10);
}
digitalWrite(in1,HIGH);
digitalWrite(in2,HIGH);
digitalWrite(in3,HIGH);
digitalWrite(in4,HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(printButton) == LOW){
    delay(50);
    PrintRequested = true;
  }
  int speedpotReading =analogRead(speedPotPin);
  int targetSpeed=map(speedpotReading ,0,1023,0,255);
  
  
  if (isReversed){
    moveBackward(targetSpeed);
  }
  else{
    moveForward(targetSpeed);
  }
    
  //------------------------------------
  int steerpotVal= analogRead(steeringpotPin);
  int servoAngle=map(steerpotVal,0,1023,0,180);
 myservo.write(servoAngle) ;
  
float distanceF=ultrasonicDist(trig1,echo1);
float distanceB=ultrasonicDist(trig2,echo2);
   calcSpeed();
 //------------------------------------
  if (distanceF<=30){
  stop(targetSpeed);
  delay(300);
  moveBackward(targetSpeed);
  delay(1000);
  moveForward(targetSpeed);
 }

else if (distanceB<=30){
  stop(targetSpeed);
  delay(300);
  moveForward(targetSpeed);
  delay(1000);
  moveBackward(targetSpeed);
 }
  
 
//------------------------------------
if(PrintRequested){
  lcd.clear();
lcd.setCursor(0,0);
  lcd.print("F");
lcd.print(distanceF);
lcd.print("");
  lcd.setCursor(8,0);
  lcd.print("B");
lcd.print(distanceB);
lcd.setCursor(0,1);
lcd.print(vehicleSpeed);
  lcd.print("(m/s)");
lcd.setCursor(9,1);
   if(isReversed){
        lcd.print("backward");
   }else{
    lcd.print("forward");
   }
delay(1000);
PrintRequested=false;

}
}