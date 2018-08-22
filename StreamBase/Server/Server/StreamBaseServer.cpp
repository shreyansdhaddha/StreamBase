#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

#define BUFSIZE 4096
#define SYNCRONIZE FALSE

using namespace std;

int LastError()
{
	switch (GetLastError())
	{
	case ERROR_FILE_NOT_FOUND:
		cout << "ERROR: ERROR_FILE_NOT_FOUND" << endl;
		return 1;
	case ERROR_INVALID_HANDLE:
		cout << "ERROR: ERROR_INVALID_HANDLE" << endl;
		return 1;
	case ERROR_NO_DATA:
		cout << "ERROR: ERROR_NO_DATA" << endl;
		return 1;
	case ERROR_PIPE_NOT_CONNECTED:
		cout << "ERROR: ERROR_PIPE_NOT_CONNECTED" << endl;
		return 1;
	case ERROR_PIPE_LISTENING:
		cout << "ERROR: ERROR_PIPE_LISTENING" << endl;
		return 1;
	case ERROR_IO_PENDING:
		cout << "ERROR: ERROR_IO_PENDING" << endl;
		return 1;
	case ERROR_NO_DATA_DETECTED:
		cout << "ERROR: ERROR_NO_DATA_DETECTED" << endl;
		return 1;
	case ERROR_PIPE_CONNECTED:
		cout << "ERROR: ERROR_PIPE_CONNECTED" << endl;
		return -1;
	default:
		cout << "ERROR: (" << GetLastError() << ")" << endl;
		return 0;
	}
}

void printRetrievedCollection(vector<string> retrievedCollection)
{
	cout << "PRINTING COLLECTION" << endl;
	for (string message : retrievedCollection) 
	{
		cout << message << endl;
	}
}

int main()
{
	//Pipe related fields
	HANDLE pipe;
	LPCWSTR lpName = TEXT("\\\\.\\pipe\\StreamBase");
	DWORD dwOpenMode = PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED;
	DWORD dwPipeMode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | ((SYNCRONIZE) ? PIPE_WAIT : PIPE_NOWAIT);
	DWORD nMaxInstances = PIPE_UNLIMITED_INSTANCES;
	DWORD nOutBufferSize = BUFSIZE * sizeof(char *);
	DWORD nInBufferSize = BUFSIZE * sizeof(char *);
	DWORD nDefaultTimeOut = NMPWAIT_USE_DEFAULT_WAIT;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;

	//Connection related fields
	OVERLAPPED isOverlapped; // Need to support this. Should have multiple Events array to allow multiple clients.
	bool connected = false;

	//Read File related
	char * buffer[1024];
	DWORD nNumberOfBytesToRead = sizeof(char*) * 300;
	DWORD lpNumberOfBytesRead = 0;
	bool readResult = false;

	//Overlapped structure
	memset(&isOverlapped, 0, sizeof(isOverlapped));
	isOverlapped.Offset = 4096;
	isOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//Data Structure - Storage (Should be a custom object instead of string)
	vector<string> retrievedCollection;


	cout << "SERVER STARTED" << endl;
	cout << "**************\n\n";

	cout << "Creating Named Pipe" << endl;
	pipe = CreateNamedPipe(lpName, dwOpenMode, dwPipeMode, nMaxInstances, nOutBufferSize, nInBufferSize, nDefaultTimeOut, lpSecurityAttributes);
	if (pipe == INVALID_HANDLE_VALUE || pipe == NULL)
	{
		cout << "Connect Named Pipe Failed!! \n";
		LastError();
		return 0;
	}
	else
		cout << "PIPE Created!!" << endl;

	cout << "\nConnecting Named Pipe" << endl;
	connected = ConnectNamedPipe(pipe, &isOverlapped);
	cout << "Connect Named Pipe Started" << endl;

	while (true)
	{
		readResult = ReadFile(pipe, buffer, nNumberOfBytesToRead, &lpNumberOfBytesRead, &isOverlapped);
		if (GetLastError() != ERROR_IO_PENDING && HasOverlappedIoCompleted(&isOverlapped))
		{
			if (readResult)
			{
				buffer[lpNumberOfBytesRead] = '\0';
				char* data = reinterpret_cast<char *>(buffer);
				cout << "\n*Received New Data*\n" << endl;
				retrievedCollection.push_back(string(data));

				printRetrievedCollection(retrievedCollection);
			}
		}
	}
}