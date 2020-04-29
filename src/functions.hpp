#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

//============================================================================
// Name        : functions.hpp
// Author      : Ben Newey
// Version     :
// Copyright   : Your copyright notice
// Description : All basic functions for port reading / mysql / json sending to UI
//============================================================================


#include "./switch_handler.hpp"
//#include "./tmpSwitch.cpp"
 
#define PORT 8081 

using namespace std;

const short BUFF_SIZE = 502;

void print_buf(char (&read_buf)[BUFF_SIZE], int numIterations, int numReads);

void print_write_buff(char (&write_buf)[502], int numIterations, int numReads);

int read_bytes(char  (&read_buf)[BUFF_SIZE],int & serial_port , int & numIterations);

void getDataFromRead(char  (&read_buf)[BUFF_SIZE], vector<short> & switch_vector);

void editWriteBuf(char (&temp)[502] , SwitchHandler * sh);

void write_bytes(int & serial_port, char (&temp)[502]);

int usb_port(int & serial_port) ;

void mysqlConnect(MYSQL & mysql);

int mysqlQuery(MYSQL & mysql, vector<string> & return_array, const char * field_name);

int mysqlQueryFixed(MYSQL & mysql, vector< vector<string> > & return_array);

void mysqlCloseConnect(MYSQL &mysql);

int readNodeSocket( int & new_socket, char  (&ui_buf)[4] );

void sendNodeSocket(int & new_socket, char const * data, const unsigned short DATA_SIZE);

int nodeSocket(int & server_fd);

string createJsonDataString(char  (&read_buf)[BUFF_SIZE],  SwitchHandler * sh, long numJsonSends);

string createJsonString(string message);




#endif    /* FUNCTIONS_HPP */