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


#define PORT 8081 //also defined in functions.h


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
	char ui_buf[UI_BUFF_SIZE];
	memset(&ui_buf, '\0', sizeof(ui_buf));

	//Allocate memory for write_buf command buffer
	char write_buf[WRITE_BUFF_SIZE];
	memset(&write_buf, '\0', sizeof(write_buf));

	//Create vector for our switch values
	vector<short> switch_vector(SWITCHES_MAX_SIZE,0);

    //Set / read in settings for our Port
	usb_port(serial_port);

	//Connect to MySQL Database
	MYSQL mysql;
	mysqlConnect(mysql);
	

	// //Get Switch config variables
	vector< vector<string> > switch_variables;
	if(!(mysqlQueryFixed(mysql,"SELECT id, array_index, type, name, description, toggle_timer, move_timer, mode FROM switches ORDER BY id ASC" , switch_variables))){
		cout<<"Query to MySQL did not successfully get switch variables, default variables applied"<<endl;
		return 0;
	}

	//Get Light config variables
	vector<vector<string> > light_variables;
	if(!(mysqlQueryFixed(mysql,"SELECT id, array_index, switch_id, value, name, description  FROM lights ORDER BY id ASC" , light_variables))){
		cout<<"Query to MySQL did not successfully get light variables"<<endl;
		return 0;
	}

	//Set Start time for Timers
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
	//Set Start Time for DB update
	std::chrono::steady_clock::time_point db_timer1 = std::chrono::steady_clock::now();
	
	//Create SwitchHandler with our config variables
	shared_ptr<SwitchHandler> sh(make_shared<SwitchHandler>(switch_variables, light_variables));

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
			string command = "python " + GetCurrentWorkingDir() + "/reset_usb.py pathpci /sys/bus/pci/drivers/xhci_hcd/0000:00:14.0";
			string command2 = "python "  + GetCurrentWorkingDir() + "/reset_usb.py search \"Future Technology Devices International\"";
			system(command.c_str());
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
			totalReadChars = 0;
			numIterations = 0;
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
			const string tmp2 = (*sh).createJsonDataString( numJsonSends);
			//convert string to char array
			char const * stringified_json = tmp2.c_str();
			int size = strlen(stringified_json);

			//make sure client is still connected to socket
			int stillAlive = readNodeSocket(new_socket, ui_buf);
			//Print UI buff 
			print_ui_buff(ui_buf);

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
				if(ui_buf[0]=='9' && ui_buf[1]=='9'){
					//Refetch switch_variables
					if(!(mysqlQueryFixed(mysql,"SELECT id, array_index, type, name, description, toggle_timer, move_timer, mode FROM switches ORDER BY id ASC" , switch_variables))){
						cout<<"Query to MySQL did not successfully get switch variables, default variables applied"<<endl;
						return 0;
					}

					//Refetch light variables
					if(!(mysqlQueryFixed(mysql,"SELECT id, array_index, switch_id, value, name, description  FROM lights ORDER BY id ASC" , light_variables))){
						cout<<"Query to MySQL did not successfully get light variables"<<endl;
						return 0;
					}

					//Reset will destruct old pointers
					sh.reset();
					//Create SwitchHandler with our config variables
					shared_ptr<SwitchHandler> sh_tmp(make_shared<SwitchHandler>(switch_variables, light_variables));
					sh = sh_tmp;
					cout<<"Switch Variables Changed"<<endl;
				}

				

			}else{
				//TODO create a new thread that returns with new_socket once UI has reconnected

				//create 'new' socket (should resuse old one) and wait for client to reconnect until timeout
				cout<<"Client disconnected; Waiting for reconnect."<<endl;

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

			//Send Update to DB ever 30 seconds
			std::chrono::steady_clock::time_point db_timer2 = std::chrono::steady_clock::now();
			std::chrono::duration<double> db_time_span = std::chrono::duration_cast<std::chrono::duration<double>>(db_timer2 - db_timer1);
			int tmp = db_time_span.count();
			cout<<"Time til DB Write"<<tmp<<endl;
			if( tmp > 30){
				cout<<"Writing to DB"<<endl;
				db_timer1 = std::chrono::steady_clock::now();
				vector<string> sql_strings = (*sh).getMySqlSaveStringLights(mysql);
				auto iter = sql_strings.begin();
				for ( ; iter !=  sql_strings.end(); iter++){
					if(!(mysqlQueryNoReturn(mysql, (*iter)))){
						cout<<"Error sending light data to DB"<<endl;
					}
				}
				
			}

			//Reset writeResponse & numIterations
			numIterations = 0;
		}else{
			missedReads++;
		}
        numReads++;
    }

	//Clean up
	//mysqlCloseConnect(mysql);
	
	close(serial_port);


}

