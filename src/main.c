#include <pebble.h>
#include <main_window.h>
#include "localize.h"

static void init(void) {
  // Init locale framework
  locale_init();
  main_window_push();
}

static void deinit(void) {
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
