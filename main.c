#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
  #include <windows.h>
  #define usleep Sleep
#else
  #include <unistd.h>
  #define usleep(TIME) sleep(TIME / 1000)
#endif

#define DISPLAY_WIDTH  50
#define DISPLAY_HEIGHT 50

#define FPS            20
#define DT             (1.0f / FPS)

#define BALL_RADIUS    5
#define BALL_X_VEL     (1.5f / DT)
#define GRAVITY        98
#define VEL_LOSS       0.60

static_assert(
  BALL_RADIUS <= DISPLAY_WIDTH / 2 && BALL_RADIUS <= DISPLAY_HEIGHT / 2 && BALL_RADIUS > 0,
  "Ball radius cannot be bigger than display width/height and cannot be lower than 0!"
);

typedef enum State {
  STATE_OFF = 0,
  STATE_ON  = 1,
} State;

static State display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
static char display_chars[2][2] = {
  {' ', '_'},
  {'"', '#'},
};

void fill(State s) {
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      display[y * DISPLAY_WIDTH + x] = s;
    }
  }
}

void update(int cx, int cy) {
  fill(STATE_OFF);

  int lower_bound_y = max(0, cy - BALL_RADIUS);
  int upper_bound_y = min(cy + BALL_RADIUS, DISPLAY_HEIGHT);

  int lower_bound_x = max(0, cx - BALL_RADIUS);
  int upper_bound_x = min(cx + BALL_RADIUS, DISPLAY_WIDTH);

  for (int y = lower_bound_y; y < upper_bound_y; y++) {
    for (int x = lower_bound_x; x < upper_bound_x; x++) {
      float dx = x - cx + 0.5;
      float dy = y - cy + 0.5;

      display[y * DISPLAY_WIDTH + x] = dx*dx + dy*dy <= BALL_RADIUS*BALL_RADIUS ? STATE_ON : STATE_OFF;
    }
  }
}

void show(void) {
  for (int y = 0; y < DISPLAY_HEIGHT / 2; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      State top    = display[(y * 2 + 0) * DISPLAY_WIDTH + x];
      State bottom = display[(y * 2 + 1) * DISPLAY_WIDTH + x];

      putc(display_chars[top][bottom], stdout);
    }
    putc('\n', stdout);
  }
  putc('\n', stdout);
}

void back(void) {
  printf("\x1b[%dA", DISPLAY_HEIGHT / 2 + 1);
}

int main(void) {
  int x = 0;
  int y = 0;

  float vx = BALL_X_VEL, vy = 0;
  int run = 1;

  // Hide cursor
  printf("\x1b[?25l");

  while (run) {
    vy += GRAVITY * DT;

    x  += vx * DT;
    y  += vy * DT;

    if ((int)vx <= 0 / DT || (int)vy == 0) run = 0;

    if (y >= DISPLAY_HEIGHT - BALL_RADIUS) {
      vx -=  VEL_LOSS / 2.0f;
      vy *= -VEL_LOSS;
      y   =  DISPLAY_HEIGHT - BALL_RADIUS;
    }

    if (x > DISPLAY_WIDTH + BALL_RADIUS) {
      x = -BALL_RADIUS;
    }

    update(x, y);
    show();
    back();

    usleep(DT * 1000);
  }

  // We wait on a still screen for a second then we clear the display
  usleep(1000);

  fill(STATE_OFF);
  show();
  back();

  // Show cursor
  printf("\x1b[?25h");

  return 0;
}
