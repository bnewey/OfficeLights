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

#include <ctime>
#include <ratio>
#include <chrono>

// Socket header
#include <netinet/in.h> 

#include <mysql/mysql.h>

//forward declaration
class SwitchHandler;
//Functions File
#include "./functions.hpp"


#define PORT 8081 //also defined in functions.cpp


using namespace std;

//BUFF_SIZE defined in functions.cpp

int main() {
	
	//initialize socket 
	int server_fd;
	int new_socket = nodeSocket(server_fd);
 	
	// Allocate memory for read buffer, set size according to your needs
	int serial_port;
	char read_buf [BUFF_SIZE];
	memset(&read_buf, '\0', sizeof(read_buf));

	//Allocate memory for ui_buf buffer
	char ui_buf[5];
	memset(&ui_buf, '\0', sizeof(ui_buf));

	//Allocate memory for write_buf command buffer
	char write_buf[302];
	memset(&write_buf, '\0', sizeof(write_buf));

	//Create vector for our switch values
	vector<short> switch_vector(150,0);

    //Set / read in settings for our Port
	usb_port(serial_port);

	//Connect to MySQL Database
	MYSQL mysql;
	mysqlConnect(mysql);
	

	// //Get Switch config variables
	vector< vector<string> > switch_variables;
	if(!(mysqlQueryFixed(mysql,"SELECT id, array_index, type, name, description  FROM switches ORDER BY id ASC" , switch_variables))){
		cout<<"Query to MySQL did not successfully get switch variables, default variables applied"<<endl;
		return 0;
	}

	//Get Light config variables
	vector<vector<string> > light_variables;
	if(!(mysqlQueryFixed(mysql,"SELECT id, array_index, switch_id, type, name, description  FROM lights ORDER BY id ASC" , light_variables))){
		cout<<"Query to MySQL did not successfully get light variables"<<endl;
		return 0;
	}

	//Set Start time for Timers
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
	
	//Create SwitchHandler with our config variables
	SwitchHandler * sh = new SwitchHandler(switch_variables, light_variables);


	//numReads: num of reads from port
	//n: num of iterations to read exact num of bits | 0 means nothing read this iteration, > 0 means something has been read 
    long numReads = 0;
	long numJsonSends = 0;
	int numIterations = 0;
	int totalReadChars = 0;
	int missedReads =0;

    while(true){
		

		//    the port might have been disconnected, so we will check for reconnection
		if( missedReads > 10){//if the usb port been disconnected
			if(serial_port > 0){
				close(serial_port);
			}

			///
			//python test to reset USB 
			system("python /opt/officelights_git/OfficeLights/reset_usb.py pathpci /sys/bus/pci/drivers/xhci_hcd/0000:00:14.0");
			usleep(5000000);

			///
			
			if(usb_port(serial_port) > 0){
				numReads = 0;
				cout<<"Successfully reconnecting to Port"<<endl;
				missedReads=0;
				totalReadChars = 0;
				numIterations = 0;
				t1 = std::chrono::steady_clock::now();
				//Set stop on reconnect
				
			}else{	
				//Send error to UI
				string error_json = createJsonString("{error: 1}");
				char const * stringified_error_json = error_json.c_str();
				int error_size = strlen(stringified_error_json);
				sendNodeSocket(new_socket, stringified_error_json , error_size);
				cout<<"Attempting to reconnect to Port in 5 seconds..."<<endl;
				usleep(5000000);
			}
				
		}else{ // if usb not disconnected, poll & read
			totalReadChars = read_bytes(read_buf, serial_port, numIterations);	
		}

		if( numIterations > 0 && totalReadChars > 0){ 
			//if something was successfully polled and read from USB, do stuff with this data
			//cout<<"If poll&read ----"<<"WriteResponse: "<<writeResponse<<"  |  ReadResponse: "<<numIterations<<endl;
			print_buf(read_buf, numIterations ,totalReadChars);
			missedReads = 0;
	
			//Handle Time Updates
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			t1 = std::chrono::steady_clock::now();
			(*sh).updateTimers(time_span.count());

			//Take read data and fill our switch vector for update
			//switch_vector can be edited from UI commands also
			getDataFromRead(read_buf, switch_vector);			

			numJsonSends++;
			const string tmp2 = createJsonDataString(read_buf, sh, numJsonSends);
			//convert string to char array
			char const * stringified_json = tmp2.c_str();
			int size = strlen(stringified_json);

			//make sure client is still connected to socket
			int stillAlive = readNodeSocket(new_socket, ui_buf);

			//read from node js socket here
			//sterilize string here
			//write to port here
			
			if(stillAlive > 0){
				//Send out data to NodeJS
				sendNodeSocket(new_socket, stringified_json, size);
				
				if(ui_buf[0]=='0' && ui_buf[1]=='5'){
					//toggle light
					string switch_to_toggle = "";
					switch_to_toggle.push_back(ui_buf[2]);
					switch_to_toggle.push_back(ui_buf[3]);
					switch_to_toggle.push_back(ui_buf[4]);
					int switch_id = stoi(switch_to_toggle);
					//Edit switch_vector
					if(!((*sh).setSwitchToggle(switch_id))){
						cout<<"Error: Did not toggle switch with array_index:"<<switch_id<<endl;
					}
					
					
					
				}
				if(ui_buf[0]=='0' && ui_buf[1]=='6'){
					// change to turn all on(*sh).setStart();					
					
				}
				// if(ui_buf[0]=='9' && ui_buf[1]=='9'){
				// 	//Refetch switch_variables
				// 	if(!(mysqlQueryFixed(mysql, switch_variables))){
				// 		cout<<"Query to MySQL did not successfully get switch_variables, default variables applied"<<endl;
				// 	}
				// 	//create new object 
				// 	/*SwitchHandler switchHandlerNew(mode_variables[0][1] == "timer_mode2_wait" ? stof(mode_variables[0][0]) : float(120),
				// 			mode_variables[1][1] == "timer_mode4_wait" ? stof(mode_variables[1][0]) : float(10),
				// 			mode_variables[2][1] == "timer_motor_relay" ? stof(mode_variables[2][0]) : float(30),
				// 			mode_variables[3][1] == "timer_start_relay" ? stof(mode_variables[3][0]) : float(2),
				// 			mode_variables[4][1] == "timer_stop_relay" ? stof(mode_variables[4][0]) : float(2),
				// 			mode_variables[5][1] == "timer_shut_down_counter" ? stof(mode_variables[5][0]) : float(30),
				// 			mode_variables[6][1] == "timer_bleed_relay_m45" ? stof(mode_variables[6][0]) : float(5),
				// 			mode_variables[7][1] == "timer_bleed_relay_m1" ? stof(mode_variables[7][0]) : float(2),
				// 			mode_variables[8][1] == "max_high_pressure" ? stoi(mode_variables[8][0]) : 400,
				// 			mode_variables[9][1] == "high_pressure_thres" ? stoi(mode_variables[9][0]) : 350,
				// 			mode_variables[10][1] == "max_low_pressure" ? stoi(mode_variables[10][0]) : 94,
				// 			mode_variables[11][1] == "low_pressure_thres" ? stoi(mode_variables[11][0]) : 86,
				// 			mode_variables[12][1] == "min_low_pressure" ? stoi(mode_variables[12][0]) : 60,
				// 			mode_variables[13][1] == "shut_down_counter" ? stoi(mode_variables[13][0]) : 2); 
				// 	sh =&switchHandlerNew;*/
				// 	cout<<"Switch Variables Changed"<<endl;
				// }

				

			}else{
				//create 'new' socket (should resuse old one) and wait for client to reconnect until timeout
				cout<<"Client disconnected; Waiting for reconnect."<<endl;

				//(*sh).setStop();
				cout<<"Hitting the STOP button because we lost control of UI..."<<endl;

				if(new_socket != -1)
						close(new_socket);
					if(server_fd != -1)
						close(server_fd);

					server_fd = -1;
				new_socket = nodeSocket(server_fd);

				while(new_socket == -1){
					if(new_socket != -1)
						close(new_socket);
					if(server_fd != -1)
						close(server_fd);

					server_fd = -1;
					new_socket = nodeSocket(server_fd);
				
					cout<<"Check ip addr to make sure IP ADDRESS is correct. New socket:"<<new_socket<<endl;
				}
				
				
			}
	
			//Call Update to SwitchHandler Object
			(*sh).updateSwitches(switch_vector);

			//Have this write section after we recieve potential override commands from UI
			//Edit write_buf with relay_p pointer to array
			editWriteBuf(write_buf, sh);
				

			//Write to Serial Port to Update Relays
			write_bytes(serial_port, write_buf);

			print_write_buff(write_buf, 3 ,3 );

			//Reset writeResponse & numIterations
			numIterations = 0;
		}else{
			missedReads++;
		}
        numReads++;
    }

	delete sh;
	//Clean up
	//mysqlCloseConnect(mysql);
	
	close(serial_port);


}

