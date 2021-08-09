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

void show_main_menu(int border_width, uint16_t color){
	for (int i = 0; i < border_width; ++i) {
		hagl_draw_rounded_rectangle(i, i, 128 - i, 160 - i, border_width - i, color);
	}
	hagl_put_text(L"USTAWIENIA", 40, 30, rgb565(color, 0, 0), font6x9);
}
