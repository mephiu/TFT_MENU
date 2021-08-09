#include "lcd.h"
#include "spi.h"

#define ST7735S_SLPOUT			0x11
#define ST7735S_DISPOFF			0x28
#define ST7735S_DISPON			0x29
#define ST7735S_CASET			0x2a
#define ST7735S_RASET			0x2b
#define ST7735S_RAMWR			0x2c
#define ST7735S_MADCTL			0x36
#define ST7735S_COLMOD			0x3a
#define ST7735S_FRMCTR1			0xb1	//framerate in normal mode
#define ST7735S_FRMCTR2			0xb2	//framerate in idle mode
#define ST7735S_FRMCTR3			0xb3	//framerate in partial mode(?) - read the docs ;)
#define ST7735S_INVCTR			0xb4	//display inversion
#define ST7735S_PWCTR1			0xc0	//power control, normal mode
#define ST7735S_PWCTR2			0xc1	//etc
#define ST7735S_PWCTR3			0xc2
#define ST7735S_PWCTR4			0xc3
#define ST7735S_PWCTR5			0xc4
#define ST7735S_VMCTR1			0xc5
#define ST7735S_GAMCTRP1		0xe0	//gamma adjustment
#define ST7735S_GAMCTRN1		0xe1	//same, but negative

#define CMD(x)			((x) | 0x100)

#define LCD_OFFSET_X  0
#define LCD_OFFSET_Y  0

static uint16_t frame_buffer[LCD_WIDTH * LCD_HEIGHT];


static void lcd_cmd(uint8_t cmd) //send any command to st7735
{
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static void lcd_data(uint8_t data)//send data(value for command) to st7735, spi always uses 8bit data
{
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static void lcd_data16(uint16_t value)//function that allows to send 16bit data as 2x 8bit
{
	lcd_data(value >> 8);
	lcd_data(value);
}

static void lcd_set_window(int x, int y, int width, int height) {
	lcd_cmd(ST7735S_CASET);	//command to set window columns
	lcd_data16(LCD_OFFSET_X + x);	//start col
	lcd_data16(LCD_OFFSET_X + x + width - 1);	//end col

	lcd_cmd(ST7735S_RASET);	//command to set window rows
	lcd_data16(LCD_OFFSET_Y + y);	//start row
	lcd_data16(LCD_OFFSET_Y + y + height - 1);	//end row
}

static void lcd_send(uint16_t value) {
	if (value & 0x100) { //checks if value is a command (9th bit is 1) or data (9th bit is 0)
		lcd_cmd(value);
	} else {
		lcd_data(value);
	}
}

static const uint16_t init_table[] = { CMD(ST7735S_FRMCTR1), 0x01, 0x2c, 0x2d,
		CMD(ST7735S_FRMCTR2), 0x01, 0x2c, 0x2d, CMD(ST7735S_FRMCTR3), 0x01,
		0x2c, 0x2d, 0x01, 0x2c, 0x2d, CMD(ST7735S_INVCTR), 0x07, CMD(
				ST7735S_PWCTR1), 0xa2, 0x02, 0x84, CMD(ST7735S_PWCTR2), 0xc5,
		CMD(ST7735S_PWCTR3), 0x0a, 0x00, CMD(ST7735S_PWCTR4), 0x8a, 0x2a, CMD(
				ST7735S_PWCTR5), 0x8a, 0xee, CMD(ST7735S_VMCTR1), 0x0e, CMD(
				ST7735S_GAMCTRP1), 0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20,
		0x22, 0x1f, 0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10, CMD(
				ST7735S_GAMCTRN1), 0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29,
		0x2e, 0x30, 0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10, CMD(0xf0), 0x01,
		CMD(0xf6), 0x00, CMD(ST7735S_COLMOD), 0x05, CMD(ST7735S_MADCTL), 0xa0, };

void lcd_init(void) {
	int i;

	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

	for (i = 0; i < sizeof(init_table) / sizeof(uint16_t); i++) {
		lcd_send(init_table[i]);
	}

	HAL_Delay(200);

	lcd_cmd(ST7735S_SLPOUT);
	HAL_Delay(120);

	lcd_cmd(ST7735S_DISPON);
}


void lcd_put_pixel(int x, int y, uint16_t color) {
	frame_buffer[x + y * LCD_WIDTH] = color;
}


void lcd_copy(void) {
	lcd_set_window(0, 0, LCD_WIDTH, LCD_HEIGHT);

	lcd_cmd(ST7735S_RAMWR);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	HAL_SPI_Transmit(&hspi2, (uint8_t *)frame_buffer, sizeof(frame_buffer), HAL_MAX_DELAY);
	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)frame_buffer, sizeof(frame_buffer));
}


void lcd_transfer_done(void){
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}


bool lcd_is_busy(void){
	if (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY){
		return true;
	}
	else{
		return false;
	}
}
