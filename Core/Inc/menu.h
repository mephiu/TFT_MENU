#pragma once

#include <stdio.h>

#define MAX_MAIN_MENU_INDEX	5
#define MAX_MISC_MENU_INDEX	5


void show_main_menu();
void show_sensor_data(uint16_t sensor_data[], uint8_t activeChannels);
void show_misc_menu(void);

void set_channels_value(uint8_t activeChannels, uint16_t color);
void set_oversampling_prescaler(uint8_t oversamplingPrescaler, uint16_t color);
void set_datetime_screen(uint8_t activeSymbol, uint8_t* datetime);
void show_sd_card_info();

void select_item(int previousItemIndex, int currentItemIndex);

