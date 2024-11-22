//todo
// refactor code

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Rotary Encoder Pins
const int pinCLK = 12;
const int pinDT = 14;
const int pinSW = 27;

// Second rotaryEncoerder pins
const int pinCLK2 = 13;
const int pinDT2 = 15;
const int pinSW2 = 26;

// Encoder variables
int counter = 4; 
int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;
int displayOffset = 0;
const int maxLines = 8;

// second encoder variables
int counter2 = 4;
int currentStateCLK2;
int lastStateCLK2;
unsigned long lastButtonPress2 = 0;
int displayOffset2 = 0;
const int maxLines2 = 8;

// refresh rate stuff
unsigned long previousMillis = 0;
const long interval = 40;  // Update screen every 50ms

// Game variables
int ballRadius = 2;
int score = 0;
int speedX = 3;
int circleX = 63;
int speedY = 2;
int circleY = 31;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Function declarations (prototypes)
void updateDisplay();
void checkEncoder();
void checkButton();
void checkEncoder2();
void checkButton2();

void setup() {
  Serial.begin(115200);
  
  // Setup encoder pins
  pinMode(pinCLK, INPUT_PULLUP);
  pinMode(pinDT, INPUT_PULLUP);
  pinMode(pinSW, INPUT_PULLUP);
  lastStateCLK = digitalRead(pinCLK);

  // Setup second encoder pins
  pinMode(pinCLK2, INPUT_PULLUP);
  pinMode(pinDT2, INPUT_PULLUP);
  pinMode(pinSW2, INPUT_PULLUP);
  lastStateCLK2 = digitalRead(pinCLK2);

  Wire.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  updateDisplay();
}

void loop(){
    unsigned long currentMillis = millis();
    
    // Check encoders and buttons
    checkEncoder();
    checkEncoder2();
    checkButton();
    checkButton2();
    
    // Update display every interval milliseconds
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        
        // Update circle position
        circleX -= speedX;
        circleY += speedY;
        
        if ((circleX < 8 && (circleY >= (16+counter*4-ballRadius) && circleY <= (16+counter*4+16+ballRadius))) || (circleX > 117 && (circleY >= (16+counter2*4-ballRadius) && circleY <= (16+counter2*4+16+ballRadius)))) {
            speedX *= -1;
            // Calculate where on the paddle the ball hit (0 to 16)
            float hitPosition;
            if (circleX < 8) {
                hitPosition = circleY - (16+counter*4);
            } else {
                hitPosition = circleY - (16+counter2*4);
            }
            
            // Convert hit position to a -2 to 2 range
            // Middle (8) = 0 speed
            // Edges (0 or 16) = maximum speed
            speedY = (hitPosition - 8) / 4;
            
            // Ensure minimum speed to prevent horizontal-only movement
            if (abs(speedY) < 0.1) {
                speedY = 0;
            }
        }

        if (circleX < -6) {
            score++;
            circleX = 63;
            circleY = 40;
            speedX *= -1;
        } else if (circleX > 133) {
            score--;
            circleX = 63;
            circleY = 40;
            speedX *= -1;
        }
        
        if (circleY < 20) {
            speedY *= -1;
        } else if (circleY > 59) {
            speedY *= -1;
        }
        

        updateDisplay();
    }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Display score
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("Score: ");display.print(score);

  // Calculate vertical position based on encoder counter
  display.fillRect(2, 16+counter*4, 2, 16, WHITE);

  display.fillRect(124, 16+counter2*4, 2, 16, WHITE);
   
  display.fillCircle(circleX, circleY, ballRadius, WHITE);
  
  display.display();
}

void checkEncoder() {
  currentStateCLK = digitalRead(pinCLK);

  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    if (digitalRead(pinDT) != currentStateCLK) {
      if (counter > 0) counter--;
    } else {
      if (counter < maxLines) counter++;
    }
    updateDisplay();
    Serial.print("Counter: ");
    Serial.println(counter);
  }
  lastStateCLK = currentStateCLK;
}

void checkButton() {
  if (digitalRead(pinSW) == LOW) {
    if (millis() - lastButtonPress > 50) {
      counter = 0;
      updateDisplay();
      Serial.println("Left padel reset to top");
    }
    lastButtonPress = millis();
  }
}

void checkEncoder2() {
  currentStateCLK2 = digitalRead(pinCLK2);

  if (currentStateCLK2 != lastStateCLK2 && currentStateCLK2 == 1) {
    if (digitalRead(pinDT2) != currentStateCLK2) {
      if (counter2 > 0) counter2--;
    } else {
      if (counter2 < maxLines2) counter2++;
    }
    updateDisplay();
    Serial.print("Counter2: ");
    Serial.println(counter2);
  }
  lastStateCLK2 = currentStateCLK2;
}

void checkButton2() {
  if (digitalRead(pinSW2) == LOW) {
    if (millis() - lastButtonPress2 > 50) {
      counter2 = 0;
      updateDisplay();
      Serial.println("Right padel reset to top");
    }
    lastButtonPress2 = millis();
  }
}
