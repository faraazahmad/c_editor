#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// store the original mode
struct termios orig_termios;

// function to disable the raw mode of the terminal
void disableRawMode() {
	// set the attributes to original input mode
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

	/*
		TCSAFLUSH
	    the change occurs after all output written to the object referred by 
	    fd has been transmitted, and all input that has been received but not 
	    read will be discarded before the change is made. 
	*/
}

// function to enable raw mode of the terminal
void enableRawMode() {
	// get current input mode of terminal
	tcgetattr(STDIN_FILENO, &orig_termios);
	// disable raw mode at exit of program
	atexit(disableRawMode);

	// copy the terminal attributes to make changes
	struct termios raw = orig_termios;

	/*
		The c_lflag field is for “local flags”. A comment in macOS’s <termios.h> 
		describes it as a “dumping ground for other state”. So perhaps it should
		be thought of as “miscellaneous flags”. The other flag fields are c_iflag
		(input flags), c_oflag (output flags), and c_cflag (control flags), all of
		which we will have to modify to enable raw mode.

		ECHO is a bitflag, defined as 00000000000000000000000000001000 in binary. 
		We use the bitwise-NOT operator (~) on this value to get 11111111111111111111111111110111. 
		We then bitwise-AND this value with the flags field, which forces the fourth bit in the flags 
		field to become 0, and causes every other bit to retain its current value. 
		Flipping bits like this is common in C.
	*/

	/*
		Following are c_lflags
		Refer: http://man7.org/linux/man-pages/man3/termios.3.html

		ECHO = echo input characters
		ICANON = Enable canonical mode

		~(ECHO | ICANON) = disable ECHO and ICANON
	*/
	raw.c_lflag &= ~(ECHO | ICANON);

	// finally set the raw mode
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enableRawMode();

	char c;
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
		/*
			iscntrl() checks if the argument is a control character
			i.e. characters with ASCII 0-31. They are non printable.
		*/
		if(iscntrl(c)) {
			printf("%d\n", c);
		} else {
			printf("%d ('%c')\n", c, c);
		}

	}
	return 0;
}