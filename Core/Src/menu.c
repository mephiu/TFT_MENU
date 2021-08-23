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

char text_to_parse[8];

void show_menu_window() {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i,
				5 - i, rgb565(255, 0, 0));
	}
	hagl_put_text(L"USTAWIENIA", 10, 10, rgb565(255, 0, 0), font6x9);
	hagl_put_text(L"Ilość kanałów:", 10, 30, rgb565(0, 102, 204), font5x7);
	hagl_put_text(L"Oversampling:", 10, 50, rgb565(0, 102, 204), font5x7);
	hagl_put_text(L"Kalibracja", 10, 70, rgb565(0, 102, 204), font5x7);
	hagl_put_text(L"Inne", 10, 90, rgb565(0, 102, 204), font5x7);
	hagl_put_text(L"Powrót", 10, 110, rgb565(102, 255, 102), font5x7);
	lcd_copy();
}


void show_sensor_window() {
	hagl_clear_screen();
	for (int i = 0; i < 5; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i,
				5 - i, rgb565(0, 51, 102));
	}
	hagl_put_text(L"DANE Z CZUJNIKÓW", 10, 10, rgb565(15, 127, 225), font6x9);
	lcd_copy();
}


void show_sensor_data(uint16_t sensor_data[]) {
	for (int var = 0; var < 8; ++var) {
		snprintf(text_to_parse, 20, L"Kanal: %d\t\t%d", var,	sensor_data[var]);
		hagl_put_text(text_to_parse, 10, 30 + var * 15, rgb565(15, 127, 225), font5x7);
	}
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

