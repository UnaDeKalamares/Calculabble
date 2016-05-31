#include "main_window.h"
#include "utils.h"
#include "localize.h"
  
static Window *window;
static GRect window_bounds;

static Layer *divider_layer;

static TextLayer *first_operand_text_layer;
static char *first_operand_string;

static BitmapLayer *operation_bitmap_layer;

static TextLayer *second_operand_text_layer;
static char *second_operand_string;

static int current_value = 0;

static int left_margin = 0;

static ActionBarLayer *action_bar_layer;

// Declare method prototypes
void operation_click_config_provider(void *context);
void result_click_config_provider(void *context);
void increase_value_click_handler(ClickRecognizerRef recognizer, void *context);
void add_figure_click_handler(ClickRecognizerRef recognizer, void *context);
void remove_figure_click_handler(ClickRecognizerRef recognizer, void *context);
void select_operation_click_handler(ClickRecognizerRef recognizer, void *context);
void result_click_handler(ClickRecognizerRef recognizer, void *context);
void set_text_current_operand();

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
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_UP, gbitmap_create_with_resource(RESOURCE_ID_INCREASE), true);
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_SELECT, gbitmap_create_with_resource(RESOURCE_ID_OPERATION), true);
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_DOWN, gbitmap_create_with_resource(RESOURCE_ID_ADD_FIGURE), true);
  
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
  int_to_string(current_value, first_operand_string);
  text_layer_set_text(first_operand_text_layer, first_operand_string);
  
}

// Define click handlers for each button interactor (when introducing first operand)
void operation_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) increase_value_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) add_figure_click_handler);
  window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 0, 0, false, (ClickHandler) remove_figure_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_operation_click_handler);
}

// Define click handlers for each button interactor (when introducing second operand)
void result_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) increase_value_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) add_figure_click_handler);
  window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 0, 0, false, (ClickHandler) remove_figure_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) result_click_handler);
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

// Implement select operand handler
void select_operation_click_handler(ClickRecognizerRef recognizer, void *context) {
  const char *first = text_layer_get_text(first_operand_text_layer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "First operand = %s", first);
  if (strcmp(first, _("Division by")) == 0 || strcmp(first, _("Result")) == 0) {
    text_layer_set_text(first_operand_text_layer, first_operand_string);
    // Reset second operand text layer
    text_layer_set_text(second_operand_text_layer, "");
  } else {
    operation_window_push();
  }
}

// Implement result handler
void result_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Get operands as ints
  int first_operand_int = atoi(first_operand_string);
  int second_operand_int = atoi(second_operand_string);
  
  // Reset window
  // Set first operand as current
  first_operand = true;
  
  // Prevent division by zero
  if (second_operand_int == 0 && operation_enum == Division) {
    
    text_layer_set_text(first_operand_text_layer, _("Division by"));
    text_layer_set_text(second_operand_text_layer, _("zero!"));
    // Restore default value for first operand
    int_to_string(0, first_operand_string);
    
  } else {
    
    // Perform calculation
    current_value = get_result(first_operand_int, operation_enum, second_operand_int);
        
    // Ignore result if it's too big 
    if ( current_value > MAX_VALUE || current_value < MIN_VALUE) {
     
      text_layer_set_text(first_operand_text_layer, _("Result"));
      text_layer_set_text(second_operand_text_layer, _("too big!"));
      // Restore default value for first operand
      int_to_string(0, first_operand_string);
     
    } else {
     
      // Set calculation result as first operand
      set_text_current_operand();
      
      // Reset second operand text layer
      text_layer_set_text(second_operand_text_layer, "");
     
    }
    
  }
  
  // Restore current value
  current_value = 0;
  
  // Remove bitmap (necessary dealloc and alloc)
  layer_remove_from_parent(bitmap_layer_get_layer(operation_bitmap_layer));
  bitmap_layer_destroy(operation_bitmap_layer);
  init_operation_bitmap_layer();
  
  // Restore select button icon
  action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_SELECT, gbitmap_create_with_resource(RESOURCE_ID_OPERATION), true);
  
  // Restore action bar click config provider
  action_bar_layer_set_click_config_provider(action_bar_layer, operation_click_config_provider);
}

// Set current value in the correct text layer
void set_text_current_operand() {
  if (first_operand) {
    int_to_string(current_value, first_operand_string);
    text_layer_set_text(first_operand_text_layer, first_operand_string);
  } else {
    int_to_string(current_value, second_operand_string);
    text_layer_set_text(second_operand_text_layer, second_operand_string);
  }
}

// Called when window shows up
static void window_appear() {
  // If we chose an operation
  if (!first_operand) {
    
    // Init bitmap
    GBitmap *bitmap;
    switch(operation_enum) {
      case Addition:
      bitmap = gbitmap_create_with_resource(RESOURCE_ID_ADDITION);
      break;
      case Subtraction:
      bitmap = gbitmap_create_with_resource(RESOURCE_ID_SUBTRACTION);
      break;
      case Multiplication:
      bitmap = gbitmap_create_with_resource(RESOURCE_ID_MULTIPLICATION);
      break;
      default:
      bitmap = gbitmap_create_with_resource(RESOURCE_ID_DIVISION);
      break;
    }
    
    // Set operation bitmap
    bitmap_layer_set_bitmap(operation_bitmap_layer, bitmap);
    // Change bitmap layer background color to transparent
    bitmap_layer_set_background_color(operation_bitmap_layer, GColorClear);
    // Set bitmap composition to use transparent background
    bitmap_layer_set_compositing_mode(operation_bitmap_layer, GCompOpSet);
  
    // Set second operand text layer value
    current_value = 0;  
    int_to_string(current_value, second_operand_string);
    text_layer_set_text(second_operand_text_layer, second_operand_string);
    
    // Replace action bar select button icon
    action_bar_layer_set_icon_animated(action_bar_layer, BUTTON_ID_SELECT, gbitmap_create_with_resource(RESOURCE_ID_RESULT), true);
    
    // Replace action bar click config provider
    action_bar_layer_set_click_config_provider(action_bar_layer, result_click_config_provider);
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