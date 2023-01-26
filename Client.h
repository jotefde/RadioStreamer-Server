#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Utils.h"
#include <thread>

using namespace std;

class Client
{
private:
	int _FD;
	Command _command;
	bool _isListeningStream;
	SyncStatus _syncStatus;

public:
	std::thread ClientThread;
	Client(int fd);
	void Listener();
	int GetFD();
	void SetCommand(Command cmd);
	Command GetCommand();
	void Disconnect();
	void Play();
	void Stop();
	bool JoinedStream();
	void ScheduleSync(SyncStatus type = SyncStatus::STANDARD);
	SyncStatus GetSyncStatus();
};