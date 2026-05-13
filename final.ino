#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>
#include<EEPROM.h>


//this takes 99% of program memory 

// --- Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR    0x3C
#define OLED_RST     -1

#define BUZZER_PIN 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

// --- Hardware Pins & Thresholds ---
const int TOUCH_PIN = A6;
const int THRESHOLD = 100;      // decrease for more sensitivity
const int LONG_TOUCH_TIME = 400; // ms for a long press
const int TAP_GAP_TIME = 400;   

// --- State Variables ---
unsigned long eventTimer;
unsigned long nextResetDelay = 100000;
int eeprom_ran = 0;
bool sound=0;
unsigned long touchStartTime = 0;
unsigned long lastTouchReleaseTime = 0;
int tapCount = 0;
bool isTouching = false;
bool eventPlayed[21] = {false};

bool aj[5] = {false};


bool longPressTriggered = false;

bool flag=true;
bool sleep=false;

bool hasPlayed = false, powerStatus=false,boot=false;

int mood=0;

int tap=0;



void setup() {
  Serial.begin(9600);

  pinMode(7, OUTPUT);
digitalWrite(7, LOW);
  EEPROM.get(0, eeprom_ran);
  eeprom_ran++;
  EEPROM.put(0, eeprom_ran);

   EEPROM.get(1, sound);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
    for(;;); 
  }
  
  Wire.setClock(400000);
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.close();
  
  eventTimer = millis();
  nextResetDelay = random(100000, 200000);
  
}



void loop() {
  int sensorValue = analogRead(TOUCH_PIN);
  unsigned long currentMillis = millis();
  bool isPressed = (sensorValue > THRESHOLD);

  static unsigned long powerOffTime = 0;
if(!boot){
display.clearDisplay();
  display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(50, 10);
display.println(":)");
display.setCursor(10, 30);
display.println("kuttappan standby");
display.setCursor(20, 50);
display.println("boot count:");
display.setCursor(88, 50);
display.println(eeprom_ran);
display.display();}

if (!powerStatus) {

    if (millis() - powerOffTime > 500) {

        if (isPressed && !isTouching) {
            powerStatus = true;
            boot=true;
        }
    }
}

  if(isPressed && !powerStatus && !isTouching){
    powerStatus=true;
    }

  if(powerStatus){

  bool wasTouching = isTouching;

  static unsigned long releaseTime = 0;

  if (isPressed) {

    releaseTime = 0;

    if (!isTouching) {
      isTouching = true;
      touchStartTime = currentMillis;

      longPressTriggered = false;

      roboEyes.open();
    }

  } else {

    if (releaseTime == 0) {
      releaseTime = currentMillis;
    }

    if (currentMillis - releaseTime > 50) {
      isTouching = false;
    }
  }

  roboEyes.update();

  if (isTouching && !longPressTriggered) {

    if (currentMillis - touchStartTime >= LONG_TOUCH_TIME) {

      triggerLongTouchTask();

      longPressTriggered = true;
      tapCount = 0;
    }
  }

  if (wasTouching && !isTouching) {

    unsigned long duration = currentMillis - touchStartTime;

    if (!longPressTriggered &&
        duration < LONG_TOUCH_TIME) {

      tapCount++;
      lastTouchReleaseTime = currentMillis;
    }
  }

  if (tapCount > 0 &&
      (currentMillis - lastTouchReleaseTime > TAP_GAP_TIME)) {

    switch (tapCount) {

      case 1:
        triggerShortTouchTask();
        break;

      case 2:
      
        break;

      case 3:
      playSad();
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(20, 30);

        display.println("kuttappan sleeping");
        display.display();
        delay(1000);
        display.clearDisplay();
        display.display();
        powerStatus = false;
         powerOffTime = millis();
        break;

      default:
        triggerTooManyTapsTask();
        break;
    }

    tapCount = 0;
  }

  if (!isTouching && tapCount == 0) {

    
    
    handleIdleAnimations();
  }
}

}


void triggerShortTouchTask() {

  sendDataToPC("Sinle_TAP", 1);
  playR2D2();
  
    roboEyes.setHeight(36,36);
    roboEyes.setWidth(36,36);
    roboEyes.setBorderradius(8,8);
    if(sleep)
    {
     roboEyes.setMood(ANGRY);
      sleep=false;  
       
    }
    else if(mood==1)
    {
      roboEyes.setMood(DEFAULT);
      roboEyes.anim_laugh();
      mood=0;

    }
  else{
  roboEyes.setMood(HAPPY);
  roboEyes.blink();
  mood=1;
  }
  }

  
  

