#pragma once
#include <pebble.h>

enum Operation {
  Addition = 1,
  Subtraction = 2,
  Multiplication = 3,
  Division = 4
};

int increase_value(int value);
int add_figure(int value);
int remove_figure(int value);
void int_to_string(int value, char *string);
int get_result(int first_value, int operation, int second_value);