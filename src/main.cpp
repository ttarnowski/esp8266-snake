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

#define SPEED 75

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

struct Point {
  uint8_t x;
  uint8_t y;
};

struct Game;

struct Snake {
  friend Game;
  enum Direction { Up = 1, Left = 2, Right = 3, Down = 4 };

  Snake(uint8_t w, uint8_t h) {
    head_.x = w / 2 + 3;
    tail_.x = w / 2 - 3;

    tail_.y = head_.y = h / 2;

    direction_ = Right;
  }

  void set_direction(Direction new_direction) {
    if ((direction_ == Up && new_direction == Down) ||
        (direction_ == Down && new_direction == Up) ||
        (direction_ == Left && new_direction == Right) ||
        (direction_ == Right && new_direction == Left)) {
      return;
    }

    direction_ = new_direction;
  }

  const Point &get_head() { return head_; }

  const Point &get_tail() { return tail_; }

  const Direction &get_direction() { return direction_; }

private:
  Point head_;
  Point tail_;
  Direction direction_;
};

struct Game {
  Game(Snake &s) : snake_(s) {
    for (uint8_t x = s.get_tail().x; x < s.get_head().x; x++) {
      field_[x][s.get_head().y] = Right;
    }

    field_[s.get_head().x][s.get_head().y] = Head;
  }

  enum Pixel { Empty = 0, Up = 1, Left = 2, Right = 3, Down = 4, Head = 5 };

  void move_snake() {
    this->move_head();
    this->move_tail();
  }

private:
  void move_head() {
    switch (snake_.get_direction()) {
    case Snake::Direction::Up:
      field_[snake_.get_head().x][snake_.get_head().y] = Pixel::Up;
      field_[snake_.get_head().x][snake_.get_head().y - 1] = Pixel::Head;

      snake_.head_.y--;
      break;

    case Snake::Direction::Left:
      field_[snake_.get_head().x][snake_.get_head().y] = Pixel::Left;
      field_[snake_.get_head().x - 1][snake_.get_head().y] = Pixel::Head;

      snake_.head_.x--;
      break;

    case Snake::Direction::Right:
      field_[snake_.get_head().x][snake_.get_head().y] = Pixel::Right;
      field_[snake_.get_head().x + 1][snake_.get_head().y] = Pixel::Head;

      snake_.head_.x++;
      break;

    // Down
    default:
      field_[snake_.get_head().x][snake_.get_head().y] = Pixel::Down;
      field_[snake_.get_head().x][snake_.get_head().y + 1] = Pixel::Head;

      snake_.head_.y++;
      break;
    }
  }

  void move_tail() {
    uint8_t tail_x = snake_.get_tail().x;
    uint8_t tail_y = snake_.get_tail().y;

    switch (field_[tail_x][tail_y]) {
    case Pixel::Right:
      snake_.tail_.x++;
      break;

    case Pixel::Left:
      snake_.tail_.x--;
      break;

    case Pixel::Up:
      snake_.tail_.y--;
      break;

    // Down
    default:
      snake_.tail_.y++;
      break;
    }

    field_[tail_x][tail_y] = Pixel::Empty;
  }

  Snake &snake_;
  uint8_t field_[WIDTH][HEIGHT];
};

Snake snake(WIDTH, HEIGHT);
Game game(snake);

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

  // draw snake
  for (uint8_t x = snake.get_tail().x; x <= snake.get_head().x; x++) {
    tft.drawPixel(x, snake.get_head().y, ST7735_WHITE);
  }
}

bool is_button_pressed(uint8_t pin) { return digitalRead(pin) == LOW; }

void loop() {
  if (is_button_pressed(UP_BUTTON)) {
    snake.set_direction(Snake::Direction::Up);
  }

  if (is_button_pressed(DOWN_BUTTON)) {
    snake.set_direction(Snake::Direction::Down);
  }

  if (is_button_pressed(LEFT_BUTTON)) {
    snake.set_direction(Snake::Direction::Left);
  }

  if (is_button_pressed(RIGHT_BUTTON)) {
    snake.set_direction(Snake::Direction::Right);
  }

  tft.drawPixel(snake.get_tail().x, snake.get_tail().y, ST7735_BLACK);
  game.move_snake();
  tft.drawPixel(snake.get_head().x, snake.get_head().y, ST7735_WHITE);

  delay(SPEED);
}
