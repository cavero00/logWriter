/*
# Name of file:
#	logWriter.h
#
# Type file:
#	Header(.h)
#
# Description:
#	Class that let do different operations about files. Reading, writting, ...
#
# Made by:
#	Víctor Cavero Herranz
#
# Date (dd/mm/yyyy):
#	10/09/2021
#
*/

#pragma once
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <experimental/filesystem>

//Includes for files
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

// Type of traces
typedef enum{TRACE,WARNING_LOG,ERROR_LOG,DEBUG}TRACES;

// The log's levels
#define CLOG_ALL    0
#define CLOG_DEBUG  1

// Where the log write
#define LOG_TO_CONSOLE  0
#define LOG_TO_FILE     1
#define LOG_TO_ALL      2

// Other includes
//#include "../include/parametros.h"

using namespace std;

class logWriter
{
//	int fd;
	string dateHour;

	int sizeProcess;// Maximum caracteres for save the name of the process witch write in the log

	string name;// Name of the process

	int logLevel;
	int activeLog;
	int maxLogs;
	string logNameConfig;
	string pathLog;

	int maxSizeLog; // the maximun size in bytes of a lof file
	bool logRotated;

public:
	static int logExits;// The log has been created (logExits = 1) or not (logExits = 0)
	static string nameLog;

	logWriter(string n,int ll,int al,int ml,string lnc,string pl,int msl,bool lr);
	~logWriter(void);

	void SetDateHour(char *f){dateHour = f;}
	void SetNameLog(char *f){nameLog = f;}
	//static void SetLogExist(int i){logExits = i;}

	int Create(string name);// Create a file

	int Open2read(const char *name,char *textRead,int buf);//NO TERMINADA
	int Open2write(string name,string textWrite);// Let write in a file

	int WriteLog(TRACES trace,string textTrace,string process);// It allows to configure the message that will be written in the file
	int Date(void);// Update the hour and the day of the Log

	string GetName(){return name;}// Give the name of the process
	int SetTitleLog();//Establish the title of the log

	void deletelogs();// Delete logs
};
