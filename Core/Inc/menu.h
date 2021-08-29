#pragma once

#include <stdio.h>

#define MAIN_MENU_ITEMS	5
#define MISC_MENU_ITEMS	5


void show_menu_window();
void show_sensor_window();
void show_sensor_data(uint16_t sensor_data[]);
void show_misc_menu(void);
void select_item(int index);
void deselect_item(int index);
