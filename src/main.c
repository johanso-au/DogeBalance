#include <pebble.h>
 
Window* window;
TextLayer *balance_layer;

static GBitmap *background_img;
static BitmapLayer* bg_layer;

enum {
  DOGE_BALANCE, 
};

static void out_sent_handler(DictionaryIterator *sent, void *context) {
	// outgoing message was delivered
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to send AppMessage to Pebble");
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	Tuple *balance_tuple = dict_find(iter, DOGE_BALANCE);
	if (balance_tuple) {
		text_layer_set_text(balance_layer, balance_tuple->value->cstring);
	}
}

void in_dropped_handler(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "incoming message from Pebble dropped");
}

void update() {
	app_message_outbox_send();	
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
		psleep(500);
		update();
}


void window_load(Window *window)
{
        //Background Layer
		background_img = gbitmap_create_with_resource(RESOURCE_ID_QR_CODE);
        bg_layer = bitmap_layer_create(GRect(0, 0, 144, 144));
        bitmap_layer_set_background_color(bg_layer, GColorBlack);
        bitmap_layer_set_bitmap(bg_layer, background_img);
        layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bg_layer));
				
		// Doge Balance Layer
		balance_layer = text_layer_create(GRect(0, 132, 144, 30));
		text_layer_set_text_color(balance_layer, GColorBlack);
		text_layer_set_background_color(balance_layer, GColorWhite);
		text_layer_set_font(balance_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
		text_layer_set_text_alignment(balance_layer, GTextAlignmentCenter);
		text_layer_set_text(balance_layer, "Loading...");
                
		layer_add_child(window_get_root_layer(window), (Layer*) balance_layer);
		
}
 
void window_unload(Window *window)
{
        //We will safely destroy the Window's elements here!
        text_layer_destroy(balance_layer);
        bitmap_layer_destroy(bg_layer);
}
 
static void app_message_init(void) {
	app_message_open(64 /* inbound_size */, 2 /* outbound_size */);
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);
}

void init()
{
        app_message_init();
	
		//Initialize the app elements here!
        window = window_create();
        window_set_window_handlers(window, (WindowHandlers) {
                .load = window_load,
                .unload = window_unload,
        });
        
        tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
        
		window_stack_push(window, true);
}
 
void deinit()
{
        //De-initialize elements here to save memory!
        tick_timer_service_unsubscribe();
        
        window_destroy(window);
}
 
int main(void)
{
        init();
        app_event_loop();
        deinit();
}