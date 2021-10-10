#include "../include/logWriter.h"

//typedef enum{TRACE,WARNING_LOG,ERROR_LOG,DEBUG}TRACES;

int main()
{
//	name = "logWriter";
//	sizeProcess = 5;
//	logLevel = CLOG_ALL;
//	activeLog = LOG_TO_ALL;
//	maxLogs = 2;
//	logNameConfig = "logs_";
//	pathLog = "logs/";


	logWriter log("logWriter",CLOG_ALL,LOG_TO_ALL,3,"logs_","logs/",10*1024*1024,true);

	log.WriteLog(TRACE, "Primera traza", "main");
	sleep(2);
	log.WriteLog(TRACE, "Segunda traza", "main");
	sleep(2);
	log.WriteLog(TRACE, "Segunda traza", "main");

	log.deletelogs();

	return 0;
}
