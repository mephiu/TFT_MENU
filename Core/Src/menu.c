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

void show_main_menu(int border_width, uint16_t color){
	for (int i = 0; i < border_width; ++i) {
		hagl_draw_rounded_rectangle(i, i, LCD_WIDTH - i, LCD_HEIGHT - i, border_width - i, color);
	}
	hagl_put_text(L"USTAWIENIA", 32, 10, rgb565(color, 0, 0), font6x9);
	hagl_put_text(L"Ilość kanałów:", 10, 30, rgb565(0, 102, 204), font5x7);
	hagl_put_text(L"Oversampling:", 10, 50, rgb565(0, 102, 204), font5x7);
	hagl_put_text(L"Kalibracja", 10, 70, rgb565(0, 102, 204), font5x7);
	hagl_put_text(L"Inne", 10, 90, rgb565(0, 102, 204), font5x7);
	hagl_put_text(L"Powrót", 10, 110, rgb565(102, 255, 102), font5x7);
}

void select_item(int number){
	hagl_draw_rectangle(8, 40 + 20*number, 120, 25 + 20*number, rgb565(255, 255, 0));
}

