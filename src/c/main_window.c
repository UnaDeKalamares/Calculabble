#include "main_window.h"
#include "utils.h"
#include "localize.h"
  
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

static Window *window;
static GRect window_bounds;

static Layer *divider_layer;

static TextLayer *first_operand_text_layer;
static char *first_operand_string;

static BitmapLayer *operation_bitmap_layer;
static GBitmap *operation_bitmap;

static TextLayer *second_operand_text_layer;
static char *second_operand_string;

static int current_value = 0;
static int current_num_decimals = 0;
static int current_decimals = 0;

static int last_operation = -1;

static int left_margin = 0;

static ActionBarLayer *action_bar_layer;
static GBitmap *up_bitmap;
static GBitmap *select_bitmap;
static GBitmap *down_bitmap;

// Declare method prototypes
void operation_click_config_provider(void *context);
void result_click_config_provider(void *context);
void increase_value_click_handler(ClickRecognizerRef recognizer, void *context);
void negate_operand_click_handler(ClickRecognizerRef recognizer, void *context);
void add_figure_click_handler(ClickRecognizerRef recognizer, void *context);
void remove_figure_click_handler(ClickRecognizerRef recognizer, void *context);
void add_point_click_handler(ClickRecognizerRef recognizer, void *context);
void select_last_operation_click_handler(ClickRecognizerRef recognizer, void *context);
void select_operation_click_handler(ClickRecognizerRef recognizer, void *context);
void result_click_handler(ClickRecognizerRef recognizer, void *context);
void set_text_current_operand(bool is_result);
void perform_operation();
void redraw();

static void init_operation_bitmap_layer() {
  // Create operation bitmap layer
	operation_bitmap_layer = bitmap_layer_create((GRect) {
    .origin = {left_margin, window_bounds.size.h / 3},
    .size = {window_bounds.size.w - ACTION_BAR_WIDTH, window_bounds.size.h / 3}
  });
      
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(operation_bitmap_layer));
}

// Draw divider procedure
static void divider_layer_update_proc(Layer *layer, GContext *ctx) {
  // Set vertices
  GRect rect_bounds = GRect(0, window_bounds.size.h / 3, window_bounds.size.w, window_bounds.size.h / 3);
  
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorVividCerulean);
  #else
    graphics_context_set_fill_color(ctx, GColorBlack);
  #endif
  
  graphics_fill_rect(ctx, rect_bounds, 0, GCornersAll);

}

// Called on .load handler event, init layout
static void window_load() {  
  Layer *window_layer = window_get_root_layer(window);
  window_bounds = layer_get_bounds(window_layer);
  
  // Create divider layer
  divider_layer = layer_create(window_bounds);
  
  // Assign the custom drawing procedure
  layer_set_update_proc(divider_layer, divider_layer_update_proc);

  // Add the divider layer to the window
  layer_add_child(window_get_root_layer(window), divider_layer);
        
  // Add left margin to UI components when running on round display
  #if defined(PBL_ROUND)
    left_margin = 10;
  #endif
  
  // Create first operand text layer
	first_operand_text_layer = text_layer_create((GRect) {
    .origin = {left_margin, window_bounds.size.h / 9},
    .size = {window_bounds.size.w - ACTION_BAR_WIDTH, window_bounds.size.h / 3}
  });
  
  // Set the font and text alignment
	text_layer_set_font(first_operand_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(first_operand_text_layer, GTextAlignmentCenter);
  
  // Set transparent background color
  text_layer_set_background_color(first_operand_text_layer, GColorClear);
  
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(first_operand_text_layer));
    
  // Create operation bitmap layer
	init_operation_bitmap_layer();
  
  // Create second operand text layer
  second_operand_text_layer = text_layer_create((GRect) {
    .origin = {left_margin, window_bounds.size.h * 2 / 3},
    .size = {window_bounds.size.w - ACTION_BAR_WIDTH, window_bounds.size.h / 3}
  });
  
  // Set the font and text alignment
	text_layer_set_font(second_operand_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(second_operand_text_layer, GTextAlignmentCenter);
  
  // Set transparent background color
  text_layer_set_background_color(second_operand_text_layer, GColorClear);
  
	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(second_operand_text_layer));
  
  // Create action bar layer
  action_bar_layer = action_bar_layer_create();
  
  // Define action bar icons
  up_bitmap = gbitmap_create_with_resource(RESOURCE_ID_INCREASE);
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_UP, up_bitmap, true);
  select_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OPERATION);
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_SELECT, select_bitmap, true);
  down_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ADD_FIGURE);
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_DOWN, down_bitmap, true);
  
  // Define click_config_provider for click handling
  action_bar_layer_set_click_config_provider(action_bar_layer, operation_click_config_provider);
  
  // Add the action bar to the window
  action_bar_layer_add_to_window(action_bar_layer, window);
  
  // Initialize values
  // Set first operand as current
  first_operand = true;
  
  // Allocate string memory
  first_operand_string = (char*) malloc(15 * sizeof(char));
  second_operand_string = (char*) malloc(15 * sizeof(char));
  
  // Init first_operand_string
  components_to_string(false, current_value, current_num_decimals, current_decimals, first_operand_string);
  text_layer_set_text(first_operand_text_layer, first_operand_string);
  
}

