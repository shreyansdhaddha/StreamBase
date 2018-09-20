#include <Windows.h>
#include <iostream>

#define BUFSIZE 4096
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

bool SendToServer(HANDLE pipe, char * message)
{
	// Write File related
	DWORD nNumberOfBytesToWrite = sizeof(message) * strlen(message);
	LPDWORD lpNumberOfBytesWritten = 0;
	OVERLAPPED isOverlapped;
	bool writeResult = false;

	//Overlapped structure
	memset(&isOverlapped, 0, sizeof(isOverlapped));
	isOverlapped.Offset = 4096;
	isOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	writeResult = WriteFile(pipe, message, nNumberOfBytesToWrite, lpNumberOfBytesWritten, &isOverlapped);
	if (GetLastError() != ERROR_IO_PENDING && HasOverlappedIoCompleted(&isOverlapped))
	{
		if (writeResult)
			cout << "Data Sent: " << reinterpret_cast<char *>(message) << endl;
		else
			cout << "Failed to SEND data" << endl;
	}
	return writeResult;
}

int main()
{
	HANDLE pipe;
	LPCWSTR lpFileName = TEXT("\\\\.\\pipe\\StreamBase");;
	DWORD dwDesiredAccess = GENERIC_WRITE;
	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
	DWORD dwCreationDisposition = OPEN_ALWAYS;
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;
	HANDLE hTemplateFile = NULL;
	
	cout << "CLIENT STARTED" << endl;
	cout << "**************\n\n";

	cout << "Creating File\n" << endl;
	pipe = CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (pipe == INVALID_HANDLE_VALUE || pipe == NULL)
	{
		cout << "Pipe Invalid \n";
		LastError();
		return 0;
	}
	else
		std::cout << "PIPE Found!!\n" << endl;

	//Test cases
	SendToServer(pipe, "This is a Client and Server Project");
	Sleep(2000);
	SendToServer(pipe, "This demonstrates the server receiving the data from the connected client");
	Sleep(2000);

	CloseHandle(pipe);
}