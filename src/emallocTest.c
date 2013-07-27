#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "emalloc.h"

#define BUFF_SIZE 5000

#define MY_UUID { 0x0B, 0xC3, 0x7D, 0xBB, 0xDC, 0x6E, 0x4C, 0x19, 0x93, 0x70, 0x25, 0x87, 0xF1, 0x38, 0x32, 0x5F }
PBL_APP_INFO(MY_UUID,
			 "Template App", "Your Company",
			 1, 0, /* App version */
			 DEFAULT_MENU_ICON,
			 APP_INFO_STANDARD_APP);

Window window;

struct emallocBufferInfo emb;

char buffer[BUFF_SIZE];

void handle_init(AppContextRef ctx)
{
	window_init(&window, "Window Name");

	setupEmallocBufferInfo(&emb, buffer, BUFF_SIZE);
	char* ptr = emalloc(emb, 500);
	char* ptr2 = ecalloc(emb, 1, 500);
	ptr2 = erealloc(emb, ptr2, 600);
	efree(emb, ptr);
	efree(emb, ptr2);

	window_stack_push(&window, true);
}


void pbl_main(void *params)
{
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init
	};
	app_event_loop(params, &handlers);
}