// Define click handlers for each button interactor (when introducing first operand)
void operation_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) increase_value_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 0, (ClickHandler) negate_operand_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) add_figure_click_handler);
  window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 0, 0, false, (ClickHandler) remove_figure_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 0, (ClickHandler) add_point_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_operation_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) select_last_operation_click_handler, NULL);
}

// Define click handlers for each button interactor (when introducing second operand)
void result_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) increase_value_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 0, (ClickHandler) negate_operand_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) add_figure_click_handler);
  window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 0, 0, false, (ClickHandler) remove_figure_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 0, (ClickHandler) add_point_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) result_click_handler);
}

bool is_error_state() {
  const char *first = text_layer_get_text(first_operand_text_layer);
  if (strcmp(first, _("Division by")) == 0 || strcmp(first, _("Result")) == 0) {
    text_layer_set_text(first_operand_text_layer, first_operand_string);
    // Reset second operand text layer
    text_layer_set_text(second_operand_text_layer, "");
    return true;
  } else {
    return false;
  }
}

// Implement increase value handler
void increase_value_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!is_error_state()) {
    if (current_num_decimals > 0) {
      current_decimals = increase_value(current_decimals);
    } else {
      current_value = increase_value(current_value);
    }
    set_text_current_operand(false);
  }
}

// Implement negate operand handler
void negate_operand_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_value = negate(current_value);
  set_text_current_operand(false);
}

// Implement add figure handler
void add_figure_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!is_error_state()) {
    if (current_num_decimals > 0) {
      if (current_num_decimals < MAX_DECIMALS) {
        current_num_decimals++;
        current_decimals = add_figure(current_decimals);
      }
    } else {
      current_value = add_figure(current_value);
    }
    set_text_current_operand(false);
  }
}

// Implement remove figure handler
void remove_figure_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!is_error_state()) {
    if (current_num_decimals > 0) {
      current_decimals = remove_figure(current_decimals);
      current_num_decimals--;
    } else {
      current_value = remove_figure(current_value);
    }
    set_text_current_operand(false);
  }
}

// Implement add point handler
void add_point_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!is_error_state()) {
    if (current_num_decimals == 0) {
      current_num_decimals++;
      set_text_current_operand(false);
    }
  }
}

// Implement select last operation handler
void select_last_operation_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (last_operation != -1) {
    operation_enum = last_operation;
    first_operand = false;
    redraw();
  } else {
    if (!is_error_state()) {
      operation_window_push();
    }
  }
}

// Implement select operand handler
void select_operation_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!is_error_state()) {
    operation_window_push();
  }
}

// Implement result handler
void result_click_handler(ClickRecognizerRef recognizer, void *context) {
    perform_operation();  
}

