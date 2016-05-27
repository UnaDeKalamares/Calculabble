#include "main_window.h"
#include "utils.h"

static Window *window;

static TextLayer *first_operand_text_layer;
static char *first_operand_string;

static TextLayer *operation_text_layer;
static char operation_string[2];

static TextLayer *second_operand_text_layer;
static char *second_operand_string;

static bool first_operand;
static int current_value = 0;

static ActionBarLayer *action_bar_layer;

// Declare method prototypes
void click_config_provider(void *context);
void increase_value_click_handler(ClickRecognizerRef recognizer, void *context);
void add_figure_click_handler(ClickRecognizerRef recognizer, void *context);
void remove_figure_click_handler(ClickRecognizerRef recognizer, void *context);
void set_text_current_operand();

// Called on .load handler event, init layout
static void window_load() { 
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
    
  // Create a text layer and set the text
	first_operand_text_layer = text_layer_create((GRect) {
    .origin = {0, 0},
    .size = {bounds.size.w - ACTION_BAR_WIDTH, bounds.size.h / 3}
  });
	text_layer_set_text(first_operand_text_layer, first_operand_string);
    
  // Set the font and text alignment
	text_layer_set_font(first_operand_text_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
	text_layer_set_text_alignment(first_operand_text_layer, GTextAlignmentCenter);

	// Add the text layer to the window
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(first_operand_text_layer));
  
  // Enable text flow and paging on the text layer, with a slight inset of 10, for round screens
  text_layer_enable_screen_text_flow_and_paging(first_operand_text_layer, 10);
    
  action_bar_layer = action_bar_layer_create();
  
  // Define action bar icons
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_UP, gbitmap_create_with_resource(RESOURCE_ID_INCREASE), true);
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_SELECT, gbitmap_create_with_resource(RESOURCE_ID_OPERATION), true);
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_DOWN, gbitmap_create_with_resource(RESOURCE_ID_ADD_FIGURE), true);
  
  // Define click_config_provider for click handling
  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);
  
  // Add the action bar to the window
  action_bar_layer_add_to_window(action_bar_layer, window);
  
  // Initialize values
  // Set first operand as current
  first_operand = true;
  
  // Allocate string resources
  first_operand_string = (char*) malloc(15 * sizeof(char));
  second_operand_string = (char*) malloc(15 * sizeof(char));  
  
  // Init first_operand_string
  first_operand_string = "0";
  text_layer_set_text(first_operand_text_layer, first_operand_string);
  
}

// Define click handlers for each button interactor
void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) increase_value_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) add_figure_click_handler);
  window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 0, 0, false, remove_figure_click_handler);
}

// Implement increase value handler
void increase_value_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_value = increase_value(current_value);
  set_text_current_operand();
}

// Implement add figure handler
void add_figure_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_value = add_figure(current_value);
  set_text_current_operand();
}

// Implement remove figure handler
void remove_figure_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_value = remove_figure(current_value);
  set_text_current_operand();
}

void set_text_current_operand() {
  if (first_operand) {
    int_to_string(current_value, first_operand_string);
    text_layer_set_text(first_operand_text_layer, first_operand_string);
  } else {
    int_to_string(current_value, second_operand_string);
    text_layer_set_text(second_operand_text_layer, second_operand_string);
  }
}

// Called on .unload handler event, destroy layout
static void window_unload() {
  // Destroy the TextLayer
  text_layer_destroy(first_operand_text_layer);
  
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
      
      // Release resources
      .unload = window_unload,
    });
  }
  
  // Push window to screen
  window_stack_push(window, true);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Just pushed a window!");
}