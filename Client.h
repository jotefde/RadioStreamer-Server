#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Utils.h"
#include <thread>
#include <mutex>

using namespace std;

class Client
{
private:
	int _FD;
	Command _command;
	bool _isListeningStream;
	SyncStatus _syncStatus;
	bool _doTerminate = false;
	mutex _writeMutex;
	mutex _readMutex;
	mutex _busyMutex;

public:
	std::thread ListenerThread;
	std::thread NotifierThread;
	Client(int fd);
	int GetFD();
	void SetCommand(Command cmd);
	void Disconnect();
	void Play();
	Command GetCommand();
	void Stop();
	bool JoinedStream();
	void ScheduleSync(SyncStatus type = SyncStatus::STANDARD);
	SyncStatus GetSyncStatus();
	bool IsAlive();
	void LockWriting();
	void UnlockWriting();
	void LockReading();
	void UnlockReading();
};