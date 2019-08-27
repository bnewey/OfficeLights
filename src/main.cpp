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
#include <jsoncpp/json/json.h>


// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <sys/file.h>
#include <sys/socket.h>

#include <vector>
#include <iostream>
#include <iomanip>

// Socket header
#include <netinet/in.h> 

#include <mysql/mysql.h>
 
#define PORT 8081 


using namespace std;

const unsigned short BUFF_SIZE = 200;

void print_buf(char (&read_buf)[BUFF_SIZE], int numIterations, int numReads){
	cout<< numReads << ": "<<endl<<"Print iterations: "<< numIterations<<endl;

	for (int i = 0; i < BUFF_SIZE; ++i)
		cout <<  hex << setfill('0') << setw(2)  << (int)(*(unsigned char*)(&read_buf[i])) << dec << " ";

	cout <<  endl<<endl;
}

void read_bytes(char  (&read_buf)[BUFF_SIZE],int & serial_port, int & numIterations) {
		/* reading above 255 is tricky, we need to read BUFFSIZE bytes exactly, so this ensures it */
		int totalNeeded = BUFF_SIZE;
		int remaining   = sizeof(read_buf);
		numIterations=0;
		while (remaining > 0){
			try{
				ssize_t readChars = read(serial_port, &read_buf[totalNeeded - remaining], remaining);
				if (!(readChars > 0)){
					return;
				}
				else{
					remaining -= readChars;
					numIterations++;
				}
			} catch(ssize_t readChars){
				cout<<"Read exception caught"<<endl;
			}	
		}
}

int usb_port(int & serial_port) {
	serial_port = open("/dev/ttyUSB0", O_RDWR );
	//Check for errors
	if (serial_port < 0) {
	    printf("Error %i from open: %s\n", errno, strerror(errno));	
		return(serial_port);	
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

		tty.c_cc[VTIME] = 2;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
		tty.c_cc[VMIN] = 0; //wait for BUFF_SIZE bytes to fill then return

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
		return(serial_port);
	}
}

void mysqlConnect(MYSQL & mysql){

	//Connect to MySQL server
	mysql_init(&mysql);
	mysql_options(&mysql,MYSQL_READ_DEFAULT_GROUP,"nitrogen");
	printf("MYSQL INFO: %s\n", mysql_get_client_info());
	if (!mysql_real_connect(&mysql,"127.0.0.1","nitro","n1tr0","db_nitro",0,NULL,0))
	{
		fprintf(stderr, "Failed to connect to database: Error: %s\n",
			mysql_error(&mysql));
		return;
	}
	cout<<"Successfully Connected to MYSQL"<<endl;
	return;
	
}

vector<string> mysqlQuery(MYSQL & mysql, const char * field_name){
	vector<string> machines;
	unsigned int num_fields;
	MYSQL_ROW row;

	if(mysql_query(&mysql, "SELECT * from machines")){
		//error
		cout<<"MySQL Error"<<endl;
		return machines;
	}else{
		MYSQL_RES *result  = mysql_store_result(&mysql);
		if(result){
			//Find the index of our machine table name field 
			num_fields = mysql_num_fields(result); 
			MYSQL_FIELD *fields;
			unsigned int field_index;
			fields = mysql_fetch_fields(result);
			for(unsigned int i = 0; i < num_fields; i++){
				if(!(strcmp(fields[i].name , field_name))){
					field_index = i;
				}
			}			
	
			//Search through rows
			while ((row = mysql_fetch_row(result)))	{
				for(unsigned int p = 0; p < num_fields; p++) {	
					//if field matches our field_name from above, record that cell
					if(p == field_index){
						string t = row[p];//(1, row[p]);
						machines.push_back(t);
					}
				}
			}
			mysql_free_result(result);
			return machines;
		}else{
			fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
			return machines;
		}
	}
}

void mysqlCloseConnect(MYSQL &mysql){
	mysql_close(&mysql);
}

int readSocket( int & new_socket ){
	int valread;
	char buffer[1024] = {0}; 
	//valread = read( new_socket , buffer, 1024);
	valread = recv( new_socket, buffer,1024, 0);

    return(valread);
}

void sendSocket(int & new_socket, char const * data, const unsigned short DATA_SIZE){
	try{
    	send(new_socket , data , DATA_SIZE , 0 ); 
		printf("Data message sent from C++\n"); 
	}
	catch (int e){
		printf("*** could not send data ***");
	}
}

