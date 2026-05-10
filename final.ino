#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

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
const int TAP_GAP_TIME = 400;   // Time to wait for next tap (ms)

// --- State Variables ---
unsigned long eventTimer;
unsigned long touchStartTime = 0;
unsigned long lastTouchReleaseTime = 0;
int tapCount = 0;
bool isTouching = false;
bool event1Played = false, event2Played = false,event3Played = false ,event4Played = false,event5Played = false,event6Played = false,event7Played = false,event8Played = false,event9Played=false,event10Played=false,event11Played=false,event12Played=false,event13Played=false,event14Played=false,event15Played=false,event16Played=false,event17Played=false,event18Played=false,event19Played=false,event20Played=false,event21Played=false;

bool aj1=false,aj2=false,aj3=false,aj4=false,aj5=false;


bool longPressTriggered = false;

bool flag=true;
bool sleep=false;

bool hasPlayedHeart = false, powerStatus=false,boot=false;

int mood=0;

int tap=0;



void setup() {
  Serial.begin(9600);

  pinMode(7, OUTPUT);
digitalWrite(7, LOW);
  
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
    for(;;); 
  }
  
  Wire.setClock(400000); // High speed I2C
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.close();
  
  eventTimer = millis();
  
}
// 10 seconds to shut down



void loop() {
  int sensorValue = analogRead(TOUCH_PIN);
  unsigned long currentMillis = millis();
  bool isPressed = (sensorValue > THRESHOLD);

  static unsigned long powerOffTime = 0;

if(!boot){  //initial boot only
display.clearDisplay();
  display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(50, 10);
display.println(":)");
display.setCursor(10, 30);
display.println("kuttappan standby");
display.setCursor(20, 50);
display.println("firmware 2.16");
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

  // 1. UPDATE TOUCH STATE
  bool wasTouching = isTouching;

  // --- Improved touch handling with debounce ---
  static unsigned long releaseTime = 0;

  if (isPressed) {

    releaseTime = 0;

    // First touch detected
    if (!isTouching) {
      isTouching = true;
      touchStartTime = currentMillis;

      longPressTriggered = false;

      roboEyes.open();
    }

  } else {

    // Start release timer
    if (releaseTime == 0) {
      releaseTime = currentMillis;
    }

    // Require stable release for 50ms
    if (currentMillis - releaseTime > 50) {
      isTouching = false;
    }
  }

  // 2. NORMAL OPERATION
  roboEyes.update();

  // 3. HANDLE LONG PRESS
  if (isTouching && !longPressTriggered) {

    if (currentMillis - touchStartTime >= LONG_TOUCH_TIME) {

      triggerLongTouchTask();

      longPressTriggered = true;
      tapCount = 0;
    }
  }

  // 4. RELEASE LOGIC (DetectING Taps)
  if (wasTouching && !isTouching) {

    unsigned long duration = currentMillis - touchStartTime;

    // Only count as tap if not long press
    if (!longPressTriggered &&
        duration < LONG_TOUCH_TIME) {

      tapCount++;
      lastTouchReleaseTime = currentMillis;
    }
  }

  // 5. MULTI-TAP EXECUTION
  if (tapCount > 0 &&
      (currentMillis - lastTouchReleaseTime > TAP_GAP_TIME)) {

    switch (tapCount) {

      case 1:
        triggerShortTouchTask();
        break;

      case 2:
      //freeze
      
        break;

      case 3:
      //SLEEP
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

  // 6. IDLE ANIMATIONS
  if (!isTouching && tapCount == 0) {
    handleIdleAnimations();
  }
}

}


// ==========================================
//          CORE ACTION FUNCTIONS
// ==========================================


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

  roboEyes.setMood(HAPPY);

  sendDataToPC("Long_TAP", 2);

 if (!hasPlayedHeart) {
    heart(); 
    hasPlayedHeart = true; // This prevents it from running again
  }
  roboEyes.setMood(TIRED);
}



//dont edit this works good
void triggerTooManyTapsTask() {

 
  roboEyes.setWidth(36, 36);
  roboEyes.setHeight(36, 36);
  roboEyes.setBorderradius(8, 8);
  roboEyes.setMood(ANGRY);

  if (tap > 3) {
    // --- SCREAM LOGIC ---
    // Instead of one long playScream(), we break it into bursts
    playScream();
    roboEyes.setVFlicker(OFF, 0);
    tap = 0; // Reset tap counter
  } 
  else {
    // --- CONFUSED/ANGRY LOGIC ---
    roboEyes.anim_confused();
    
    if (flag) {
      // Start a low growl sound that doesn't block
      tone(6, 150, 100); 
      
      roboEyes.setVFlicker(ON, 10);
      // Update eyes while the tone plays in the background
      for(int i = 0; i < 8; i++) {
        roboEyes.update();
        delay(10);
      }
      roboEyes.setVFlicker(OFF, 0);
    } 
    else {
      roboEyes.setHFlicker(ON, 10);
      // Short high-pitched "tick" sound
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

// ==========================================
//          EXPANSION AREAS
// ==========================================

void handleIdleAnimations() {
  unsigned long currentMillis = millis();

  int choice = random(0, 2);

    
  // Event: Wake up after 2 seconds
  if(currentMillis >= eventTimer + 2000 && !event1Played){
    event1Played = true;
    roboEyes.open(); 
    roboEyes.blink(0,1) ;
    
  }
  
  // Event: Look Tired after 5 seconds
  if(currentMillis >= eventTimer + 5000 && !event2Played){
    event2Played = true;
    roboEyes.setMood(TIRED);
  }

  if(currentMillis >= eventTimer + 8000 && !event3Played){
    event3Played = true;
    roboEyes.setCuriosity(ON);
    roboEyes.setMood(DEFAULT);    
    roboEyes.setPosition(W); 
    
     

  }

  if(currentMillis >= eventTimer + 11000 && !event4Played){
    event4Played = true;   
    roboEyes.setPosition(SW);  
     roboEyes.setMood(DEFAULT);

  }

  if(currentMillis >= eventTimer + 14000 && !event5Played){
    event5Played = true;  
    roboEyes.setPosition(E); 
    roboEyes.close(); 
     roboEyes.setMood(DEFAULT);

  }

  if(currentMillis >= eventTimer + 17000 && !event6Played){
    event6Played = true;
    roboEyes.open(); 
    roboEyes.setPosition(SE); 
    roboEyes.setPosition(DEFAULT); 
     roboEyes.setMood(DEFAULT); 
     roboEyes.blink(0,1) ;

  }

   if(currentMillis >= eventTimer + 19000 && !event7Played){
    event7Played = true;
     roboEyes.setPosition(W);
    roboEyes.update();   
    roboEyes.close();
     roboEyes.setMood(DEFAULT);

  }
  if(currentMillis >= eventTimer + 22000 && !event8Played){
    event8Played = true;
    roboEyes.setPosition(E);
    roboEyes.open();
     roboEyes.setMood(DEFAULT);

  }




  if(currentMillis >= eventTimer + 25000 && !event9Played){
    event9Played = true; 
    roboEyes.setHeight(66,66);
    roboEyes.setWidth(66,66);
    roboEyes.update();
   roboEyes.setMood(DEFAULT);
    roboEyes.setPosition(DEFAULT);  

  }


  if(currentMillis >= eventTimer + 27000 && !event10Played){
    event10Played = true;   
    roboEyes.setPosition(SW);  
    roboEyes.setHeight(36,36);
    roboEyes.setWidth(36,36);
    roboEyes.update();
     roboEyes.setMood(DEFAULT);

  }

  if(currentMillis >= eventTimer + 30000 && !event11Played){
    event11Played = true;  
    roboEyes.setPosition(E); 
    roboEyes.setBorderradius(8,20);
    roboEyes.open(1, 0);
     
      roboEyes.setMood(DEFAULT);
      roboEyes.setSweat(ON);
     

  }

 if(currentMillis >= eventTimer + 32000 && !event12Played){
    event12Played = true;
    roboEyes.open(); 
    roboEyes.setPosition(S); 
     roboEyes.setMood(DEFAULT);
    roboEyes.setPosition(DEFAULT);  
  } // <--- Added missing brace here

   if(currentMillis >= eventTimer + 32500 && !event13Played){
    event13Played = true;
    roboEyes.setHFlicker(ON,10);  
     roboEyes.setMood(DEFAULT);
  } 
  if(currentMillis >= eventTimer + 33000 && !event14Played){
    event14Played = true;
    roboEyes.setHFlicker(OFF,5);  
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
      roboEyes.setMood(DEFAULT);
     roboEyes.setHeight(36, 36);
  } 

  if(currentMillis >= eventTimer + 36000 && !event15Played){
    event15Played = true;
     roboEyes.setMood(TIRED); 
     sleep=true;
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(32, 32);
  } 

  if(currentMillis >= eventTimer + 37000 && !event16Played){
    event16Played = true;
      
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(28, 28);
  } 

  if(currentMillis >= eventTimer + 39000 && !event17Played){
    event17Played = true;
      
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(25, 25);
  } 


if(currentMillis >= eventTimer + 41000 && !event18Played){
    event18Played = true;
      
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(20, 20);
  } 

if(currentMillis >= eventTimer + 43000 && !event19Played){
    event19Played = true;
      
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(17, 17);
  } 


  if(currentMillis >= eventTimer + 44500 && !event20Played){
    event20Played = true;
     
     roboEyes.setVFlicker(ON,5);
     roboEyes.setMood(DEFAULT);
     sleep=false;
     roboEyes.setBorderradius(8,8);
     roboEyes.setWidth(36, 36);
     roboEyes.setHeight(15, 15);

     roboEyes.setSweat(OFF);
  } 


  // Loop Reset: Reset everything after 35 seconds
  if(currentMillis >= eventTimer + 45000&& !event21Played){
    event21Played = true;
    roboEyes.setVFlicker(OFF,2);
    roboEyes.setHeight(36, 36);
    
    roboEyes.setCuriosity(ON);
   roboEyes.setAutoblinker(ON, 3, 2); // Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
  roboEyes.setIdleMode(ON, 5, 2);
                                                    
    
  }
  



  

    if(currentMillis >= eventTimer + 100000){
    eventTimer = currentMillis; // Reset the baseline time
    roboEyes.setCuriosity(OFF);
     roboEyes.setAutoblinker(OFF, 3, 2);
     roboEyes.setIdleMode(OFF, 5, 2);
    // Reset ALL flags to false so the IF statements can trigger again
    event1Played = false;
    event2Played = false;
    event3Played = false;
    event4Played = false;
    event5Played = false;
    event6Played = false;
    event7Played = false;
    event8Played = false; // Make sure event 8 is here!
    event9Played = false;
    event10Played = false;
    event11Played = false;
    event12Played = false;
    event13Played = false; // Add this
    event14Played = false;
    event15Played=false;
    event16Played=false;
    event17Played=false;
    event18Played=false;
    event19Played=false;
    event20Played=false;
    event21Played=false;

    hasPlayedHeart = false;
    tap=0;

    aj1=false;
    aj2=false;

  }

 
}











// --- HELPER: Draw a heart with a specific size ---
void drawHeart(int x, int y, int size) {
  if (size <= 0) return; 
  int r = size;
  float cr = (size * 0.55);
  int off = (size * 0.5);
  
  display.fillTriangle(x - r, y, x, y + r, x + r, y, WHITE);
  display.fillCircle(x - off, y, cr, WHITE);
  display.fillCircle(x + off, y, cr, WHITE);
}

// --- HELPER: Redraw the whole face to prevent flickering ---
void renderFace(int heartSize, bool isBlink) {
  display.clearDisplay();
  
  int leftX = 40;
  int rightX = 88;
  int eyeY = 22;

  if (isBlink) {
    // Smooth blink lines (slightly rounded rectangles)
    display.fillRoundRect(leftX - 8, eyeY + 5, 16, 4, 2, WHITE);
    display.fillRoundRect(rightX - 8, eyeY + 5, 16, 4, 2, WHITE);
    // Neutral small mouth
    display.fillRoundRect(54, 54, 22, 3, 1, WHITE);
  } else {
    // Draw the growing/beating hearts
    drawHeart(leftX, eyeY, heartSize);
    drawHeart(rightX, eyeY, heartSize);
    
    // Smooth curved smile
    display.fillCircle(64, 50, 12, WHITE);
    display.fillRect(0, 38, 128, 12, BLACK); 
  }
  
  display.display();
}

void heart() {
  // 1. SMOOTH GROW (Transition from roboEyes HAPPY to Heart Eyes)
  // Hearts "pop" out from size 0 to 16
  for (int s = 0; s <= 16; s += 2) {
    renderFace(s, false);
    delay(15); 
  }

  // 2. THE HEARTBEAT EFFECT
  // Loops a "pulse" 3 times to make the robot feel emotional
  for (int pulse = 0; pulse < 3; pulse++) {
    // Scale up slightly
    for (int s = 16; s <= 19; s++) {
      renderFace(s, false);
      delay(20);
    }
    // Scale back down
    for (int s = 19; s >= 16; s--) {
      renderFace(s, false);
      delay(20);
    }
    delay(100); // Pause at the bottom of the beat
  }

  // 3. SMOOTH BLINK
  renderFace(0, true);


  // 4. RETURN TO NORMAL HEARTS
  for (int s = 0; s <= 16; s += 2) {
    renderFace(s, false);
    delay(10);
  }
  
// Hold the final look
}











// Function to send robot data to Python via Serial
void sendDataToPC(String eventName, int value) {
  // Format: EventName,Value
  // Example: "HEART_ANIM,1"
  Serial.print(eventName);
  Serial.print(",");
  Serial.println(value); // println adds the '\n' Python needs to identify the end of a line
}










//audio

void playSound() {
  int soundType = random(0, 3); // Pick a random sound style

  if (soundType == 0) {
    // --- Style 0: The "Happy Chirp" (Quick rising scale) ---
    for (int i = 1000; i < 2000; i += 150) {
      tone(BUZZER_PIN, i, 20);
      delay(25);
    }
  } 
  else if (soundType == 1) {
    // --- Style 1: The "Double Pip" (Classic robot blink) ---
    tone(BUZZER_PIN, 1500, 50);
    delay(60);
    tone(BUZZER_PIN, 2000, 50);
  } 
  else {
    // --- Style 2: The "Sparkle" (High pitched random pips) ---
    for (int i = 0; i < 3; i++) {
      int note = random(2000, 3000);
      tone(BUZZER_PIN, note, 30);
      delay(50);
    }
  }
  
  noTone(BUZZER_PIN); // Ensure the buzzer turns off
}


void playR2D2() {
  int type = random(0, 3);

  if (type == 0) {
    // --- The "Excited Whistle" (Fast slide up) ---
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
    // --- The "Processing Chirp" (Rapid random pips) ---
    for (int i = 0; i < 5; i++) {
      int chirp = random(1800, 3500);
      tone(BUZZER_PIN, chirp, 15);
      delay(30);
    }
  } 
  else {
    // --- The "Question Whistle" (Low to High slide) ---
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
  // A slow, mournful slide downwards
  for (int freq = 1500; freq > 800; freq -= 5) {
    tone(BUZZER_PIN, freq);
    delay(5);
  }
  noTone(BUZZER_PIN);
}





void playAngry() {
  // A low-pitched "grumble" that drops off
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, random(600, 900), 60);
    delay(70);
  }
  // A final low tone
  tone(BUZZER_PIN, 400, 200);
  delay(200);
  noTone(BUZZER_PIN);
}

void playScream() {
  // High-frequency "stutter" that sounds like a glitchy panic
  for (int i = 0; i < 15; i++) {
    tone(BUZZER_PIN, random(2000, 4000)); // Rapid jumps between high pitches
    delay(20); // Very fast transitions
  }
  // A sharp drop to a low, flat tone
  tone(BUZZER_PIN, 500, 300);
  delay(300);
  noTone(BUZZER_PIN);
}
