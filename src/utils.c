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

void itoa (bool is_result, int value, int num_decimals, int decimals, char *result) {  
  char *ptr = result, *ptr1 = result, tmp_char;
  int tmp_value = 0, tmp_decimals = 0;
  bool is_trailing = true;

  while (num_decimals > 0) {

    tmp_decimals = decimals % 10;
    
    // Remove trailing zeros when formatting an operation result
    if (!is_result || tmp_decimals != 0 || !is_trailing) {
      *ptr++ = abs(tmp_decimals) + '0';
      is_trailing = false;
    }

    if (abs(decimals) % power(10, num_decimals) < power(10, num_decimals - 1) && abs(decimals) != 0) {

      *ptr++ = '0';
      num_decimals--;

    }
    
    decimals /= 10;
    num_decimals--;

    if (num_decimals == 0) {
      *ptr++ = '.';
    }

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

int string_to_extended(char *operand) {  
  int result = 0;
  int operand_length = strlen(operand);
  
  // Find position of point in first operand
  int point_position = find_char(operand, '.');
    
  // In case there's no point, append decimals
  if (point_position == -1) {
    
    strcat(operand, ".00");
    
  // If there's point, calculate num of decimals  
  } else {
    
    char *decimals = (char*) malloc((operand_length - point_position) * sizeof(char));
        
    char *aux = decimals;
    for (int i = point_position + 1; i < operand_length; i++) {
      *aux++ = operand[i];
    }
    *aux = '\0';
    
    if (strlen(decimals) < MAX_DECIMALS) {
      strcat(operand, "0");
    }
    
    free(decimals);
    
  }
  
  operand_length = strlen(operand);

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
  
  if (point) {
    result /= 10;
  }
  
  if (negative) {
    result *= -1;
  }
  
  return result;
  
}

void extended_to_components(int extended, int *value, int *num_decimals, int *decimals) {
  *value = 0;
  *num_decimals = 0;
  *decimals = 0;
  
  int i = 0;
  
  int exp = 0;
  while (extended != 0) {

    if (i < MAX_DECIMALS) {
      
      *decimals += (extended % 10) * power(10, exp);
      extended /= 10;
      *num_decimals += 1;
    
            
    } else if (i == MAX_DECIMALS) {
            
      exp = -1;      
      if (*decimals == 0) {
        *num_decimals = 0;
      }
                
    } else {
      
      *value += (extended % 10) * power(10, exp);
      extended /= 10;
      
    }
    
    i++;
    exp++;
    
  }

}

int get_result(int first_value, int operation, int second_value, bool *error) {
  switch (operation) {
    
    // Addition
    case Addition:
      // Check operand signs
      if ((first_value > 0) == (second_value > 0)) {
        // Check for overflow
        if (first_value + second_value > MAX_VALUE) {
          *error = true;
        }
      }
      return first_value + second_value;
    
    // Subtraction
    case Subtraction:
      // Check operand signs
      if ((first_value > 0) == (second_value > 0)) {
        // Check for overflow
        if (first_value - second_value < MIN_VALUE) {
          *error = true;
        }
      }
      return first_value - second_value;
    
    // Multiplication
    case Multiplication:
      // Check operand signs
      if ((first_value > 0) == (second_value > 0)) {
        // Check for overflow
        if (first_value > get_result(MAX_VALUE, Division, second_value, error)) {
          *error = true;
        }
      }
      // Calculate integer and decimal separatedly, then add them
      return (first_value / 100) * second_value + ((first_value % 100) * second_value) / 100;
    
    // Division
    default:      
      if (second_value == 0) {
        *error = true;
        return -1;
      }
      // If there's enough margin, increase precision;
      if (first_value < MAX_VALUE / 100) {
        return (first_value * 100) / second_value;
      // Otherwise, get rid of second value's decimals
      } else {
        return first_value / (second_value / 100);
      }
      
  }
}