void triggerLongTouchTask() {

 
  sound=!sound;
  EEPROM.put(1, sound);

  display.clearDisplay();
  display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(20, 50);
display.println("sound mode:");
display.setCursor(88, 50);
sound==1 ? display.println("ON") : display.println("OFF");
display.display();
 delay(1000);
  display.clearDisplay();
 roboEyes.setMood(HAPPY);



 if (!hasPlayed) {

    roboEyes.setWidth(46, 46);
    roboEyes.setHeight(50, 50);
    roboEyes.setBorderradius(12, 12);
    roboEyes.setMood(HAPPY);
    roboEyes.setAutoblinker(OFF, 0, 0);
    roboEyes.anim_laugh();
    roboEyes.blink(0, 2);
    for (int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 1800 + (i * 300), 60);
      delay(90);
    }
    noTone(BUZZER_PIN);
    
    hasPlayed = true;
  }
  else {
    
  
  roboEyes.setMood(TIRED);
 }
}

//dont edit this works good
void triggerTooManyTapsTask() {

 
  roboEyes.setWidth(36, 36);
  roboEyes.setHeight(36, 36);
  roboEyes.setBorderradius(8, 8);
  roboEyes.setMood(ANGRY);

  if (tap > 3) {
    playScream();
    roboEyes.setVFlicker(OFF, 0);
    tap = 0;
  } 
  else {
    roboEyes.anim_confused();
    
    if (flag) {
      tone(6, 150, 100); 
      
      roboEyes.setVFlicker(ON, 10);
      for(int i = 0; i < 8; i++) {
        roboEyes.update();
        delay(10);
      }
      roboEyes.setVFlicker(OFF, 0);
    } 
    else {
      roboEyes.setHFlicker(ON, 10);
      tone(6, 2500, 20); 
      
      for(int i = 0; i < 8; i++) {
        roboEyes.update();
        delay(10);
      }
      roboEyes.setHFlicker(OFF, 0);
      tap++;
    }
    flag = !flag;
  }
}

void handleIdleAnimations() {
  unsigned long currentMillis = millis();

  
    
    
  if(currentMillis >= eventTimer + random(2000,5000) && !eventPlayed[0]){
    eventPlayed[0] = true;
    roboEyes.setAutoblinker(ON, 3, 2);
    roboEyes.open(); 
    roboEyes.blink(0,1) ;
    if(sound){playR2D2();}
    
  }
  
  if(currentMillis >= eventTimer + random(5000,8000) && !eventPlayed[1]){
    eventPlayed[1] = true;
    
    roboEyes.setMood(TIRED);
  }

  if(currentMillis >= eventTimer + random(8000,11000) && !eventPlayed[2]){
    eventPlayed[2] = true;
    roboEyes.setCuriosity(ON);
    roboEyes.setMood(DEFAULT);    
    roboEyes.setPosition(W); 
    
     

  }

  if(currentMillis >= eventTimer + 11000 && !eventPlayed[3]){
    eventPlayed[3] = true;   
    roboEyes.setPosition(SW);  
     roboEyes.setMood(DEFAULT);
     if(sound){playR2D2();}

  }

  if(currentMillis >= eventTimer + 14000 && !eventPlayed[4]){
    eventPlayed[4] = true;  
    roboEyes.setPosition(E); 
    roboEyes.close(); 
     roboEyes.setMood(DEFAULT);
     if(sound){playR2D2();}

  }

  if(currentMillis >= eventTimer + random(14000,17000) && !eventPlayed[5]){
    eventPlayed[5] = true;
    roboEyes.open(); 
    
    roboEyes.setPosition(SE); 
    roboEyes.setPosition(DEFAULT); 
     roboEyes.setMood(DEFAULT); 
     roboEyes.blink(0,1) ;

  }

   if(currentMillis >= eventTimer + random(140000,27000) && !eventPlayed[6]){
    eventPlayed[6] = true;
     roboEyes.setPosition(W);
    roboEyes.update();   
    roboEyes.close();
     roboEyes.setMood(DEFAULT);

  }
  if(currentMillis >= eventTimer + random(22000,25000) && !eventPlayed[7]){
    eventPlayed[7] = true;
    roboEyes.setPosition(E);
    roboEyes.open();
     roboEyes.setMood(DEFAULT);

  }




  if(currentMillis >= eventTimer + 25000 && !eventPlayed[8]){
    eventPlayed[8] = true; 
    roboEyes.setHeight(66,66);
    roboEyes.setWidth(66,66);
    roboEyes.update();
   roboEyes.setMood(DEFAULT);
    roboEyes.setPosition(DEFAULT);  

  }


  if(currentMillis >= eventTimer + 27000 && !eventPlayed[9]){
    eventPlayed[9] = true;   
    roboEyes.setPosition(SW);  
    roboEyes.setHeight(36,36);
    roboEyes.setWidth(36,36);
    if(sound){playR2D2();}
    roboEyes.update();
     roboEyes.setMood(DEFAULT);


  }

  if(currentMillis >= eventTimer + 30000 && !eventPlayed[10]){
    eventPlayed[10] = true;  
    roboEyes.setPosition(E); 
    roboEyes.setBorderradius(8,20);
    roboEyes.open(1, 0);
     if(sound){playR2D2();}
      roboEyes.setMood(DEFAULT);
      roboEyes.setSweat(ON);
     

  }

 if(currentMillis >= eventTimer + 32000 && !eventPlayed[11]){
    eventPlayed[11] = true;
    roboEyes.open(); 
    roboEyes.setPosition(S); 
     roboEyes.setMood(DEFAULT);
    roboEyes.setPosition(DEFAULT);  
  } 

   if(currentMillis >= eventTimer + 32500 && !eventPlayed[12]){
    eventPlayed[12] = true;
    roboEyes.setHFlicker(ON,10);  
     roboEyes.setMood(DEFAULT);
  } 
  if(currentMillis >= eventTimer + 33000 && !eventPlayed[13]){
    eventPlayed[13] = true;
    roboEyes.setHFlicker(OFF,5);  
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
      roboEyes.setMood(DEFAULT);
     roboEyes.setHeight(36, 36);
  } 

  if(currentMillis >= eventTimer + 36000 && !eventPlayed[14]){
    eventPlayed[14] = true;
    if(sound){playR2D2();}
     roboEyes.setMood(TIRED); 
     sleep=true;
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(32, 32);
  } 

  if(currentMillis >= eventTimer + 37000 && !eventPlayed[15]){
    eventPlayed[15] = true;
    
      
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(28, 28);
  } 

  if(currentMillis >= eventTimer + 39000 && !eventPlayed[16]){
    eventPlayed[16] = true;
      
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(25, 25);
  } 


if(currentMillis >= eventTimer + 41000 && !eventPlayed[17]){
    eventPlayed[17] = true;
      
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(20, 20);
  } 

if(currentMillis >= eventTimer + 43000 && !eventPlayed[18]){
    eventPlayed[18] = true;
      
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(17, 17);
  } 


  if(currentMillis >= eventTimer + 44500 && !eventPlayed[19]){
    eventPlayed[19] = true;
     
     roboEyes.setVFlicker(ON,5);
     roboEyes.setMood(DEFAULT);
     sleep=false;
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(15, 15);

     roboEyes.setSweat(OFF);
  } 


  if(currentMillis >= eventTimer + 45000&& !eventPlayed[20]){
    eventPlayed[20] = true;
    roboEyes.setVFlicker(OFF,2);
    roboEyes.setHeight(36, 36);
    if(sound){playR2D2();}
    
    roboEyes.setCuriosity(ON);
   roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 5, 2);
                                                    
    
  }
    

    if(currentMillis >= eventTimer + nextResetDelay){
    eventTimer = currentMillis;
    nextResetDelay = random(100000, 200000);
    roboEyes.setCuriosity(OFF);
     roboEyes.setAutoblinker(OFF, 3, 2);
     roboEyes.setIdleMode(OFF, 5, 2);
    for (int i = 0; i < 21; i++) {
      eventPlayed[i] = false;
    }

    hasPlayed = false;
    tap=0;

    aj[0]=false;
    aj[1]=false;

  }

 
}


