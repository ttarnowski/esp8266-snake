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

#define WIDTH 160
#define HEIGHT 128

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

struct Point {
  uint8_t x;
  uint8_t y;
};

struct Game {
  Game(Snake &s) : snake_(s) {}

  enum Pixel { Empty = 0, Up = 1, Left = 2, Right = 3, Down = 4, Head = 5 };

  void move_snake() {}

private:
  Snake &snake_;
  uint8_t field[WIDTH][HEIGHT];
};

struct Snake {
  friend Game;
  enum Direction { Up = 1, Left = 2, Right = 3, Down = 4 };

  Snake(uint8_t w, uint8_t h) {
    head_.x = w / 2 + 3;
    tail_.x = w / 2 - 3;

    tail_.y = head_.y = h / 2;

    direction_ = Right;
  }

  void set_direction(Direction d) { direction_ = d; }

  const Point &get_head() { return head_; }

  const Point &get_tail() { return tail_; }

private:
  Point head_;
  Point tail_;
  Direction direction_;
};

Snake snake(WIDTH, HEIGHT);

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
}

bool is_button_pressed(uint8_t pin) { return digitalRead(pin) == LOW; }

void loop() {
  if (is_button_pressed(UP_BUTTON)) {
  }

  if (is_button_pressed(DOWN_BUTTON)) {
  }

  if (is_button_pressed(LEFT_BUTTON)) {
  }

  if (is_button_pressed(RIGHT_BUTTON)) {
  }

  delay(100);
}