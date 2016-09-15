#pragma once
#include <pebble.h>
#include <limits.h>

#define MAX_NUM_CHARS 8
#define MAX_CHARACTERS 10000000
#define MAX_DECIMALS 3
#define MULTIPLY_FACTOR 1000

static const int MAX_VALUE = INT_MAX;
static const int MIN_VALUE = INT_MIN;

enum Operation {
  Addition = 1,
  Subtraction = 2,
  Multiplication = 3,
  Division = 4,
  Squared = 5,
  Root = 6
};

int increase_value(int value);
int negate(int value);
int add_figure(int value);
int remove_figure(int value);
int find_char(char *string, char char_to_find);
void components_to_string(bool is_result, int value, int num_decimals, int decimals, char *result);
int string_to_extended(char *operand);
void extended_to_components(int extended, int *value, int *num_decimals, int *decimals);
int get_result(int first_value, int operation, int second_value, bool *error);