int socket(int & server_fd){
	int new_socket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        cout<<"*** Failed to create socket ***"<<endl; 
        exit(EXIT_FAILURE); 
    } 
    cout<< "Created Socket "<<endl;
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        cout<<"*** Failed to run setsockopt() ***"<<endl; 
        exit(EXIT_FAILURE); 
    }
	
	// set a timeout for client to connect / read from 
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)){
		
	}

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
    
    // Forcefully attaching socket to the port 8080 
    int error_num = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(error_num < 0 ){ 
       cout<<"*** Bind Failed ***"<<endl; 
        exit(EXIT_FAILURE); 
    } 	
	if (listen(server_fd, 3) < 0) 
    { 
        cout<<"*** listen ***"<<endl; 
        exit(EXIT_FAILURE); 
    } 
	// socket blocks program on accept() until either a client accepts or timeout occurs
    int error_num2 = new_socket = accept(server_fd, (struct sockaddr *)&address,   (socklen_t*)&addrlen);
    if(error_num2 < 0){ 
        cout<<"*** No Client Accepted Connection *** - "<<errno<<endl; 
		return -1;
    } 

	return new_socket;
}

string createJsonString(char  (&read_buf)[BUFF_SIZE], const vector<string> &machines){
	//string machines[8] = {"Air Compressor", "Air Dryer", "Tank 1", "Tank 1_3", "Tank 2_3", "Tank 3_3", "Generator", "Nitrogen Tank"}; 
	int temp, pressure;
	vector<Json::Value> arrayVehicles;
	Json::Value root;
	Json::Value myJson = root["machines"];

	for(int i = 0; i < 8; i++)
	{
		stringstream ss;
		ss.clear();
		ss << hex << setfill('0') << setw(2)  << (int)(*(unsigned char*)(&read_buf[(i*2)]));
		ss >> temp;
		ss.clear();
		ss << hex << setfill('0') << setw(2)  << (int)(*(unsigned char*)(&read_buf[(i*2)+1]));
		ss >> pressure;
		
		myJson["id"] = Json::Value("machine_"+to_string(i));
		string machineName = machines[i]; 
		myJson["name"] = Json::Value(machineName);
		myJson["temp"] = Json::Value::Int(temp);
		myJson["pressure"] = Json::Value::Int(pressure);

		arrayVehicles.push_back(myJson);
	}

	Json::FastWriter fastWriter;
	string output = "{ \"machines\":  [ ";
	for(int i=0; i<8; i++){
		if(i != 0)
			output += ",";
		output += fastWriter.write(arrayVehicles[i]);
	}
	output += " ] }";
	
	return(output);
}

int main() {
	
	//initialize socket 
	int server_fd;
	int new_socket = socket(server_fd);
 	
	// Allocate memory for read buffer, set size according to your needs
	int serial_port;
	char read_buf [BUFF_SIZE];
	memset(&read_buf, '\0', sizeof(read_buf));

    //Set / read in settings for our Port
	usb_port(serial_port);

	//Connect to MySQL Database
	MYSQL mysql;
	mysqlConnect(mysql);
	
	//Query for machine names to use with our JsonString
	vector<string> machines;	
	machines =	mysqlQuery(mysql, "name");
	
	//numReads: num of reads from port
	//n: num of iterations to read exact num of bits | 0 means nothing read this iteration, > 0 means something has been read 
    int numReads = 0, numIterations = 0, missed_reads = 0;

    while(true){
		
		//read_bytes() might miss once or twice every once in a while, if it misses more than 10 times in a row,
		//    the port might have been disconnected, so we will check for reconnection
		if(missed_reads > 10){ //if the usb port been disconnected
			if(serial_port > 0){
				close(serial_port);
			}
			
			if(usb_port(serial_port) > 0){
				numReads = 0;
				cout<<"Successfully reconnecting to Port"<<endl;
				missed_reads = 0;
			}else{
				cout<<"Attempting to reconnect to Port in 5 seconds..."<<endl;
				usleep(5000000);
			}
				
		}else{ // if usb not disconnected, read
			read_bytes(read_buf, serial_port, numIterations);
		}
	    
		if(numIterations > 0){ //if something was read from USB, do stuff with this data
			missed_reads = 0;

			print_buf(read_buf, numIterations, numReads);

			const string tmp2 = createJsonString(read_buf, machines);

			//convert string to char array
			char const * stringified_json = tmp2.c_str();
			int size = strlen(stringified_json);

			//make sure client is still connected to socket
			int stillAlive = readSocket(new_socket);

			//read from node js socket here
			//sterilize string here
			//write to port here

			if(stillAlive > 0){
				sendSocket(new_socket, stringified_json, size);	
			}else{
				//create 'new' socket (should resuse old one) and wait for client to reconnect until timeout
				cout<<"Client disconnected; Waiting for reconnect."<<endl;
	
				if(new_socket != -1)
					close(new_socket);
				if(server_fd != -1)
					close(server_fd);

				server_fd = -1;
				new_socket = socket(server_fd);
				
			}
		}else{
			missed_reads++;			
		}
        numReads++;
    }
	//Clean up
	mysqlCloseConnect(mysql);
	close(serial_port);


}

