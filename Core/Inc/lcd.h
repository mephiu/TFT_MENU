#pragma once

#include <stdint.h>
#include <stdbool.h>

#define LCD_WIDTH	128
#define LCD_HEIGHT	160

//defined colors have reversed bytes
#define BLACK     0x0000
#define RED       0x00f8
#define GREEN     0xe007
#define BLUE      0x1f00
#define YELLOW    0xe0ff
#define MAGENTA   0x1ff8
#define CYAN      0xff07
#define WHITE     0xffff

void lcd_init(void);
//void lcd_fill_box(int x, int y, int width, int height, uint16_t color);
void lcd_put_pixel(int x, int y, uint16_t color);
void lcd_draw_image(int x, int y, int width, int height, uint8_t* data);
void lcd_copy(void); //kopia zawartosci buforu
void lcd_transfer_done(void);
bool lcd_is_busy(void);
