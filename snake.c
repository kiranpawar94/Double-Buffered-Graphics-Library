/*CS1550 Fall 2018: Snake driver file
*
*Author: Kiran Pawar
*
*Runs the snake program using library.c functions
*/

#include "graphics.h"

//A pseudo-snake game program, this will only present one pixel that
//will move around the screen using the arrow keys on the keyboard

//Press q to quit the program
int main(){

	init_graphics();	//Initialize graphics
	void *img = new_offscreen_buffer();
	//Starting position
	int x = 320;
	int y = 240;
	int xsign = 1;		//Start moving left to right
	int ysign = 0;
	char key;

	while(1){		//The loop that will keep the game running
		//Borders
		clear_screen(img);

		//draw_line(img, 100, 100, 100, 400, RGB(20, 50, 0));
		//draw_line(img, 100, 400, 100, 100, RGB(20, 50, 0));
		//draw_line(img, 100, 400, 100, 100, RGB(20, 50, 0));

		draw_pixel(img, x, y, RGB(31, 60, 30));
	//	draw_pixel(img, x+1, y, RGB(30, 40, 0));
	//	draw_pixel(img, x, y+1, RGB(30, 40, 0 ));
	//	draw_pixel(img, x, y+2, RGB(30, 40, 0 ));
	//	draw_pixel(img, x+2, y, RGB(30, 40, 0 ));

		//Collision detection
		//Ends the game if the pixel(head) touches any part of the line (tail)
		//If it hits a wall, will appear on the opposite side of the screen


		x = x + (xsign * 1);
		y = y + (ysign * 1);
		if(x > 600){
			x = 0;
		}
		if(y > 480){
			y = 0;
		}
		if(x < 0){
			x = 600;
		}
		if(y < 0){
			y = 480;
		}

		key = getkey();
		if(key == ('A')){
			//Turn up
			ysign = -1;
			xsign = 0;
		}

		else if(key == ('D')){
			//Turn left
			xsign = -1;
			ysign = 0;

		}

		else if(key == ('B')){
			//Turn down
			xsign = 0;
			ysign = +1;
		}

		else if(key == ('C')){
			//Turn right
			xsign = +1;
			ysign = 0;
		}
	//	else if(key == ('q') || ('Q')){
	//		return 0;
	//	}

		else if(key == 'q'){
			exit_graphics();
			exit(1);
		}
		sleep_ms(1);
		blit(img);



	}
	//exit_graphics();
}