void sendDataToPC(String eventName, int value) {
  Serial.print(eventName);
  Serial.print(",");
  Serial.println(value);
}



void playSound() {
  int soundType = random(0, 3);

  if (soundType == 0) {
    for (int i = 1000; i < 2000; i += 150) {
      tone(BUZZER_PIN, i, 20);
      delay(25);
    }
  } 
  else if (soundType == 1) {
    tone(BUZZER_PIN, 1500, 50);
    delay(60);
    tone(BUZZER_PIN, 2000, 50);
  } 
  else {
    for (int i = 0; i < 3; i++) {
      int note = random(2000, 3000);
      tone(BUZZER_PIN, note, 30);
      delay(50);
    }
  }
  
  noTone(BUZZER_PIN);
}


void playR2D2() {
  int type = random(0, 3);

  if (type == 0) {
    for (int freq = 1500; freq < 2500; freq += 20) {
      tone(BUZZER_PIN, freq, 10);
      delay(2);
    }
    for (int freq = 2500; freq > 1800; freq -= 40) {
      tone(BUZZER_PIN, freq, 10);
      delay(2);
    }
  } 
  else if (type == 1) {
    for (int i = 0; i < 5; i++) {
      int chirp = random(1800, 3500);
      tone(BUZZER_PIN, chirp, 15);
      delay(30);
    }
  } 
  else {
    for (int freq = 1000; freq < 3000; freq += 10) {
      tone(BUZZER_PIN, freq, 5);
    }
    delay(50);
    tone(BUZZER_PIN, 3500, 40);
  }
  
  noTone(BUZZER_PIN);
}






#define BUZZER_PIN 6

// Make sure D7 is LOW in setup() to act as ground!

void playSad() {
  for (int freq = 1500; freq > 800; freq -= 5) {
    tone(BUZZER_PIN, freq);
    delay(5);
  }
  noTone(BUZZER_PIN);
}


void playAngry() {
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, random(600, 900), 60);
    delay(70);
  }
  tone(BUZZER_PIN, 400, 200);
  delay(200);
  noTone(BUZZER_PIN);
}

void playScream() {
  for (int i = 0; i < 15; i++) {
    tone(BUZZER_PIN, random(2000, 4000));
    delay(20);
  }
  tone(BUZZER_PIN, 500, 300);
  delay(300);
  noTone(BUZZER_PIN);
}
