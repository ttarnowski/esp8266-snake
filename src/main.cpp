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
    head_.x = w / 2 + 10;
    tail_.x = w / 2 - 10;

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

    for (uint8_t x = 0; x < WIDTH; x++) {
      field_[x][0] = field_[x][HEIGHT - 1] = Pixel::Wall;
    }

    for (uint8_t y = 0; y < HEIGHT; y++) {
      field_[0][y] = field_[WIDTH - 1][y] = Pixel::Wall;
    }

    is_over_ = false;
  }

  enum Pixel {
    Empty = 0,
    Up = 1,
    Left = 2,
    Right = 3,
    Down = 4,
    Head = 5,
    Wall = 6
  };

  void move_snake() {
    this->move_head();
    this->move_tail();
  }

  bool is_over() { return is_over_; }

  void field_for_each(std::function<void(uint8_t, uint8_t, Pixel)> fn) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      for (uint8_t y = 0; y < HEIGHT; y++) {
        fn(x, y, (Pixel)field_[x][y]);
      }
    }
  }

private:
  void move_head() {
    uint8_t new_x = snake_.get_head().x;
    uint8_t new_y = snake_.get_head().y;
    Pixel p;

    switch (snake_.get_direction()) {
    case Snake::Direction::Up:
      new_y--;
      p = Pixel::Up;
      break;

    case Snake::Direction::Left:
      p = Pixel::Left;
      new_x--;
      break;

    case Snake::Direction::Right:
      p = Pixel::Right;
      new_x++;
      break;

    // Down
    default:
      p = Pixel::Down;
      new_y++;
      break;
    }

    if (field_[new_x][new_y] != Pixel::Empty) {
      is_over_ = true;
      return;
    }

    field_[snake_.get_head().x][snake_.get_head().y] = p;
    field_[new_x][new_y] = Pixel::Head;

    snake_.head_.x = new_x;
    snake_.head_.y = new_y;
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
  bool is_over_;
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
  game.field_for_each([](uint8_t x, uint8_t y, Game::Pixel p) {
    if (p != Game::Pixel::Empty) {
      tft.drawPixel(x, y, ST7735_WHITE);
    }
  });
}

bool is_button_pressed(uint8_t pin) { return digitalRead(pin) == LOW; }

void loop() {
  if (game.is_over()) {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, HEIGHT / 3);
    tft.setTextColor(ST7735_RED);
    tft.setTextSize(2);
    tft.print("Game Over");
    delay(3000);
    ESP.restart();
  }

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
