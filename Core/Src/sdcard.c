/*
 * sdcard.c
 *
 *  Created on: 2 paź 2021
 *      Author: Adrian
 */
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include "fatfs_sd.h"

#define BUFFER_SIZE 128

FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

char buffer[BUFFER_SIZE];  // to store uart statuses
int i = 0;

int bufsize(char *buf) {
	int i = 0;
	while (*buf++ != '\0')
		i++;
	return i;
}

void clear_buffer(void) {
	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = '\0';
}

uint8_t sd_card_mount(void) {
	fresult = f_mount(&fs, "/", 1);
	uint8_t errorCode;
	if (fresult != FR_OK){
		printf("ERROR!!! in mounting SD CARD: %d", fresult);
		errorCode = 1;
	}
	else{
		printf("SD CARD mounted successfully...\n\n");
		errorCode = 0;
	}
	return errorCode;
}

void sd_card_unmount(void) {
	/* Unmount SDCARD */
	fresult = f_mount(NULL, "/", 1);
	if (fresult == FR_OK)
		printf("SD CARD UNMOUNTED successfully...\n");
}

char* sd_card_check_capacity(void) {
	f_getfree("", &fre_clust, &pfs);
	total = (uint32_t) ((pfs->n_fatent - 2) * pfs->csize * 0.5);
	free_space = (uint32_t) (fre_clust * pfs->csize * 0.5);
	sprintf(buffer, "SD CARD Total Size: \t%lu\nSD CARD Free Space: \t%lu\n\n", total, free_space);
	printf(buffer);
	clear_buffer();
	return buffer;
}

void sd_card_basic_write_to_file(char *filename) {
	/* Open file to write/ create a file if it doesn't exist */
	fresult = f_open(&fil, filename, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

	/* Writing text */
	f_puts(
			"DateTime - Channel 0 - Channel 1 - Channel 2 - Channel 3 - Channel 4 - Channel 5 - Channel 6 - Channel 7\n",
			&fil);

	/* Close file */
	fresult = f_close(&fil);

	if (fresult == FR_OK)
		printf("READINGS.txt created and the data is written \n");

}

void sd_card_basic_read_file(void) {
	/* Open file to read */
	fresult = f_open(&fil, "READINGS.txt", FA_READ);

	/* Read string from the file */
	f_gets(buffer, f_size(&fil), &fil);

	printf("READINGS.txt is opened and it contains the data as shown below\n");
	printf(buffer);
	printf("\n\n");

	/* Close file */
	f_close(&fil);

	clear_buffer();
}

void sd_card_write_to_file(char *filename) {
	/* Create second file with read write access and open it */
	fresult = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);

	/* Writing text */
	strcpy(buffer, "This is file with measurements\n");

	fresult = f_write(&fil, buffer, bufsize(buffer), &bw);

	printf("datetime.txt created and data is written\n");

	/* Close file */
	f_close(&fil);

	// clearing buffer to show that result obtained is from the file
	clear_buffer();
}

void sd_card_read_file(char *filename) {
	/* Open second file to read */
	fresult = f_open(&fil, filename, FA_READ);
	if (fresult == FR_OK)
		printf("filename is open and the data is shown below\n");

	/* Read data from the file
	 * Please see the function details for the arguments */
	f_read(&fil, buffer, f_size(&fil), &br);
	printf(buffer);
	printf("\n\n");

	/* Close file */
	f_close(&fil);

	clear_buffer();
}

void sd_card_update_file(char *filename, uint16_t *measurements) {
	/* Open the file with write access */
	fresult = f_open(&fil, filename, FA_OPEN_EXISTING | FA_READ | FA_WRITE);

	/* Move to offset to the end of the file */
	fresult = f_lseek(&fil, f_size(&fil));

	if (fresult == FR_OK)
		printf("About to update the filename.txt\n");

	/* write the string to the file */
	snprintf(buffer, sizeof(buffer),
			"01/01/2021 - %d - %d - %d - %d - %d - %d - %d - %d\n",
			measurements[0], measurements[1], measurements[2], measurements[3],
			measurements[4], measurements[5], measurements[6], measurements[7]);
	fresult = f_puts(buffer, &fil);

	f_close(&fil);

	clear_buffer();
}

void sd_card_read_from_file(char *filename) {
	/* Open to read the file */
	fresult = f_open(&fil, filename, FA_READ);

	/* Read string from the file */
	fresult = f_read(&fil, buffer, f_size(&fil), &br);
	if (fresult == FR_OK)
		printf("Below is the data from updated file2.txt\n");
	printf(buffer);
	printf("\n\n");

	/* Close file */
	f_close(&fil);

	clear_buffer();
}

void sd_card_remove_file(char *filename) {
	fresult = f_unlink(filename);
	if (fresult == FR_OK)
		printf("filename.txt removed successfully...\n");
}
