#include "operation_window.h"
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

static MenuLayer *operations_menu_layer;

static GBitmap *addition_bitmap;
static GBitmap *addition_bitmap_black;
static GBitmap *subtraction_bitmap;
static GBitmap *subtraction_bitmap_black;
static GBitmap *multiplication_bitmap;
static GBitmap *multiplication_bitmap_black;
static GBitmap *division_bitmap;
static GBitmap *division_bitmap_black;
static GBitmap *squared_bitmap;
static GBitmap *squared_bitmap_black;
static GBitmap *root_bitmap;
static GBitmap *root_bitmap_black;


static int window_height;

// Implement menu layer num rows callback
static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NUM_ROWS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  bool highlighted = menu_cell_layer_is_highlighted(cell_layer);
  switch(cell_index->row) {
    // Allocate each row's bitmap depending if the row is highlighted
    // First, dealloc if it isn't null
    case 0:
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Addition"), NULL, addition_bitmap);
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Addition"), NULL, addition_bitmap_black);
      }
      break;
    case 1:
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Subtraction"), NULL, subtraction_bitmap);
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Subtraction"), NULL, subtraction_bitmap_black);
      }
      break;
    case 2:    
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Mutiplication"), NULL, multiplication_bitmap);
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Mutiplication"), NULL, multiplication_bitmap_black);        
      }
      break;
    case 3:    
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Division"), NULL, division_bitmap);
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Division"), NULL, division_bitmap_black);
      }
      break;
    case 4:
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Squared"), NULL, squared_bitmap);
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Squared"), NULL, squared_bitmap_black);
      }
      break;
    case 5:
      if (highlighted) {
        menu_cell_basic_draw(ctx, cell_layer, _("Root"), NULL, root_bitmap);
      } else {
        menu_cell_basic_draw(ctx, cell_layer, _("Root"), NULL, root_bitmap_black);
      }
  }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return window_height / 4;
}

// Called on .unload handler event, destroy layout
static void window_unload() {
  // Destroy layers
  menu_layer_destroy(operations_menu_layer);
  
  // Destroy bitmaps
  gbitmap_destroy(addition_bitmap);
  gbitmap_destroy(subtraction_bitmap);
  gbitmap_destroy(multiplication_bitmap);
  gbitmap_destroy(division_bitmap);
  gbitmap_destroy(squared_bitmap);
  gbitmap_destroy(root_bitmap);
  
  gbitmap_destroy(addition_bitmap_black);
  gbitmap_destroy(subtraction_bitmap_black);
  gbitmap_destroy(multiplication_bitmap_black);
  gbitmap_destroy(division_bitmap_black);
  gbitmap_destroy(squared_bitmap_black);
  gbitmap_destroy(root_bitmap_black);  
  
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
    case 3:
      operation_enum = Division;
      break;
    // Squared
    case 4:
      operation_enum = Squared;
      break; 
    // Root
    default:
      operation_enum = Root;
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
  
  addition_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ADDITION);
  addition_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_ADDITION_BLACK);
  
  subtraction_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SUBTRACTION);  
  subtraction_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_SUBTRACTION_BLACK);
  
  multiplication_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MULTIPLICATION);
  multiplication_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_MULTIPLICATION_BLACK);
  
  division_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DIVISION);
  division_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_DIVISION_BLACK);   
  
  squared_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SQUARED);
  squared_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_SQUARED_BLACK);
  
  root_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ROOT);
  root_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_ROOT_BLACK);
  
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