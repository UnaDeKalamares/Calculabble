#include "operation_window.h"
#include "main_window.h"
#include "utils.h"
#include "localize.h"

static Window *window;

static MenuLayer *operations_menu_layer;

static int window_height;

// Implement menu layer num rows callback
static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NUM_ROWS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  bool highlighted = menu_cell_layer_is_highlighted(cell_layer);
  switch(cell_index->row) {
    case 0:
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Addition"), NULL, gbitmap_create_with_resource(RESOURCE_ID_ADDITION));
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Addition"), NULL, gbitmap_create_with_resource(RESOURCE_ID_ADDITION_BLACK));
      }
      break;
    case 1:
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Subtraction"), NULL, gbitmap_create_with_resource(RESOURCE_ID_SUBTRACTION));
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Subtraction"), NULL, gbitmap_create_with_resource(RESOURCE_ID_SUBTRACTION_BLACK));
      }
      break;
    case 2:
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Mutiplication"), NULL, gbitmap_create_with_resource(RESOURCE_ID_MULTIPLICATION));
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Mutiplication"), NULL, gbitmap_create_with_resource(RESOURCE_ID_MULTIPLICATION_BLACK));
      }
      break;
    case 3:
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Division"), NULL, gbitmap_create_with_resource(RESOURCE_ID_DIVISION));
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Division"), NULL, gbitmap_create_with_resource(RESOURCE_ID_DIVISION_BLACK));
      }
      break;
  }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return window_height / NUM_ROWS;
}

// Called on .unload handler event, destroy layout
static void window_unload() {
  // Destroy layers
  menu_layer_destroy(operations_menu_layer);
  
  // Destroy the Window
  window_destroy(window);
  window = NULL;
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    // Addition
    case 0:
      operation_enum = Addition;
      break;
    // Subtraction
    case 1:
      operation_enum = Subtraction;
      break;
    // Multiplication
    case 2:
      operation_enum = Multiplication;
      break;
    // Division
    default:
      operation_enum = Division;
      break; 
  }
  first_operand = false;
  window_stack_pop(true);
}

// Called on .load handler event, init layout
static void window_load() {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  window_height = bounds.size.h;
  
  // Create operations menu layer
  operations_menu_layer = menu_layer_create(bounds);
  // Config current window as button click interactor
  menu_layer_set_click_config_onto_window(operations_menu_layer, window);
  menu_layer_pad_bottom_enable(operations_menu_layer, false);
  
  #ifdef PBL_COLOR
    menu_layer_set_highlight_colors (operations_menu_layer, GColorVividCerulean, GColorWhite);
  #endif
  
  // Define menu callbacks
  menu_layer_set_callbacks(operations_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(operations_menu_layer));
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