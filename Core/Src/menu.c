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
#include <stdio.h>

char text_to_parse[16];
static uint16_t ch_buffer[4];
static uint16_t os_buffer[4];
static uint16_t datetime_buffer[20];
int status = 0;

void show_main_menu() {
	hagl_clear_screen();

	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(255, 0, 0));
	}
	hagl_put_text("USTAWIENIA", 10, 10, rgb565(255, 0, 0), font6x9);
	hagl_put_text("Ilosc kanalow:", 10, 30, rgb565(0, 102, 204), font5x7);
	hagl_put_text("Oversampling:", 10, 50, rgb565(0, 102, 204), font5x7);
	hagl_put_text("Kalibracja", 10, 70, rgb565(0, 102, 204), font5x7);
	hagl_put_text("Inne", 10, 90, rgb565(0, 102, 204), font5x7);
	hagl_put_text("Powrot", 10, 110, rgb565(102, 255, 102), font5x7);
	lcd_copy();
}

void show_misc_menu() {
	hagl_clear_screen();

	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(96, 195, 10));
	}
	hagl_put_text("INNE", 10, 10, rgb565(255, 0, 0), font6x9);
	hagl_put_text("Data/Godzina", 10, 30, rgb565(0, 102, 204), font5x7);
	hagl_put_text("Status karty SD", 10, 50, rgb565(0, 102, 204), font5x7);
	hagl_put_text("Powrot", 10, 70, rgb565(102, 255, 102), font5x7);
	lcd_copy();
}

void set_channels_value(uint8_t activeChannels, uint16_t color) {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(255, 0, 0));
	}
	hagl_put_text("AKTYWNE KANALY", 20, 10, rgb565(255, 0, 0), font6x9);
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
	hagl_put_text("OVERSAMPLING", 30, 10, rgb565(255, 0, 0), font6x9);
	hagl_put_text("Wybierz z ilu probek ", 15, 30, rgb565(0, 102, 204),
			font5x7);
	hagl_put_text("  usredniac pomiar", 15, 40, rgb565(0, 102, 204), font5x7);
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
	hagl_put_text("DANE Z CZUJNIKOW", 10, 10, rgb565(15, 127, 225), font6x9);
	hagl_fill_rectangle(7, 27, LCD_WIDTH - 7, LCD_HEIGHT - 7, rgb565(0, 0, 0));
	for (int var = 0; var < activeChannels; ++var) {
		snprintf(text_to_parse, 16, "Kanal %u: \t\t%u", var, sensor_data[var]);
		hagl_put_text(text_to_parse, 10, 30 + var * 15, rgb565(15, 127, 225),
				font5x7);
	}
	lcd_copy();
}

void set_datetime_screen(uint8_t activeSymbol, uint8_t* datetime) {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, 5 - i,
				rgb565(255, 0, 0));
	}
	hagl_put_text("Data/Godzina", 30, 10, rgb565(255, 0, 0), font6x9);
	hagl_put_text("Wybierz aktualna date i godzine ", 15, 30,
			rgb565(0, 102, 204), font5x7);

	snprintf((char*) datetime_buffer, 20, "%u%u%u%u-%u%u-%u%u %u%u:%u%u",
			datetime[0], datetime[1], datetime[2], datetime[3], datetime[4],
			datetime[5], datetime[6], datetime[7], datetime[8], datetime[9],
			datetime[10], datetime[11]);
	hagl_put_text((char*) os_buffer, 10, 70, rgb565(0, 102, 204), font6x9);
	HAL_Delay(50);
	hagl_put_text((char*) os_buffer, 10, 70, rgb565(0, 204, 102), font6x9);
	HAL_Delay(50);

	lcd_copy();
}

void select_item(int index) {
	hagl_draw_rectangle(8, 40 + 20 * index, 120, 25 + 20 * index,
			rgb565(255, 255, 0));
}

void deselect_item(int index) {
	hagl_draw_rectangle(8, 40 + 20 * index, 120, 25 + 20 * index,
			rgb565(0, 0, 0));
}

