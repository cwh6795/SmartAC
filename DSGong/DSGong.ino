#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <pm2008_i2c.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_PIN    3
#define FAN_PIN    5
#define LED_COUNT 18
#define BUTTON_A  13

int FAN_SPD = 30;

int blueTx=10;
int blueRx=11;

PM2008_I2C pm2008_i2c;
int p1p0 = 0;
int p2p5 = 0;
int p10p0 = 0;

int MODE = 1;

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
SoftwareSerial SBlueT(blueTx, blueRx);
void colorWipe(uint32_t color, int wait);
void theaterChase(uint32_t color, int wait);
void rainbow(int wait);
void theaterChaseRainbow(int wait);
void check_dustval(int p2p5, int p10p0, int *p2p5_val, int *p10p0_val);
void print_dust_lcd(int p2p5, int p10p0, int total_val);


void setup() {
  Serial.begin(9600);
  SBlueT.begin(9600);
  pm2008_i2c.begin();
  pm2008_i2c.command();
  Serial.println("HELLO");

  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println("OLED begun");

  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
  pinMode(BUTTON_A, INPUT_PULLUP);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
}

void loop() {
  int BTN = digitalRead(BUTTON_A);
  if (BTN) {
    if (MODE == 0) {
      FAN_SPD = 31;
      MODE += 1;
    }
    else if (MODE == 1) {
      FAN_SPD = 42;
      MODE += 1;
    }
    else if (MODE == 2) {
      FAN_SPD = 20;
      MODE -= 2;
    }
  }

  if (Serial.available()) {         
    btSerial.write(Serial.read());
  }
  
  display.clearDisplay();
  display.setCursor(0,0); 
  display.display();
  
  uint8_t ret = pm2008_i2c.read();
  if (ret != 0) {
    Serial.println("PM2008 Read Error");
  }

  if (ret == 0) {
    p1p0 = pm2008_i2c.pm1p0_grimm;
    p2p5 = pm2008_i2c.pm2p5_grimm;
    p10p0 = pm2008_i2c.pm10_grimm;
  }
  else {
    p2p5 = 16; // Test Value
    p10p0 = 400; // Test Value
  } // TEST CODE
  
  int p2p5_val, p10p0_val;
  check_dustval(p2p5, p10p0, &p2p5_val, &p10p0_val);

  Serial.print("p2p5 : ");
  Serial.println(p2p5);
  Serial.print("p10p0 : ");
  Serial.println(p10p0);
  Serial.print("p2p5_val : ");
  Serial.println(p2p5_val);
  Serial.print("p10p0_val : ");
  Serial.println(p10p0_val);

  print_dust_lcd(p2p5, p10p0, p2p5_val + p10p0_val);
  colorWipe(strip.Color(((p2p5_val + p10p0_val) * FAN_SPD), 255 - ((p2p5_val + p10p0_val) * FAN_SPD), 0), 50 * (1 + MODE));
  analogWrite(FAN_PIN, FAN_SPD * (p2p5_val + p10p0_val));
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(50);                           //  Pause for a moment
  }
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void check_dustval(int p2p5, int p10p0, int *p2p5_val, int *p10p0_val) {
  if (p2p5 <= 15) *p2p5_val = 0;
  else if (p2p5 <= 35) *p2p5_val = 1;
  else if (p2p5 <= 75) *p2p5_val = 2;
  else *p2p5_val = 3;

  if (p10p0 <= 30) *p10p0_val = 0;
  else if (p10p0 <= 80) *p10p0_val = 1;
  else if (p10p0 <= 150) *p10p0_val = 2;
  else *p10p0_val = 3;
}

void print_dust_lcd(int p2p5, int p10p0, int total_val) {
  display.setCursor(0,0);
  display.print("PM2.5 : "); // pm 1.0
  display.println(p2p5); // pm 1.0
  display.print("PM10 :  "); // pm 1.0
  display.println(p10p0); // pm 1.0
  if (total_val == 0) 
    display.println("Status = Good");
  else if (total_val <= 2)
    display.println("Status = Not Bad");
  else if (total_val <= 4)
    display.println("Status = Bad");
  else if (total_val <= 6)
    display.println("Status = Really Bad");

  if (MODE == 0)
    display.println("MODE : SILENCE");
  else if (MODE == 1)
    display.println("MODE : NORMAL");
  else if (MODE == 2)
    display.println("MODE : TURBO");
  display.display();
}
