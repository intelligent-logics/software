// Simple program to display message and check video type
 
/* Original example provided by Doug

	Functionality provided by Beto Perez
	Last Update 3/20/24 - comments, cleaning, from source code that was executed on the MiSTer on Tuesday, 3/19/24

*/
#include "LIB/neslib.h"
#include "LIB/nesdoug.h" 

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30
// there's some oddities in the palette code, black must be 0x0f, white must be 0x30
 
 
#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
// all variables should be global for speed
// zeropage global is even faster

const unsigned char palette[]={
BLACK, DK_GY, LT_GY, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 

// My own function, a 'printf'. Puts the character's sprite on VRAM which will display on NAMETABLE which will go to screen
void put_str(unsigned int adr, const char* str){
	// Where we put the character
	vram_adr(adr);
	// \0 Terminated string
	while(1){

		if(!*str)break;
		
		vram_put((*str++));
		
	}
}


	

void main (void) {
	
	ppu_off(); // screen off

	pal_bg(palette); //	load the BG palette
	// Load pallete colors
	pal_col(1, 0x30);


	put_str(NTADR_A(2, 2), "GO GATORS!");
	put_str(NTADR_A(2, 4), "VIDEO MODE:");

	if(ppu_system()) put_str(NTADR_A(2, 5), "NTSC");
	else put_str(NTADR_A(2, 5), "PAL");

	
	// vram_adr and vram_put only work with screen off
	// NOTE, you could replace everything between i = 0; and here with...
	// vram_write(text,sizeof(text));
	// does the same thing
	
	ppu_on_all(); //	turn on screen
	// Infinite loop for now
	while (1);
}
	
	