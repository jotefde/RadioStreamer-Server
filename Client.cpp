#include "Client.h"

int Client::GetFD()
{
	return _FD;
}

void Client::SetCommand(Command cmd)
{
	_command = cmd;
}

Command Client::GetCommand()
{
	return _command;
}

void Client::Listener()
{
	Utils::Print(to_string(_FD) + "\n");
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
	_isListeningStream = false;
	Utils::Print("Client " + to_string(_FD) + " left the stream");
}

bool Client::JoinedStream()
{
	return _isListeningStream;
}

void Client::ScheduleSync(SyncStatus type)
{
	_syncStatus = type;
}

SyncStatus Client::GetSyncStatus()
{
	return _syncStatus;
}

Client::Client(int fd)
{
	_FD = fd;
	_isListeningStream = false;
}
