#include "utils.h"

/**************************************************************************
  Copyright (C) 2016 CarlosPC

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

int power(int a, int b){
  int result = 1;
  for (int i = 0; i < b; i++) {
    
    result *= a;
    
  }
  return result;
}

int increase_value(int value) {
  if (value % 10 == 9) {
    
    value -= 9;
    
  } else {
    
    value++;
    
  }
  return value;
}

int negate(int value) {
  return value *= -1;
}

int add_figure(int value) {
  if (value < power(10, MAX_NUM_CHARS)) {
    
    return value *= 10;
    
  } else {
    
    return value;
    
  }
}

int remove_figure(int value) {
  if (value >= 10) {
    
    return value /= 10;
    
  } else {
    
    return value;
    
  }
}

// Find character in string
int find_char(char *string, char char_to_find) {
  char* ptr = string;
  int occurence = -1;
  bool found = false;
  
  while (*ptr != '\0' && !found) {
    
    occurence++;
    if (*ptr == char_to_find) {
      found = true;
    }
    ptr++;
    
  }
  
  if (found) {
    return occurence;
  } else {
    return -1;
  }
}

// Based on rageandqq implementation: https://gist.github.com/rageandqq/2ab31c71e5c2185e20d2
// Transform number in components into string
void components_to_string(bool is_result, int value, int num_decimals, int decimals, char *result) {
  char *ptr = result, *ptr1 = result, tmp_char;
  int tmp_value = 0, tmp_decimals = 0;
  int tmp_num_decimals = num_decimals;
  bool is_trailing = true;  
  
  while (tmp_num_decimals > 0) {

    tmp_decimals = decimals % 10;
    
    // Don't add trailing zeros when formatting an operation result,
    if (!is_result || tmp_decimals != 0 || !is_trailing) {
      *ptr++ = abs(tmp_decimals) + '0';
      is_trailing = false;
    }
    
    decimals /= 10;
    tmp_num_decimals--;

  }
  
  // Add leading zeros
  if (tmp_num_decimals > 0) {
    *ptr++ = '0';
  }
  
  // Add point only if number has decimals
  if (num_decimals != 0) {
    *ptr++ = '.';
  }

  do {

    tmp_value = value % 10;   
    value /= 10;
    
    *ptr++ = abs(tmp_value) + '0';
      
  } while ( value );   

  // Apply negative sign
  if (tmp_value < 0 || tmp_decimals < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr--= *ptr1;
    *ptr1++ = tmp_char;
  }

}

// Transform string into extended integer
int string_to_extended(char *operand) {  
  int result = 0;
  int operand_length = strlen(operand);
  
  // Find position of point in first operand
  int point_position = find_char(operand, '.');
    
  // In case there's no point, append decimals
  if (point_position == -1) {
    
    strcat(operand, ".000");
    
  // If there's point, calculate num of decimals  
  } else {
    
    char *decimals = (char*) malloc((operand_length - point_position) * sizeof(char));
        
    char *aux = decimals;
    for (int i = point_position + 1; i < operand_length; i++) {
      *aux++ = operand[i];
    }
    *aux = '\0';
    
    // Add trailing zeros to reach max decimals
    for (int i = strlen(decimals); i < MAX_DECIMALS; i++) {
      strcat(operand, "0");
    }
    
    free(decimals);
    
  }
  
  operand_length = strlen(operand);

  // Parse string
  int exp = 1;
  bool point = false;
  bool negative = false;
  for (int i = 0 ; i < operand_length; i++) {
    
    char current_char = operand[i];
        
    if (current_char != '.') {
      
      if (current_char == '-') {
        
        negative = true;
        
      } else {
        
        // Get char as int
        int current_int = current_char - '0';
        result += current_int * power(10, operand_length - exp);
        
      }
      
    } else {
      
      exp--;
      point = true;
      
    }
    
    exp++;
    
  }
  
  // If string had point char, decrease result
  if (point) {
    result /= 10;
  }
  
  // Apply sign
  if (negative) {
    result *= -1;
  }
  
  return result;
  
}

// Transform extended int into number in components (used for creating string)
void extended_to_components(int extended, int *value, int *num_decimals, int *decimals) {
  *value = 0;
  *num_decimals = MAX_DECIMALS;
  *decimals = 0;
  
  int current_num_decimals = 0;
  
  int exp = 0;
  while (extended != 0) {

    if (current_num_decimals < MAX_DECIMALS) {
      
      *decimals += (extended % 10) * power(10, exp);
      extended /= 10;    
            
    // Check if there weren't decimals
    } else if (current_num_decimals == MAX_DECIMALS) {
            
      exp = -1;      
      if (*decimals == 0) {
        *num_decimals = 0;
      }
                
    } else {
      
      *value += (extended % 10) * power(10, exp);
      extended /= 10;
      
    }
    
    current_num_decimals++;
    exp++;
    
  }

}

// Perform operation
int get_result(int first_value, int operation, int second_value, bool *error) {
  switch (operation) {
    // Addition
    case Addition:
    {
      // Check operand signs
      if ((first_value > 0) == (second_value > 0)) {
        // Check for overflow
        if (first_value > MAX_VALUE - second_value) {
          *error = true;
        }
      }
      return first_value + second_value;
    }
    // Subtraction
    case Subtraction:
    {
      // Check operand signs
      if ((first_value > 0) == (second_value > 0)) {
        // Check for overflow
        if (first_value < MIN_VALUE + second_value) {
          *error = true;
        }
      }
      return first_value - second_value;
    }
    // Multiplication
    case Multiplication:
    {
      // Check for overflow
      if (second_value / MULTIPLY_FACTOR != 0 && first_value > get_result(MAX_VALUE, Division, second_value, error)) {
        *error = true;
      }
      
      // Calculate integer and decimal separatedly, then add them
      return (first_value / MULTIPLY_FACTOR) * second_value + ((first_value % MULTIPLY_FACTOR) * second_value) / MULTIPLY_FACTOR;
    }
    // Division
    case Division:
    {
      if (second_value == 0) {
        *error = true;
        return -1;
      }
      // If there's enough margin, increase precision;
      if (first_value < MAX_VALUE / MULTIPLY_FACTOR) {
        return (first_value * MULTIPLY_FACTOR) / second_value;
      // Otherwise, get rid of second value's decimals
      } else {
        return first_value / (second_value / MULTIPLY_FACTOR);
      }
    }
    // Squared
    case Squared:
    {
      if (first_value < 0) {
        first_value *= -1;
      }
      return get_result(first_value, Multiplication, first_value, error);
    }
    // Root
    default:      
    {
      // Disallow negative roots
      if (first_value < 0) {
        *error = true;
        return -1;
      }
      // Babylon approximation method of square roots with 10 iterations
      // https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method
      int last_intermediate = 1000;
      // For bigger numbers, increase starting value
      if (first_value > 9999999) {
        last_intermediate = 1000000;
      }
      int one_half = get_result(1, Division, 2, error);
      for (int i = 0; i < 9; i++) {
        int b = get_result(first_value, Division, last_intermediate, error);
        int a = get_result(last_intermediate, Addition, b, error);
        last_intermediate = get_result(one_half, Multiplication, a, error);
      }
      return last_intermediate;
    }
  }
}