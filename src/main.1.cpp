//============================================================================
// Name        : com.cpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================


// C library headers
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <sys/file.h>

#include <vector>
#include <iostream>
#include <iomanip>

#include <redox.hpp>


using namespace std;
using namespace redox;

const unsigned short BUFF_SIZE = 1000;

void print_buf(char (&read_buf)[1001], int n, int q){
	cout<< q << ": "<<endl<<"Print iterations: "<< n<<endl;

	for (int i = 0; i < BUFF_SIZE; ++i)
		cout <<  hex << setfill('0') << setw(2)  << (int)(*(unsigned char*)(&read_buf[i])) << dec << " ";

	cout <<  endl<<endl;

}

void convert_string(string buffer){
    char cstr[BUFF_SIZE + 1];
    strcpy(cstr,buffer.c_str());
    print_buf( cstr,1,1);
}

void read_bytes(char  (&read_buf)[BUFF_SIZE],int & serial_port, int & n) {
		/* reading above 255 is tricky, we need to read 1000 bytes exactly, so this ensures it */
		int totalNeeded = BUFF_SIZE;
		int remaining   = sizeof(read_buf);
		n=0;
		while (remaining > 0){
    	ssize_t readChars = read(serial_port, &read_buf[totalNeeded - remaining], remaining);
			if (!(readChars > 0)){
				return;
			}
			else{
					remaining -= readChars;
					n++;
   	 		}
		}
}

void usb_port(int & serial_port) {
	serial_port = open("/dev/ttyUSB0", O_RDWR );
	//Check for errors
	if (serial_port < 0) {
	    printf("Error %i from open: %s\n", errno, strerror(errno));		
	}
	else{
		// Create new termios struc, we call it 'tty' for convention
		struct termios tty;
		memset(&tty, 0, sizeof tty);

		// Read in existing settings, and handle any error
		if(tcgetattr(serial_port, &tty) != 0) {
		//	printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		}

		tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
		tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
		tty.c_cflag |= CS8; // 8 bits per byte (most common)

		//"Enabling this when it should be disabled can result in your serial port receiving no data"
		tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)

		tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

		tty.c_lflag &= ~ICANON; //Canonical mode is disabled

		//If this bit is set, sent characters will be echoed back.
		tty.c_lflag &= ~ECHO; // Disable echo
		tty.c_lflag &= ~ECHOE; // Disable erasure
		tty.c_lflag &= ~ECHONL; // Disable new-line echo

		tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP (We don’t want this with a serial port)

		tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow control

		tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes, we just want raw data

		tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
		tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
		// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
		// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

		tty.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
		tty.c_cc[VMIN] = 200; //wait for BUFF_SIZE bytes to fill then return

		// Set in/out baud rate to be 9600
		// If you have no idea what the baud rate is and you are trying to communicate with a 3rd party system,
		//   try B9600, then B57600 and then B115200 as they are the most common rates.
		cfsetispeed(&tty, B230400);
		cfsetospeed(&tty, B230400);

		// Save tty settings, also checking for error
		if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		//	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		}
	
		//prevent other processes from reading/writing to the serial port at the same time you are.
    	if(flock(serial_port, LOCK_EX | LOCK_NB) == -1) {
			throw std::runtime_error("Serial port with file descriptor " +
				std::to_string(serial_port) + " is already locked by another process.");
		}
	
	}
}

int main() {

	Redox rdx;
 	if(!rdx.connect("localhost", 6379)){
	   return 1;
	}
  	

	// Allocate memory for read buffer, set size according to your needs
	int serial_port;
	char read_buf [BUFF_SIZE];
	memset(&read_buf, '\0', sizeof(read_buf));

    //Set / read in settings for our Port
	usb_port(serial_port);

    string s = "";

    int q, n = 0;
    while(true){

	    read_bytes(read_buf, serial_port,n);
        cout<<"Iteration to complete: "<<n<<endl;

        stringstream ss;
        for(int i=0; i<1000; i++){
             ss << hex << setfill('0') << setw(2)  << (int)(*(unsigned char*)(&read_buf[i]));
        }
        string mystr = ss.str();


        //string s(read_buf);
        cout<<"size: "<<mystr.size()<<"  -  "<<mystr<<endl;

        rdx.set("hello", mystr);
        //convert_string(rdx.get("hello"));
        
        //print_buf(read_buf, n, q );
        q++;
    }
	//Clean up
	close(serial_port);

}

