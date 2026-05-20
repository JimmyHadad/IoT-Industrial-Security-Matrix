#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DFRobotDFPlayerMini.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

// --- REPLACE THESE WITH YOUR OWN CREDENTIALS ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
String scriptUrl = "YOUR_GOOGLE_SCRIPT_WEBAPP_URL_HERE";

// --- ACCESS PASSWORDS ---
String masterPassword = "0000"; // Change to your preferred master password
String userPassword = "1111";   // Change to your preferred user password
String enteredPassword = "";    
int wrongAttempts = 0;          
bool systemLocked = false;      

// --- KEYPAD SETUP ---
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','3','2','A'},
  {'4','6','5','B'},
  {'7','9','8','C'},
  {'*','#','0','D'}
};
byte rowPins[ROWS] = {19, 18, 5, 15}; 
byte colPins[COLS] = {13, 12, 14, 23}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- MP3 PLAYER SETUP ---
#define RX_PIN 16
#define TX_PIN 17
#define BUSY_PIN 4  
HardwareSerial mySerial(2); 
DFRobotDFPlayerMini myDFPlayer;

// --- OLED SETUP ---
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- PINS SETUP ---
#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 27
#define BUZZER_PIN 32
#define RELAY_PIN 33

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN); 
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); 

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  setLED(0, 0, 0); 
  pinMode(BUSY_PIN, INPUT); 

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;); 
  }
  
  mySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  if (!myDFPlayer.begin(mySerial)) {
    updateDisplay("MP3 FAIL", "ERROR");
    while (true);
  }

  myDFPlayer.volume(30); 
  delay(1000); 
  resetSystem(); 
}

void loop() {
  if (systemLocked) {
    setLED(1, 1, 0); 
    delay(500);
    setLED(0, 0, 0); 
    delay(500);
    return; 
  }

  char key = keypad.getKey();
  if (key) {
    beep(50, 1000); 
    if (key == '#') {
      checkPassword();
    } 
    else if (key == '*') {
      enteredPassword = "";
      updateDisplay("ENTER", "PASSWORD");
    } 
    else {
      if(enteredPassword.length() < 8) {
        enteredPassword += key;
      }
      updateDisplay("PASSWORD:", enteredPassword);
    }
  }
}

void checkPassword() {
  if (enteredPassword == masterPassword) {
    wrongAttempts = 0;
    setLED(0, 0, 1); 
    beep(100, 600); delay(50); beep(150, 800); 
    
    updateDisplay("HI", "ADMIN");
    
    sendAlertToDatabase("🔓 VIP ACCESS: Welcome back, Admin.");
    playTrackAndWait(7); 
    
    delay(1500); 
    resetSystem(); 
  } 
  else if (enteredPassword == userPassword) {
    wrongAttempts = 0;
    setLED(0, 1, 0); 
    beep(100, 600); delay(50); beep(150, 800); 
    
    updateDisplay("HI", "USER");
    
    sendAlertToDatabase("🔓 ACCESS GRANTED: User Mode.");
    playTrackAndWait(6); 
    
    delay(1500); 
    resetSystem(); 
  } 
  else {
    wrongAttempts++;
    setLED(1, 0, 0); 
    beep(400, 650); 
    
    if (wrongAttempts == 1) {
      updateDisplay("WRONG", "PASSWORD");
      sendAlertToDatabase("⚠️ SECURITY ALERT: Wrong Code (1/3).");
      playTrackAndWait(2); 
      delay(1000); 
      resetSystem();
    } 
    else if (wrongAttempts == 2) {
      updateDisplay("WARNING", "1 TRY LEFT");
      sendAlertToDatabase("⚠️ CRITICAL WARNING: Wrong Code (2/3).");
      playTrackAndWait(3); 
      delay(1000); 
      resetSystem();
    } 
    else if (wrongAttempts >= 3) {
      systemLocked = true; 
      updateDisplay("YOU WILL", "DIE");
      
      playTrackAndWait(4); 
      sendAlertToDatabase("🚨 PROTOCOL ZERO ACTIVATED: System Purge.");

      for (int i = 10; i > 3; i--) {
        updateDisplay("DESTROY IN", String(i));
        setLED(1, 0, 0);
        tone(BUZZER_PIN, 1500); 
        delay(300);
        setLED(0, 0, 0);
        noTone(BUZZER_PIN);
        delay(700);
      }

      unsigned long startDecimals = millis();
      while (millis() - startDecimals < 3000) {
        unsigned long elapsed = millis() - startDecimals;
        float timeLeft = (3000.0 - elapsed) / 1000.0;
        if (timeLeft < 0) timeLeft = 0.00; 
        updateDisplay("DESTROY IN", String(timeLeft, 2));
        
        if ((elapsed % 250) < 100) {
          setLED(1, 0, 0);
          tone(BUZZER_PIN, 1550);
        } else {
          setLED(0, 0, 0);
          noTone(BUZZER_PIN);
        }
      }
      
      updateDisplay("DESTROY IN", "0.00");
      setLED(1, 0, 0); 
      digitalWrite(RELAY_PIN, LOW); 
      tone(BUZZER_PIN, 2000); 
      delay(1000); 
      setLED(0, 0, 0);
      noTone(BUZZER_PIN);
      
      updateDisplay("SYSTEM", "SECURED");
      playTrackAndWait(5); 
      
      sendAlertToDatabase("🔒 THREAT NEUTRALIZED.");
      
    }
  }
}

void backgroundTask(void *parameter) {
  String *msgPtr = (String*)parameter; 
  String msg = *msgPtr;

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); 
    HTTPClient http;
    msg.replace(" ", "%20"); 
    String fullUrl = scriptUrl + "?message=" + msg;

    http.begin(client, fullUrl);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
    http.GET(); 
    http.end();
  }

  delete msgPtr; 
  vTaskDelete(NULL); 
}

void sendAlertToDatabase(String msg) {
  String *msgToPass = new String(msg); 
  xTaskCreate(backgroundTask, "TelegramTask", 8192, (void*)msgToPass, 1, NULL);
}

void resetSystem() {
  enteredPassword = "";
  setLED(1, 1, 1); 
  updateDisplay("ENTER", "PASSWORD");
  playTrackAndWait(1); 
}

void setLED(int r, int g, int b) {
  digitalWrite(RED_PIN, r ? HIGH : LOW);
  digitalWrite(GREEN_PIN, g ? HIGH : LOW);
  digitalWrite(BLUE_PIN, b ? HIGH : LOW);
}

void beep(int delayTime, int freq) {
  tone(BUZZER_PIN, freq);
  delay(delayTime);
  noTone(BUZZER_PIN);
}

void updateDisplay(String line1, String line2) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); 
  int x1 = (SCREEN_WIDTH - (line1.length() * 12)) / 2;
  display.setCursor(x1 > 0 ? x1 : 0, 20); 
  display.print(line1);
  int x2 = (SCREEN_WIDTH - (line2.length() * 12)) / 2;
  display.setCursor(x2 > 0 ? x2 : 0, 45); 
  display.print(line2);
  display.display();
}

void playTrackAndWait(int trackNumber) {
  myDFPlayer.playMp3Folder(trackNumber);
  delay(500); 
  unsigned long startWait = millis();
  while (digitalRead(BUSY_PIN) == LOW && millis() - startWait < 5000) { 
    delay(10); 
  }
}
