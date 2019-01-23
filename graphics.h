//Header file "graphics.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
//For color, upper 5 bits are for red (shift 11), middle 6 for green
//(shift 5), and lower 5 are for blue(shift none)
#define RGB(r,g,b) (color_t)((r<<11)|(g<<5)|(b))
//Absolute value function to be implemented in draw_line since
//A negative value would result in no line being drawn
#define abso(x) ((x)<0?-(x):(x))

typedef unsigned short color_t;

void init_graphics();

void exit_graphics();

char getkey();

void sleep_ms(long ms);

void clear_screen(void *img);

void draw_pixel(void *img, int x, int y, color_t color);

void draw_line(void *img, int x1, int y1, int x2, int y2, color_t color);

void *new_offscreen_buffer();

void blit(void *src);

