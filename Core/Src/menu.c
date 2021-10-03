/*
 * menu.c
 *
 *  Created on: Aug 9, 2021
 *      Author: Adrian
 */
#include "lcd.h"
#include "hagl.h"
#include "rgb565.h"
#include "font6x9.h"
#include "font5x7.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "menu.h"

char text_to_parse[16];
extern uint8_t mounted;
static uint16_t ch_buffer[4];
static uint16_t os_buffer[4];
static uint16_t datetime_buffer[20];
static char activeSymbolBuffer[2];
const static char *main_menu_items[] = {"SETTINGS", "Active channels", "Oversampling", "Calibration", "Misc", "Return"};
const static char *misc_menu_items[] = {"MISC", "Date/Hour", "SD card info", "Mount SD card", "Unmount SD card", "Return"};

void show_main_menu() {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(255, 0, 0));
	}
	hagl_put_text(main_menu_items[0], 10, 10, rgb565(255, 0, 0), font6x9);
	for (int var = 1; var < MAX_MAIN_MENU_INDEX+1; ++var) {
		hagl_put_text(main_menu_items[var], 10, 10 + 20*var, rgb565(0, 102, 204), font5x7);
	}
	lcd_copy();
}

void show_misc_menu() {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(96, 195, 10));
	}
	hagl_put_text(misc_menu_items[0], 10, 10, rgb565(255, 0, 0), font6x9);
		for (int var = 1; var < MAX_MAIN_MENU_INDEX+1; ++var) {
			hagl_put_text(misc_menu_items[var], 10, 10 + 20*var, rgb565(0, 102, 204), font5x7);
		}
	lcd_copy();
}

void set_channels_value(uint8_t activeChannels, uint16_t color) {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(255, 0, 0));
	}
	hagl_put_text(main_menu_items[1], 20, 10, rgb565(255, 0, 0), font6x9);
	snprintf((char*) ch_buffer, 4, "%u", activeChannels);
	hagl_put_text((char*) ch_buffer, 60, 50, color, font6x9);
	lcd_copy();
}

void set_oversampling_prescaler(uint8_t oversamplingPrescaler, uint16_t color) {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(255, 0, 0));
	}
	hagl_put_text(main_menu_items[2], 25, 10, rgb565(255, 0, 0), font6x9);
	hagl_put_text(" How many samples", 20, 30, rgb565(0, 102, 204),
			font5x7);
	hagl_put_text("should be averaged", 20, 40, rgb565(0, 102, 204), font5x7);
	snprintf((char*) os_buffer, 4, "%u", oversamplingPrescaler);
	hagl_put_text((char*) os_buffer, 60, 70, color, font6x9);
	lcd_copy();
}

void show_sensor_data(uint16_t sensor_data[], uint8_t activeChannels) {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(0, 51, 102));
	}
	hagl_put_text("SENSORS DATA", 10, 10, rgb565(15, 127, 225), font6x9);
	hagl_fill_rectangle(7, 27, LCD_WIDTH - 7, LCD_HEIGHT - 7, rgb565(0, 0, 0));
	for (int var = 0; var < activeChannels; ++var) {
		snprintf(text_to_parse, 16, "Ch %u: \t\t%u", var, sensor_data[var]);
		hagl_put_text(text_to_parse, 10, 30 + var * 15, rgb565(15, 127, 225),
				font5x7);
	}
	lcd_copy();
}

void set_datetime_screen(uint8_t activeSymbol, uint8_t* datetime) {
	uint8_t spacer = 0; //to skip special characters while highlightning changeable

	//***SCREEN CONST*****
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(255, 0, 0));
	}
	hagl_put_text(misc_menu_items[1], 30, 10, rgb565(255, 0, 0), font6x9);
	hagl_put_text("Choose current datetime", 7, 30,
			rgb565(0, 102, 204), font5x7);
	//***SCREEN CONST*****
	snprintf((char*) datetime_buffer, 20, "%u%u%u%u-%u%u-%u%u %u%u:%u%u",
			datetime[0], datetime[1], datetime[2], datetime[3], datetime[4],
			datetime[5], datetime[6], datetime[7], datetime[8], datetime[9],
			datetime[10], datetime[11]);
	hagl_put_text((char*) datetime_buffer, 20, 70, rgb565(0, 102, 204), font6x9);
	//***HIGHLIGHTNING ACTIVE CHARACTER***
	snprintf(activeSymbolBuffer, 2, "%u", datetime[activeSymbol]);	//buffer for highlighted char
	if (activeSymbol >3 && activeSymbol <= 5) spacer = 1;
	else if (activeSymbol >5 && activeSymbol <= 7) spacer = 2;
	else if (activeSymbol >7 && activeSymbol <= 9) spacer = 3;
	else if (activeSymbol >9 ) spacer = 4;
	hagl_put_text(activeSymbolBuffer, 20+(6*(activeSymbol+spacer)), 70, rgb565(200, 200, 200), font6x9);
	lcd_copy();
}

void select_item(int previousItemIndex, int currentItemIndex) {
	hagl_draw_rectangle(8, 40 + 20 * previousItemIndex, 120, 25 + 20 * previousItemIndex,
			rgb565(0, 0, 0));
	hagl_draw_rectangle(8, 40 + 20 * currentItemIndex, 120, 25 + 20 * currentItemIndex,
			rgb565(255, 255, 0));
}


void show_sd_card_info(){
	static char card_capacity[40];
	if (mounted) strcpy(card_capacity, sd_card_check_capacity());
	else snprintf(card_capacity, 30, "SD CARD NOT MOUNTED!");
	//***SCREEN CONST*****
		hagl_clear_screen();
		for (int i = 0; i < 5; ++i) {
			hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
					rgb565(255, 0, 0));
		}
		hagl_put_text(misc_menu_items[2], 30, 10, rgb565(255, 0, 0), font6x9);
		hagl_put_text(card_capacity, 7, 30,
				rgb565(0, 102, 204), font5x7);
	//***SCREEN CONST*****
}
