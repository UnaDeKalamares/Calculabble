#pragma once
#include <pebble.h>

#define MAX_CHARACTERS 10000000
#define MAX_VALUE 99999999
#define MIN_VALUE -9999999

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