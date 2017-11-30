#ifndef LED_H
#define LED_H

#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2c-dev.h"
#include <fcntl.h>
#include "8x8font.h"
#include <string.h>

__u16 block[I2C_SMBUS_BLOCK_MAX];
//global variables used for matrix
int res, i2cbus, daddress, address, size, file;
uint16_t   bitShifted[8];
uint16_t   displayBuffer[8][15*8];
uint16_t displayBuffer2[8];
	int length;
        int letter=0;
        int y=0;
//Reverse the bits
unsigned  char  reverseBits(unsigned  char num)
{
    unsigned  char count = sizeof(num) * 8 - 1;
    unsigned  char reverse_num = num;
    num >>= 1;
    while(num)
    {
       reverse_num <<= 1;
       reverse_num |= num & 1;
       num >>= 1;
       count--;
    }
    reverse_num <<= count;
    return reverse_num;
}



int displayImage(__u16 bmp[], int res, int daddress, int file)
{
        int i;
        for(i=0; i<8; i++)
        {
             block[i] = (bmp[i]&0xfe) >>1 |
             (bmp[i]&0x01) << 7;
        }
        res = i2c_smbus_write_i2c_block_data(file, daddress, 16,
                (__u8 *)block);
}
void ledScroll(uint16_t  dBuffer[8][15*8],int length, int& letter,int& y)
{
			if(letter<(length-1))
			{
				if(y<8)
				{
    				for(int i=0; i<8; i++)
    				{
					bitShifted[i] = (dBuffer[i][letter]) << y | (dBuffer[i][letter+1]) >> (8-y);
					bitShifted[i]  = reverseBits(bitShifted[i]);
					}
		        	displayImage(bitShifted,res, daddress, file);
	    			y++;
				}
				else
				{
					letter++;
					y=0;
				}
					
					
			}
			else
			{
				letter=0;
					
			}
}

void ledDraw(uint16_t  dBuffer[8],const char *text)
{
	int Vposition,character;
	character=text[0]-31;

	for(Vposition = 0; Vposition < 8 ; Vposition++)
	{
		dBuffer[Vposition]=FONT8x8[character][Vposition];
		//printf("haha %i,%i= %u\r\n",Vposition,i,displayBuffer[Vposition][i]);
			
	}
	displayImage(dBuffer,res, daddress, file);
}

void setBuffer(const char *text,uint16_t (*dBuffer)[15*8],int& length)
{
	length = (strlen(text))-2;
	int Vposition,c,character, l;
	
	
	for(int i = 0; i < length ; i++)
	{
		character = (text[i]-31);
		 for(Vposition = 0; Vposition < 8 ; Vposition++)
		 {
			dBuffer[Vposition][i]=FONT8x8[character][Vposition];
			//printf("haha %i,%i= %u\r\n",Vposition,i,displayBuffer[Vposition][i]);
			
		}
	}
	
	
}

void ledinit()
{
		i2cbus   = 1;
        address  = 112;
        daddress = 0;
		//Startup the matrix
	size = I2C_SMBUS_BYTE;
	file = open("/dev/i2c-1", O_RDWR);
	if (file<0) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
				"/dev/i2c-%d: %s\n", i2cbus, strerror(ENOENT));
			}
		 else {
			fprintf(stderr, "Error: Could not open file "

				"`%s': %s\n", "/dev/i2c-1", strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
		exit(1);
	}

	if (ioctl(file, I2C_SLAVE, address) < 0) {
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address, strerror(errno));
	}


	res = i2c_smbus_write_byte(file, daddress);
	if (res < 0) {
		fprintf(stderr, "Warning - write failed, filename=%s, daddress=%d\n",
			"/dev/i2c-1", daddress);
	}
	
	daddress = 0x21; // Start oscillator (page 10)
	//printf("writing: 0x%02x\n", daddress);
	res = i2c_smbus_write_byte(file, daddress);

	daddress = 0x81; // Display on, blinking off (page 11)
	//printf("writing: 0x%02x\n", daddress);
	res = i2c_smbus_write_byte(file, daddress);

	daddress = 0xef; // Full brightness (page 15)
	//printf("Full brightness writing: 0x%02x\n", daddress);
	res = i2c_smbus_write_byte(file, daddress);

	daddress = 0x00; // Start writing to address 0 (page 13)
	//printf("Start writing to address 0 writing: 0x%02x\n", daddress);
	res = i2c_smbus_write_byte(file, daddress);

}

#endif
