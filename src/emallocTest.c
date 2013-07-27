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
TextLayer tl1;
TextLayer tl2;
TextLayer dbg;

struct emallocBufferInfo emb;
char buffer[BUFF_SIZE];
char dbgMsg[500];

void dump_up(ClickRecognizerRef recognizer, Window *window) {
	(void)recognizer;
	(void)window;
	
	char* ptr = emalloc(emb, 30);
	for (int i = 0; i < 29; ++i)
		if(ptr[i] == 0)
			ptr[i] = 'z';
	ptr[29] = 0;
	text_layer_set_text(&tl1, ptr);

	snprintf(dbgMsg, 500, "%p\n%p", buffer, ptr);
	text_layer_set_text(&dbg, dbgMsg);
}

void click_config_provider(ClickConfig **config, Window *window) {
	(void)window;

	config[BUTTON_ID_UP]->click.handler = (ClickHandler) dump_up;
}

void handle_init(AppContextRef ctx)
{
	window_init(&window, "Window Name");

	setupEmallocBufferInfo(&emb, buffer, BUFF_SIZE);
	char* ptr1 = emalloc(emb, 5);
	for (int i = 0; i < 4; ++i)
		ptr1[i] = 'a' + i;
	ptr1[4] = 0;

	char* ptr2 = ecalloc(emb, 1, 5);
	for (int i = 0; i < 4; ++i)
		ptr2[i] = 'a' + i;
	ptr2[4] = 0;

	char* ptr3 = erealloc(emb, ptr1, 10);
	for (int i = 0; i < 9; ++i)
		ptr3[i] = 'a' + i;
	ptr3[9] = 0;

	text_layer_init(&tl1, GRect(0,0,144,30));
	text_layer_set_text(&tl1, ptr3);
	layer_add_child(&window.layer, &tl1.layer);

	text_layer_init(&tl2, GRect(0,30,144,30));
	text_layer_set_text(&tl2, ptr2);
	layer_add_child(&window.layer, &tl2.layer);

	snprintf(dbgMsg, 500, "%p\n%p\n%p\n%p", buffer, ptr1, ptr2, ptr3);

	text_layer_init(&dbg, GRect(0,60,144,168));
	text_layer_set_text(&dbg, dbgMsg);
	layer_add_child(&window.layer, &dbg.layer);

	window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);

	efree(emb, ptr3);
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
