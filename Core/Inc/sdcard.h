/*
 * sdcard.h
 *
 *  Created on: 2 paź 2021
 *      Author: Adrian
 */
#pragma once

int bufsize(char *buf);
void clear_buffer(void);
void sd_card_mount(void);
void sd_card_unmount(void);
char* sd_card_check_capacity(void);
void sd_card_basic_write_to_file(char *filename);
void sd_card_basic_read_file(void);
void sd_card_write_to_file(char *filename);
void sd_card_read_file(char *filename);
void sd_card_update_file(char *filename, uint16_t *measurements);
void sd_card_read_from_file(char *filename);
void sd_card_remove_file(char *filename);
