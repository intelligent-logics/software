// Simple program to display message and check video type
 
/* Original example provided by Doug

	Functionality provided by Beto Perez
	Last Update 3/20/24 - comments, cleaning, from source code that was executed on the MiSTer on Tuesday, 3/19/24

*/
#include "LIB/neslib.h"
#include "LIB/nesdoug.h" 

// P2 register data
unsigned char* p2;

unsigned char p2val = 0x00;
unsigned char p2low = 0x00;
unsigned char p2high = 0x00;

// BG Palette
#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30

// Player 1 input
unsigned char pad1 = 0;
unsigned char pad1Next = 0;

#define MENU_SIZE 127
#define ENTRY_SIZE 16


// there's some oddities in the palette code, black must be 0x0f, white must be 0x30

const unsigned char palSprites[16] = {

	0x0f,0x15,0x26,0x11,
	0x0f,0x11,0x21,0x15,
	0x0f,0x15,0x25,0x19,
	0x0f,0x19,0x29,0x38

};

const unsigned char palette[]={
BLACK, DK_GY, LT_GY, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 



#define BALLS_MAX	8

//balls parameters, the x, y positions and speed

static unsigned char ball_x[BALLS_MAX];
static unsigned char ball_y[BALLS_MAX];
static unsigned char ball_dx[BALLS_MAX];
static unsigned char ball_dy[BALLS_MAX];

unsigned char i, j = 0;
unsigned char spr = 0;

// Menu location tracking
unsigned char menuIndexH = 0;
unsigned char menuIndexV = 0;

unsigned char page = 0;

// Menu entries
unsigned char* menupointer = (unsigned char*)0x30C;
unsigned char menu[32][ENTRY_SIZE] = {

	"S. MARIO BROS",	"S. MARIO BROS 2",	"S. MARIO BROS 3",	"DONKEY KONG",	"DONKEY KONG 2",	"MEGA MAN",			"MEGA MAN 2",		"MEGA MAN 3",
	"MEGA MAN 4", 		"MEGA MAN 5", 		"MEGA MAN 6", 		"L. OF ZELDA", 	"L. OF ZELDA 2", 	"MT'S PUNCH OUT", 	"CASTLEVANIA", 		"CASTLEVANIA 2",
	"CASTLEVANIA 3",	"METROID",			"FINAL FANTASY",	"KID ICARUS",	"BATTLETOADS",		"TETRIS",			"GHOSTS & GBLNS", 	"ICE CLIMBER",
	"MARIO BROS",		"TNMT",				"DUCK HUNT",		"CONTRA",		"CHIP N DALE",		"DR MARIO",			"KIRBY'S ADV"		

};


// Cursor data
unsigned char cursorX = 0;
unsigned char cursorY = 55;


#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
// all variables should be global for speed
// zeropage global is even faster


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

// Draw the menu entries. Can only be done when the screen is off
void draw_bg_menu(const unsigned char page){

	ppu_off();

	for(i = 0; i < 4; ++i){

		put_str(NTADR_A(1, i * 2 + 7), "                ");
		put_str(NTADR_A(17, i *2 + 7), "                ");

	}


	for(i = 0; i < 4; ++i){
		
		put_str(NTADR_A(1, i * 2 + 7), menu[(page * 8) + (i * 2)]);
		put_str(NTADR_A(17, i *2 + 7), menu[(page * 8) + (i * 2) + 1]);

	}
	ppu_on_all();	
}

// Another function, handles input and logic
void handleMenuInput(void){

	// Up the menu, only move up if we are not at the top of the menu
	if((pad1Next & PAD_UP) && menuIndexV){ 
		cursorY -= 16;
		--menuIndexV;
		*p2 = 0xAE;
		*menupointer = 'D';
		
	}

	// Down the menu, only move down if we are not at the bottom of the menu
	if((pad1Next & PAD_DOWN) && (menuIndexV < 3)){ 
		cursorY += 16;
		++menuIndexV;
		*p2 = 0xB1;
		*menupointer = 'T';
	}

	// Left, same conditions horizontally

	if((pad1Next & PAD_LEFT) && menuIndexH){
		cursorX -= 128;
		--menuIndexH;
		*p2 = 0x25;
		
	}

	// Right
	if((pad1Next & PAD_RIGHT) && !menuIndexH){
		cursorX += 128;
		++menuIndexH;
		*p2 = 0x49;
		
	}

	// A or right page flip
	if((pad1Next & PAD_A)){

		if(page == 3) page = 0;
		else ++page;
		clear_vram_buffer();
		draw_bg_menu(page);


	}

	if((pad1Next & PAD_B)){

		if(!page) page = 3;
		else --page;
		clear_vram_buffer();
		draw_bg_menu(page);
	}

}


void main (void) {
	
	ppu_off(); // screen off
	*p2 = 0xEF;

	pal_spr(palSprites);
	
	for(i = 0;i < BALLS_MAX; ++i){

		ball_x[i] = rand8();
		ball_y[i] = rand8();
		j = rand8();

		// x speed
		spr = 1 + (rand8() % 3);
		ball_dx[i] = j & 1? -spr:spr;

		// y speed

		spr = 1 + (rand8() % 3);
		ball_dy[i] = j & 1? -spr:spr;
	}

	pal_bg(palette); //	load the BG palette
	// Load pallete colors
	pal_spr(palSprites);

	// Write fake menu entries
		put_str(NTADR_A(1, 2), "GO GATORS!");
		put_str(NTADR_A(1, 4), "VIDEO MODE:");

		if(ppu_system()) put_str(NTADR_A(1, 5), "NTSC");
		else put_str(NTADR_A(1, 5), "PAL");

		
		put_str(NTADR_A(7, 16), "INDEX: ");
		put_str(NTADR_A(8, 17), "PAGE: ");
		draw_bg_menu(page);
		ppu_on_all();


	// Infinite loop for now
	while (1){
		// Wait for the next frame to be ready
		
		ppu_wait_nmi();
		oam_clear();
		
		spr = 0;

		for(i = 0; i < BALLS_MAX; ++i){

			if(i % 2) oam_spr(ball_x[i], ball_y[i], 0x55, i % 4);
			else oam_spr(ball_x[i], ball_y[i], 0x46, i % 4);

			ball_x[i] += ball_dx[i];
			ball_y[i] += ball_dy[i];
			if(ball_x[i]>=(256-8)) ball_dx[i]=-ball_dx[i];
			if(ball_y[i]>=(240-8)) ball_dy[i]=-ball_dy[i];

		}

		// Draw the cursor on the screen
		oam_spr(cursorX, cursorY, 0x7F, 0x00);
		// Read player1, go through logic depending on the inputs
		pad1 = pad_poll(0);
		pad1Next = get_pad_new(0);
		handleMenuInput();

		//Player 2 register logic
		p2val = menuIndexH + menuIndexV * 2 + page * 8;
		p2low = p2val & 0x0F;
		p2low += 0x30;
		if(p2low >= 58) p2low += 7;
		p2high = p2val & 0xF0;
		p2high = p2high >> 4;
		p2high += 0x30;

		oam_spr(0x88, 0x80, p2low, 0x0);
		oam_spr(0x80, 0x80, p2high, 0x0);
		oam_spr(0x88, 0x88, page + 0x30, 0x0);

	}
}
