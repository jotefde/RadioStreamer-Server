#include "Client.h"

int Client::GetFD()
{
	return _FD;
}

void Client::SetCommand(Command cmd)
{
	_busyMutex.lock();
	_command = cmd;
	_busyMutex.unlock();
}

Command Client::GetCommand()
{
	return _command;
}

void Client::Disconnect()
{
	struct sockaddr_in address {};
	int addrlen = sizeof(address);
	//Somebody disconnected , get his details and print
	getpeername(_FD, (struct sockaddr*)&address, (socklen_t*)&addrlen);
	string msg = "Client "+ to_string(_FD) +" disconnected (" + string(inet_ntoa(address.sin_addr)) + ":" + to_string(ntohs(address.sin_port)) + ")";
	Utils::Print(msg);

	//Close the socket and mark as 0 in list for reuse
	shutdown(_FD, SHUT_RDWR);
	close(_FD);
	_doTerminate = true;
}

void Client::Play()
{
	SetCommand(Command::PLAY);
	_isListeningStream = true;
	Utils::Print("Client " + to_string(_FD) + " joind the stream");
}

void Client::Stop()
{
	SetCommand(Command::STOP);
	_busyMutex.lock();
	_isListeningStream = false;
	_busyMutex.unlock();
	Utils::Print("Client " + to_string(_FD) + " left the stream");
}

bool Client::JoinedStream()
{
	return _isListeningStream;
}

void Client::ScheduleSync(SyncStatus type)
{
	_busyMutex.lock();
	_syncStatus = type;
	_busyMutex.unlock();
}

SyncStatus Client::GetSyncStatus()
{
	return _syncStatus;
}

bool Client::IsAlive()
{
	return !_doTerminate;
}

void Client::LockWriting()
{
	_writeMutex.lock();
}

void Client::UnlockWriting()
{
	_writeMutex.unlock();
}

void Client::LockReading()
{
	_readMutex.lock();
}

void Client::UnlockReading()
{
	_readMutex.unlock();
}

Client::Client(int fd)
{
	_FD = fd;
	_isListeningStream = false;
}
