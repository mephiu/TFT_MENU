/*
 * menu.c
 *
 *  Created on: Aug 9, 2021
 *      Author: Adrian
 */
#include "lcd.h"

void show_main_menu(int border_width, uint16_t color){
	for (int i = 0; i < border_width; ++i) {
		hagl_draw_rectangle(0, 0, 160 - i, 128 - i, color);
	}

}
