//Fixes program in case exit_graphics() isn't used to reenable key echo

#include "graphics.h"

int main(){
	init_graphics();
	exit_graphics();
	return 0;
}

