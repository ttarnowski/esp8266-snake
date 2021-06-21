#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#define TFT_CS D8
#define TFT_RST D6
#define TFT_DC D1

#define DOWN_BUTTON D0
#define LEFT_BUTTON D3
#define RIGHT_BUTTON D2
#define UP_BUTTON D4

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

struct Point {
  uint8_t x;
  uint8_t y;
};

Point p;

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(DOWN_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON, INPUT_PULLUP);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);

  p.x = 80;
  p.y = 64;
}

bool is_button_pressed(uint8_t pin) { return digitalRead(pin) == LOW; }

void loop() {
  if (is_button_pressed(UP_BUTTON)) {
    p.y--;
  }

  if (is_button_pressed(DOWN_BUTTON)) {
    p.y++;
  }

  if (is_button_pressed(LEFT_BUTTON)) {
    p.x--;
  }

  if (is_button_pressed(RIGHT_BUTTON)) {
    p.x++;
  }

  delay(100);

  tft.drawPixel(p.x, p.y, ST7735_WHITE);
}