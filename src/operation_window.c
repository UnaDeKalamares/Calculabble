#include "operation_window.h"

static Window *window;

// Called on .load handler event, init layout
void window_load() {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
}

// Called on .unload handler event, destroy layout
static void window_unload() {  
  // Destroy the Window
  window_destroy(window);
  window = NULL;
}

void operation_window_push() {
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
}