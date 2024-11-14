 
/* Original example provided by Doug

	Functionality provided by Beto Perez
	Last Update 11/14/24 - Sound added - Kieran A
	; Sound effect used from NESDoug

*/
#include "LIB/neslib.h"
#include "LIB/nesdoug.h" 

// BG Palette
#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30

// Player 1 input
unsigned char pad1 = 0;
unsigned char pad1Next = 0;
unsigned accumulator = 0;
unsigned char x = 0;
unsigned char p = 0;
unsigned char p_temp = 0;
unsigned char mouse_speed = 2;

unsigned char temp = 0;
unsigned char flags[8] = {'N', 'V', '1', 'B', 'D', 'I', 'Z', 'C'};

unsigned char albert_palette = 0;

enum {SFX_JUMP, SFX_DING, SFX_NOISE};

#define MENU_SIZE 127
#define ENTRY_SIZE 16


// there's some oddities in the palette code, black must be 0x0f, white must be 0x30

const unsigned char palSprites[16] = {

	0x0f,0x09,0x26,0x11,
	0x0f,0x27,0x18,0x05,
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

// Cursor data
unsigned char cursorX = 128;
unsigned char cursorY = 120;
unsigned char cpu_status = 0;

// Mario data
unsigned char can_jump = 0;
char mario_vel_y = 0;
char mario_pos_y = 120;


#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
// all variables should be global for speed
// zeropage global is even faster

unsigned char eval_pos();

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

void draw_cursor_data(){

	oam_spr(80, 215,((cursorX % 10) + 0x30), 0x0);
	oam_spr(72, 215, (((cursorX / 10) % 10) + 0x30), 0x0);
	oam_spr(64, 215, (((cursorX / 100) % 10) + 0x30), 0x0);

	oam_spr(80, 223,((cursorY % 10) + 0x30), 0x0);
	oam_spr(72, 223, (((cursorY / 10) % 10) + 0x30), 0x0);
	oam_spr(64, 223, (((cursorY / 100) % 10) + 0x30), 0x0);
}

void draw_accumulator(){
	unsigned char final_res = accumulator;

	// Handle 2's copmliment
	if(accumulator & 128){
		oam_spr(23, 119,'-', 0x2);
		final_res = 255 - final_res;
		final_res++;
	}
	oam_spr(47, 119,((final_res % 10) + 0x30), 0x2);
	oam_spr(39, 119, (((final_res / 10) % 10) + 0x30), 0x2);
	oam_spr(31, 119, (((final_res / 100) % 10) + 0x30), 0x2);

}

void draw_cpu_status(){

	p_temp = p;

	for(i = 6; i != 2; --i){

		temp = p_temp & 1;

		if(temp){
			if(i > 3)	oam_spr((88 - 16 * (i - 3)), 182, flags[7 - i], 0x2);
			else		oam_spr((88 - 16 * (i + 3)), 190, flags[7 - i], 0x2);

		}
		p_temp >>= 1;
	}

	// Carry flag
	if(p_temp & 1) oam_spr(72, 190, flags[7], 0x2);
	// Zero flag
	if(accumulator == 0) oam_spr(56, 190, flags[6], 0x2);
	// Negative flag
	if(accumulator & 128) oam_spr(24, 182, flags[0], 0x2);


}


void draw_albert(){

	for(i = 0; i != 7; ++i){

		for(j = 1; j != 7; ++j){

			oam_spr((200 + (8 * j)), 16 + (i << 3), (0x80 + j + (i << 4)), albert_palette);
			
		}
	}
}

void draw_mario(unsigned char idle){

	if(idle){
		oam_spr(144, mario_pos_y, 0x88, 0x1);
		oam_spr(152, mario_pos_y, 0x89, 0x1);
		oam_spr(144, mario_pos_y + 8, 0x98, 0x1);
		oam_spr(152, mario_pos_y + 8, 0x99, 0x1);
	}
	else{

		oam_spr(144, mario_pos_y, 0x8A, 0x1);
		oam_spr(152, mario_pos_y, 0x8B, 0x1);
		oam_spr(144, mario_pos_y + 8, 0x9A, 0x1);
		oam_spr(152, mario_pos_y + 8, 0x9B, 0x1);

	}
}

void update_mario(){

	if(can_jump) draw_mario(1);
	else{
		draw_mario(0);
		sfx_play(SFX_JUMP, 0);
		++mario_vel_y;
		mario_pos_y += mario_vel_y;

	}

	if(mario_vel_y > 0 && mario_pos_y > 120){
		can_jump = 1;
		mario_vel_y = 0;
		mario_pos_y = 120;
	}

}

	

// Another function, handles input and logic
void handleMenuInput(void){
	unsigned char result = 0;
	// Up the menu, only move up if we are not at the top of the menu
	if(PAD_UP & pad1){ 
		cursorY -= mouse_speed;
		if(cursorY < 1) cursorY = 1;
	
	}

	// Down the menu, only move down if we are not at the bottom of the menu
	if(PAD_DOWN & pad1){ 
		cursorY += mouse_speed;
		if(cursorY > 239)cursorY = 239;
		
	}

	// Left, same conditions horizontally

	if((pad1 & PAD_LEFT)){
		cursorX -= mouse_speed;
	}

	// Right
	if((pad1 & PAD_RIGHT)){
		cursorX += mouse_speed;
	}

	// A or right page flip
	if((pad1Next & PAD_A)){

		result = eval_pos();
		if(result < 10) x = result;

		if(result == 0x0B){

			accumulator += x;
			p = get_cpu_status();

		}
		if(result == 0x0D){

			accumulator -= x;
			p = get_cpu_status();

		} 
		if(result == 0x0A){
			accumulator *= x;
			p = get_cpu_status();
		}

		if(result == 0x0F){

			if(x == 0) return;
			accumulator /= x;
			p = get_cpu_status();

		}
		if(result == 'C'){
			
			accumulator = 0;
			p = get_cpu_status();
		}
		
		if(result == 32){

			albert_palette++;
			if(albert_palette > 3) albert_palette = 0;

		}

		if(result == 33){
			
			if(can_jump){
				mario_vel_y = -10;
				can_jump = 0;
			}

		}		
	}
	if(pad1 & PAD_B)mouse_speed = 4;
	else mouse_speed = 2;

}




// Hover / register a value from the cursor
// This does a high to low level check (regions then individual sprites) to reduce if's and checks
unsigned char eval_pos(void){
	// If we are in the numbers region
	if((cursorX > 30 && cursorX < 101) && (cursorY > 134 && cursorY < 150)){

		if(cursorY < 143){
			// 0 To 4
			if(cursorX > 30 && cursorX < 38) return (unsigned char)0;
			if(cursorX > 46 && cursorX < 54) return (unsigned char)1;
			if(cursorX > 62 && cursorX < 70) return (unsigned char)2;
			if(cursorX > 78 && cursorX < 86) return (unsigned char)3;
			if(cursorX > 94 && cursorX < 102) return (unsigned char)4;
			return 255;
		}
		else{
			// 5 to 9
			if(cursorX > 30 && cursorX < 38) return (unsigned char)5;
			if(cursorX > 46 && cursorX < 54) return (unsigned char)6;
			if(cursorX > 62 && cursorX < 70) return (unsigned char)7;
			if(cursorX > 78 && cursorX < 86) return (unsigned char)8;
			if(cursorX > 94 && cursorX < 102) return (unsigned char)9;
			return 255;
		}
	}

	// Operand region
	if((cursorX > 63 && cursorX < 102) && (cursorY > 158 && cursorY < 174)){
		
		if(cursorY < 166){
			// + and -
			if(cursorX > 63 && cursorX < 70) return (unsigned char) 0x0B;
			if(cursorX > 79 && cursorX < 87) return (unsigned char) 0x0D;
			return 255;

		}
		else{
			// * / and C
			if(cursorX > 63 && cursorX < 70) return 0x0A;
			if(cursorX > 79 && cursorX < 87) return 0x0F;
			if(cursorX > 95 && cursorX < 104) return 'C';
			return 255;

		}
	}

	// Albert region

	if((cursorX > 207 && cursorX < 255) && (cursorY > 15 && cursorY < 73)) return 32;

	// Mario region

	if((cursorX > 141 && cursorX < 155) && (cursorY > 119 && cursorY < 136)) return 33;

	else return (unsigned char)255;
}

void hover(void){

	unsigned char result = eval_pos();
	unsigned char pal = 0x0;


	if(result == 255){
		return;
	}
	
	if(result < 5){
		oam_spr(24, 159, '?', 0x2);
		oam_spr(32 + (result << 4), 135, result + 0x30, pal);
		return;
	}
	
	if(result < 10){
		oam_spr(24, 159, '?', 0x2);
		oam_spr(32 + ((result - 5) << 4), 143, result + 0x30, pal);
		return;
	}

	// +
	if(result == 0x0B){

		oam_spr(64, 159, result + 0x20, pal);
		// Green on the white
		oam_spr(24, 159, result + 0x20, 0x2);
		return;
	}
	// -
	if(result == 0x0D){
		
		oam_spr(80, 159, result + 0x20, pal);

		oam_spr(24, 159, result + 0x20, 0x2);
		return;
	}

	// *
	if(result == 0x0A){
		
		oam_spr(64, 167, result + 0x20, pal);

		oam_spr(24, 159, result + 0x20, 0x2);
		return;

	}

	// div
	if(result == 0x0F){

		oam_spr(80, 167, result + 0x20, pal);

		oam_spr(24, 159, result + 0x20, 0x2);
		return;

	}
	// C
	if(result == 'C'){
		oam_spr(96, 167, result, pal);
		return;
	}

}

void main (void) {
	
	ppu_off(); // screen off

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

	put_str(NTADR_A(1, 2), "Museum Emulation Systems");
	put_str(NTADR_A(1, 4), "Video mode:");
	put_str(NTADR_A(1 ,6), "Authors: Beto Perez Kieran Abesamis");

	
	if(ppu_system()) put_str(NTADR_A(13, 4), "NTSC");
	else put_str(NTADR_A(13, 4), "PAL");

	put_str(NTADR_A(1, 15), "A: ");
	put_str(NTADR_A(1, 17), "X: 0 1 2 3 4");
	put_str(NTADR_A(1, 18), "   5 6 7 8 9");
	put_str(NTADR_A(1, 20), "A   X: + -");
	put_str(NTADR_A(1, 21), "       * / C");


	put_str(NTADR_A(1, 23), "P: ");

	put_str(NTADR_A(1, 27), "POS X: ");
	put_str(NTADR_A(1, 28), "POS Y: ");
	//draw_bg_menu(page);
	ppu_on_all();

	// Infinite loop
	while (1){

		// Wait for the next frame to be ready
		ppu_wait_nmi();
		oam_clear();
	
		// Draw the cursor on the screen and it's coordinates
		oam_spr(cursorX, cursorY, 0x7F, 0x0);
		draw_cursor_data();

		// Update text to turn blue when hovered
		hover();
	
		draw_accumulator();
		// Draw x
		oam_spr(112, 135, x + 0x30, 0x2);

		draw_cpu_status();

		draw_albert();

		update_mario();

		// Read player1, go through logic depending on the inputs
		pad1 = pad_poll(0);
		pad1Next = get_pad_new(0);
		handleMenuInput();

	}
}
