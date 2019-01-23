/*CS1550 Fall 2018 Project 1: Snake Library File
*
*Author: Kiran Pawar
*
*Holds custom library functions to be used by the driver snake.c
*/
#include <stdio.h>
#include "graphics.h"



static int fd;			//File descriptor
static long int screen_sz;	//Screen resolution
static void *fb_ptr;		//File buffer pointer
static void *off_buff;		//Offscreen buffer pointer
struct fb_fix_screeninfo finfo;	//Fixed Screen info
struct fb_var_screeninfo vinfo;	//Variable Screen info
struct termios terminal;	//Terminal (screen)

//Initialize the graphics library, which will do the following
//1. Open the graphics devince
//2. Map memory to write to the screen
//3. Establish the screen size
//4. Create a cleared screen on initalization
//5. Disable keypress echo using ioctl
void init_graphics(){
	//Open the device for reading and writing
	fd = open("/dev/fb0", O_RDWR);

	if(ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1)
		return;				//Return success = 0;
	if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) == -1)
		return;

	//Get screen resolution (640x480)
	screen_sz = vinfo.yres_virtual * finfo.line_length; //Screen size
	
	//Map memory needed
	fb_ptr = mmap(0, screen_sz, PROT_WRITE, MAP_SHARED, fd, 0);

	//I/O control to disable keypress echo
	if(ioctl(STDIN_FILENO, TCGETS, &terminal) == -1)
		return;
	terminal.c_lflag &= ~(ICANON | ECHO);	//Set flag for key input
	if(ioctl(STDIN_FILENO, TCSETS, &terminal) == -1)
		return;
	write(1, "\033[2J", 8);	//Clear the screen

}

//exit_graphics() will undo whatever init_graphics() did
//Essentially, it will reenable keypress echoing, unmap the mapped
//memory, and close the file
void exit_graphics(){

	terminal.c_lflag |= (ICANON | ECHO);	//Revert key input

	ioctl(STDIN_FILENO, TCSETS, &terminal);
	munmap(fb_ptr, screen_sz);
	close(fd);
}

//Since C library functions are prohibited in this assignement,
//we'll need to create our own way of reading keyboard input in
//place of getchar() and other similar C functions
//getkey() will utilize system calls to accomplish this
char getkey(){
	char keystroke;		//Holds the key input
	//Following code was taken from the linux manual
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	struct timeval tval;

	tval.tv_sec = 0;
	tval.tv_usec = 10;	//Set to 10 nanosec for faster snake spd

	//Stop read from blocking using select()
	int key_buff = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tval);

	if(key_buff){	//If select detects an input
		//Read input
		read(STDIN_FILENO, &keystroke, sizeof(keystroke));
	}
	return keystroke;
}

//sleep_ms will make our program sleep in between graphic frames
void sleep_ms(long ms){

	struct timespec timey;
	timey.tv_sec = 0;
	timey.tv_nsec = ms * 1000000;
	nanosleep(&timey, NULL);
}

//clear_screen() will clear the screen of whatever was written onto it
//It will be used at the start of a loop so that the previous frame is
//wiped, and the new frame is drawn
void clear_screen(void *img){

	int i;
	void *temp_ptr = img;
	for(i = 0; i < screen_sz - 1; ++i){	//Copy from 0 to 679
		 *((color_t *)temp_ptr++) = 0;
	}

}

//draw_pixel will plot a pixel to a specific (x,y) coordinate
void draw_pixel(void *img, int x, int y, color_t color){
	unsigned int x_axis;	
	unsigned int y_axis;
	color_t *plot;
	if(x < 0 || x >= vinfo.xres_virtual || y < 0 ||y >=	
	vinfo.yres_virtual){
		return;
	}
	else{
		//Each pixel is represented by 2 bytes (16 bits)
		//So divide bbp by 8 to get number of bytes
		x_axis = (x+vinfo.xoffset)*(vinfo.bits_per_pixel/8);
		y_axis = (y+vinfo.yoffset)*(finfo.line_length);
//		printf("x = %d,y = %d, y_axis = %d\n",x, y, 
//		y_axis);
		plot = img + x_axis + y_axis;
		*plot = color;
	}
}

//Sign(x) is needed for when the value of x1 or y1 is greater than
//x2 or y2 respectively
int sign(x){
	if(x>0){
		return 1;
	}
	else if(x < 0){
		return -1;
	}
	else{
		return 0;
	}
}

//draw_line() utilizes Bresenham's algorithm to plot a series of
//pixels in a line(using the draw_pixel() function)
//Source: csourcecodes.blogspot.com/2016/06/bresenhams-line-drawing-algorithm-generalized-c-program.html
void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c){
	int x,y,dx,dy,swap,temp,s1,s2,p,i;
	
	x = x1;
	y = y1;
	dx = abso(x2-x1);
	dy = abso(y2-y1);
	s1 = sign(x2-x1);
	s2 = sign(y2-y1);
	swap = 0;
	draw_pixel(img, x1, y1, c);
	if(dy > dx){
		temp = dx;
		dx = dy;
		dy = temp;
		swap = 1;
	}
	p = 2*dy-dx;
	for(i = 0; i < dx; i++){
		draw_pixel(img, x, y, c);
		while(p>=0){
			p = p-2*dx;
			if(swap) x+= s1;
			else y+= s2;
		}
		p = p+2*dy;
		if(swap) y+=s2;
		else x+=s1;
	}
	draw_pixel(img, x2, y2, c);
}

//new_offscreen_buffer will create a second, hidden memory map that is
//identical to the memory map initialized
//It will be used to load up the next frame while one is shown
//That frame will then be transferred to the main memory map using
//blit()
void *new_offscreen_buffer(){
	//Create anonymous memory map
	off_buff = (void *)mmap(NULL, screen_sz, 
	PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

	return off_buff;
}

//blit() will take the memory mapped in new_offscreen_buffer() and
//produce it identically to the main memory map using pointers
void blit(void *src){	//src should = off_buff when used

	int i;
	void * temp_ptr;
	temp_ptr = fb_ptr;
	//We'll need to copy the memory mapped to the anon map
	//to the map that is displayed on screen
	for(i = 0; i < screen_sz - 1; ++i){	//Copy from 0 to 679
		 *((color_t *)temp_ptr++) = *((color_t *)src++);
	}
}
