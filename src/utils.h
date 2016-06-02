#pragma once
#include <pebble.h>

#define MAX_NUM_CHARS 
#define MAX_CHARACTERS 1000000
#define MAX_VALUE 214748364
#define MIN_VALUE -2147483
#define MAX_DECIMALS 2

enum Operation {
  Addition = 1,
  Subtraction = 2,
  Multiplication = 3,
  Division = 4
};

int increase_value(int value);
int add_figure(int value);
int remove_figure(int value);
int find_char(char *string, char char_to_find);
void itoa (bool is_result, int value, int num_decimals, int decimals, char *result);
int string_to_extended(char *operand);
void extended_to_components(int extended, int *value, int *num_decimals, int *decimals);
int get_result(int first_value, int operation, int second_value, bool *error);