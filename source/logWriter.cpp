/*
# Name of file:
#	logWriter.cpp
#
# Type file:
#	Source code(.cpp)
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

#include "../include/logWriter.h"

// Static variables, only one inicialization
int logWriter::logExits = 0;
string logWriter::nameLog = "";
//string logWriter::dateHour;
//int logWriter::sizeProcess;
//string logWriter::name;
//int logWriter::logLevel;
//int logWriter::activeLog;
//int logWriter::maxLogs;
//string logWriter::logNameConfig;
//string logWriter::pathLog;


logWriter::logWriter(string n,int ll,int al,int ml,string lnc,string pl,int msl,bool lr){
	name = n;
	logLevel = ll;
	activeLog = al;
	maxLogs = ml;
	logNameConfig = lnc;
	pathLog = pl;
	maxSizeLog = msl;
	logRotated = true;

	// Create the directories
	Create(pathLog);
}

logWriter::~logWriter(void){}

int logWriter::Create(string name)
{
	if(!std::experimental::filesystem::exists(pathLog))
		std::experimental::filesystem::create_directory(name);

	return 0;
}

int logWriter::Open2read(const char *name,char *textRead,int buf)
{


	//NO TERMINADA


	int fd = open(name,O_RDONLY);

	if(fd < 0)
		return 1;

	if(lseek(fd,0,SEEK_SET))
		return 1;

	//while (!feof(fd))
	//{
		if(read(fd,textRead,buf) < 0)
			return 1;
		//cout<<ftell(fd);
		cout<<textRead;
	//}

	close(fd);

	return 0;
}

int logWriter::Open2write(string name,string textWrite)
{
	// Create the bolt for the file
	// only one process can write in the file
	struct flock bolt; //bolt = cerrojo

	bolt.l_whence = SEEK_SET;
	bolt.l_start = 0;
	bolt.l_len = 0;//All the file
	bolt.l_pid = getpid();

	// Open the file only for writting
//	ifstream(name.c_str(),ifstream::app);
	int fd = open(name.c_str(),O_WRONLY|O_CREAT,0664);// The file have to be create before this step
	if(fd <= 0)
	{
		cout << "[logWriter::Open2write] [ERROR] Error to open the log. Log not found" << endl;
		perror("El error es: ");
		return 1;
	}

	// Catch the bolt
	bolt.l_type = F_WRLCK;
	if(fcntl(fd,F_SETLKW,&bolt) < 0)
	{
		cout << "[logWriter::Open2write] [ERROR] Error with lock the bolt of the log" << endl;
		return 1;
	}

	// Write in the final of the file
	if(lseek(fd,0,SEEK_END) < 0)
	{
		cout << "[logWriter::Open2write] [ERROR] Error with seek_end of the log" << endl;
		return 1;
	}

	// Write in the file
	if(write(fd,textWrite.c_str(),textWrite.size()) < 0)
	{
		cout << "[logWriter::Open2write] [ERROR] Error writig in the log" << endl;
		return 1;
	}

	// Uncatch the bolt
	bolt.l_type = F_UNLCK;
	if(fcntl(fd,F_SETLK,&bolt) < 0)
	{
		cout << "[logWriter::Open2write] [ERROR] Error with unlock the bolt of the log" << endl;
		return 1;
	}

	// Close the file
	close(fd);

	return 0;
}

int logWriter::WriteLog(TRACES trace,string textTrace,string process)
{
	char command[1000];
	char command_terminal[1000];
	string typeMessage;
	string messageColorIn;
	string messageColorOut = "\033[0m";

	// Filter the type of log
	if(trace == 0 && logLevel == 1)
		return 0;

	// Update the hour
	Date();

	// Update the type of Log
	switch(trace)
	{
		case 0:
			typeMessage = "INFO";
			messageColorIn = "\033[32m";
			break;
		case 1:
			typeMessage = "WARNG";
			messageColorIn = "\033[93m";
			break;
		case 2:
			typeMessage = "ERROR";
			messageColorIn = "\033[91m";
			break;
		case 3:
			typeMessage = "DEBUG";
			messageColorIn = "\033[36m";
			break;
		default:
			typeMessage = "ERROR";
			textTrace.append("  The type of Log for writting is incorrect");
			break;
	}

	// Command without colors
	sprintf(command,
		"[%s] [%s] [%s] [%s]\n",
		dateHour.c_str(),
		typeMessage.c_str(),
		process.c_str(),
		textTrace.c_str());

	// If the log exist and her size is more than X created other log
	if(logExits == 1 && logRotated)
	{
		FILE *fich;

		fich=fopen(nameLog.c_str(),"r");

		fseek(fich, 0, SEEK_END);
	//	printf("test.c ocupa %d bytes", ftell(fich));

		if (ftell(fich) > maxSizeLog) // Si se supera el tamaño del log (en bytes) se crea un nuevo log
			SetTitleLog();

		fclose(fich);
	}

	// Establish the name of the log if the log don't exist
	if (logExits == 0)
	{
		SetTitleLog();
		logExits = 1;
	}

	//cout<<"TITULO: "<<nameLog<<endl;

	// Write in the console
	if(activeLog == 0)// Only in the log
	{
		// Command with colors
		sprintf(command_terminal,
			"[%s] %s[%s] [%s] [%s]%s\n",
			dateHour.c_str(),
			messageColorIn.c_str(),
			typeMessage.c_str(),
			process.c_str(),
			textTrace.c_str(),
			messageColorOut.c_str());

		printf("%s",command_terminal);
	}
	// Write inside the selected file
	else if(activeLog == 1)// Only in the log
	{
		if(Open2write(nameLog,command) < 0)
			return 1;
	}
	else if(activeLog == 2)// In the log and in the terminal
	{
		// Command with colors
		sprintf(command_terminal,
			"[%s] %s[%s] [%s] [%s]%s\n",
			dateHour.c_str(),
			messageColorIn.c_str(),
			typeMessage.c_str(),
			process.c_str(),
			textTrace.c_str(),
			messageColorOut.c_str());

		printf("%s",command_terminal);
		if(Open2write(nameLog,command) < 0)
			return 1;
	}

	return 0;
}

int logWriter::Date(void)
{
	time_t tim;
	struct tm date;
	char dateHour[100];

	tim = time(NULL);
	date = *localtime(&tim); // De esta forma se evitan errores

	sprintf(dateHour,"%02d/%02d/%04d %02d:%02d:%02d",
		date.tm_mday,
		date.tm_mon,
		date.tm_year+1900,
		date.tm_hour,
		date.tm_min,
		date.tm_sec);

	SetDateHour(dateHour);

	return 0;
}

int logWriter::SetTitleLog()
{
	time_t tim;
	struct tm date;
	char title[1000];

	tim = time(NULL);
	date = *localtime(&tim);// De esta forma se evitan errores

	sprintf(title,"%s%s%02d%02d%04d_%02d%02d%02d",
		pathLog.c_str(),
		logNameConfig.c_str(),
		date.tm_mday,
		date.tm_mon,
		date.tm_year+1900,
		date.tm_hour,
		date.tm_min,
		date.tm_sec);

	SetNameLog(title);

	return 0;
}

void logWriter::deletelogs()
{
	int numFiles = 0;
	char *buffer;
	FILE *fd;
	long lSize;
	size_t result;

	try{
		if(system("ls logs/ | wc -l 1>.deleteLogs") != 0)
			WriteLog(WARNING_LOG, "Imposible to count the number of logs there are", GetName());

		fd = fopen(".deleteLogs","r");
		if(fd == NULL)
		{
			WriteLog(WARNING_LOG, "The deleteLogs not found", GetName());
			return;
		}

		if(fseek(fd,0,SEEK_END) != 0)
		{
			WriteLog(WARNING_LOG, "The deleteLogs not found", GetName());
			return;
		}
		lSize = ftell(fd);
		rewind(fd);

		buffer = (char*) malloc(sizeof(char)*lSize);
		if(buffer == NULL)
			WriteLog(WARNING_LOG, "Problem during deleted the log", GetName());

		result = fread(buffer,1,lSize,fd);
		if((long int)result != lSize)
			WriteLog(WARNING_LOG, "Problem during deleted the log", GetName());

		numFiles = atoi(buffer);

		if(numFiles >= maxLogs)
		{
			if(system("rm -r logs/* 1>/dev/null") != 0)
			{
				WriteLog(WARNING_LOG, "Imposible clean the  directory of logs", GetName());
			}
			else
			{
				logExits = 0;
				WriteLog(TRACE, "The directory logs are cleaned", GetName());

				fclose(fd);
				free(buffer);

				std::experimental::filesystem::remove(".deleteLogs");

				return;
			}
		}

		char mes[1000];
		snprintf(mes,sizeof(mes),"The files inside the directory are: %d",numFiles);

		WriteLog(DEBUG, mes, GetName());

		fclose(fd);
		free(buffer);

		std::experimental::filesystem::remove(".deleteLogs");
	}
	catch(...)
	{
		cout << "[logWriter::deletelogs] [WARNING] Problem during deleted the log" << endl;
		return;
	}
	return;
}