void perform_operation() {
  // Get operands as ints
  int first_operand_int = string_to_extended(first_operand_string);
  int second_operand_int = string_to_extended(second_operand_string);
  
  // Reset window
  // Set first operand as current
  first_operand = true;
          
  // Perform calculation
  bool error = false;
  int extended_current_value = get_result(first_operand_int,  operation_enum, second_operand_int, &error);
  
  if (error) {
    
    if (operation_enum == Division && extended_current_value == -1) {

      text_layer_set_text(first_operand_text_layer, _("Division by"));
      text_layer_set_text(second_operand_text_layer, _("zero!"));

    } else {

      text_layer_set_text(first_operand_text_layer, _("Result"));
      text_layer_set_text(second_operand_text_layer, _("too big!"));

    }

    // Restore default value for first operand
    components_to_string(false, 0, 0, 0, first_operand_string);

  } else {

    extended_to_components(extended_current_value, &current_value, &current_num_decimals, &current_decimals);

    // Set calculation result as first operand
    set_text_current_operand(true);

    // Reset second operand text layer
    text_layer_set_text(second_operand_text_layer, "");

  } 
  
  // Restore current value
  current_value = 0;
  current_num_decimals = 0;
  current_decimals = 0;
  
  // Remove bitmap (necessary dealloc and alloc)
  layer_remove_from_parent(bitmap_layer_get_layer(operation_bitmap_layer));
  bitmap_layer_destroy(operation_bitmap_layer);
  init_operation_bitmap_layer();
  
  // Restore select button icon
  if (!select_bitmap) {
    gbitmap_destroy(select_bitmap);
  }
  select_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OPERATION);
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_SELECT, select_bitmap, true);
  
  // Restore action bar click config provider
  action_bar_layer_set_click_config_provider(action_bar_layer, operation_click_config_provider);
}

// Set current value in the correct text layer
void set_text_current_operand(bool is_result) {
  if (first_operand) {
    components_to_string(is_result, current_value, current_num_decimals, current_decimals, first_operand_string);
    text_layer_set_text(first_operand_text_layer, first_operand_string);
  } else {
    components_to_string(is_result, current_value, current_num_decimals, current_decimals, second_operand_string);
    text_layer_set_text(second_operand_text_layer, second_operand_string);
  }
}

// Called when window shows up
static void window_appear() {
  redraw();
}

// Redraw main window to reflect current state
void redraw() {
  // If we chose an operation
  if (!first_operand) {
    
    // Release if not null
    if (!operation_bitmap) {
      gbitmap_destroy(operation_bitmap);
    }
    // Init bitmap
    switch(operation_enum) {
      case Addition:
        operation_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ADDITION);
        break;
      case Subtraction:
        operation_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SUBTRACTION);
        break;
      case Multiplication:
        operation_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MULTIPLICATION);
        break;
      case Division:
        operation_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DIVISION);
        break;
      case Squared:
        operation_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SQUARED);
        break;
      case Root:
        operation_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ROOT);
    }
    
    // Save last operation
    last_operation = operation_enum;
    
    if (last_operation == Squared || last_operation == Root) {
  
      perform_operation();
      
    } else {
    
      // Set operation bitmap
      bitmap_layer_set_bitmap(operation_bitmap_layer, operation_bitmap);
      // Change bitmap layer background color to transparent
      bitmap_layer_set_background_color(operation_bitmap_layer, GColorClear);
      // Set bitmap composition to use transparent background
      bitmap_layer_set_compositing_mode(operation_bitmap_layer, GCompOpSet);
    
      // Set second operand text layer value
      current_value = 0;
      current_num_decimals = 0;
      current_decimals = 0;
      components_to_string(false, current_value, current_num_decimals, current_decimals, second_operand_string);
      text_layer_set_text(second_operand_text_layer, second_operand_string);
      
      // Replace action bar select button icon
      if (!select_bitmap) {
        gbitmap_destroy(select_bitmap);
      }
      select_bitmap = gbitmap_create_with_resource(RESOURCE_ID_RESULT);
      action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_SELECT, select_bitmap, true);
      
      // Replace action bar click config provider
      action_bar_layer_set_click_config_provider(action_bar_layer, result_click_config_provider);
      
    }
  }
}

// Called on .unload handler event, destroy layout
static void window_unload() {
  // Destroy the TextLayer
  text_layer_destroy(first_operand_text_layer);
  bitmap_layer_destroy(operation_bitmap_layer);
  text_layer_destroy(second_operand_text_layer);
  action_bar_layer_destroy(action_bar_layer);
  
  // Dealloc resources
  free(first_operand_string);
  free(second_operand_string);
  gbitmap_destroy(operation_bitmap);
  gbitmap_destroy(up_bitmap);
  gbitmap_destroy(select_bitmap);
  gbitmap_destroy(down_bitmap);
  
  // Destroy the Window
  window_destroy(window);
}

void main_window_push() {
  
  if(!window) {
    
    // Create a window and get information about the window
    window = window_create();
    
    // Handlers for various events happening to the window
    window_set_window_handlers(window, (WindowHandlers) {
      
      // Setup layout
      .load = window_load,
      
      // Restore interface
      .appear = window_appear,
      
      // Release resources
      .unload = window_unload,
    });
  }
  
  // Push window to screen
  window_stack_push(window, true);

}