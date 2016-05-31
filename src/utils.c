#include "utils.h"

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

int add_figure(int value) {
  if (value < MAX_CHARACTERS) {
    
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

// Source: https://gist.github.com/rageandqq/2ab31c71e5c2185e20d2
void itoa (int value, int num_decimals, int decimals, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; }

    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value, tmp_decimals;
    bool point = false;
  
    while (num_decimals > 0) {
            
      if (decimals % power(10, num_decimals) < power(10, num_decimals - 1)) {
        
        *ptr++ = "0"[0];
        
      } else {
        
        tmp_decimals = decimals;
        decimals /= base;
        
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_decimals - decimals * base)];
        
      }
      
      num_decimals--;
      
      if (num_decimals == 0 && !point) {
        *ptr++ = "."[0];
        point = true;
      }
      
    }  
  
    do {
      
        tmp_value = value;
        value /= base;
            
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
      
    } while ( value );   

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }

}

void int_to_string(int value, int num_decimals, int decimals, char *string) {
  itoa(value, num_decimals, decimals, string, 10);
}

int get_result(int first_value, int operation, int second_value) {
  switch (operation) {
    // Addition
    case Addition:
      return first_value + second_value;
    // Subtraction
    case Subtraction:
      return first_value - second_value;
    // Multiplication
    case Multiplication:
      return first_value * second_value;
    // Division
    default:
      if (second_value == 0) {
        return 0;
      } else {
        return first_value / second_value;
      }
  }
}