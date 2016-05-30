#pragma once
#include <pebble.h>

#define NUM_ROWS 4

enum Operation {
  Addition = 1,
  Subtraction = 2,
  Multiplication = 3,
  Division = 4
};

void operation_window_push();