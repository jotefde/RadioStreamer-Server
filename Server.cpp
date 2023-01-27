#include "Server.h"

vector<Client*> Server::_clients = vector<Client*>();

Client* Server::findClient(int fd)
{
    for (Client* client : _clients) 
    {
        if (client->GetFD() == fd)
            return client;
    }
    return nullptr;
}

Server::Server(long port)
{
	_waitingQueue = new Playlist();
	_playedQueue = new Playlist(10, true);
	_uploadQueue = new Playlist(100);

	fs::create_directory("tracks");
	//_currentTrack = new Track("Grove AMP", "groveamp.wav", 94, NULL);
	//_trackBuffer = _currentTrack->LoadData();

	// check if directory is created or not
	if (!fs::exists("tracks"))
	{
		printf("Unable to create track directory\n");
		exit(1);
	}

	_waitingQueue->PushBack(new Track("Notepad", "notepad.wav", 60, nullptr));
	_waitingQueue->PushBack(new Track("Grove Amp", "groveamp.wav", 94, nullptr));


    sockaddr_in myAddr{};
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons((uint16_t)port);

    _FD = socket(PF_INET, SOCK_STREAM, 0);
    if (_FD == -1)
    {
        perror("socket failed");
        return;
    }

    const int enabled = 1;
    if (setsockopt(_FD, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        return;
    }

    if (bind(_FD, (sockaddr*)&myAddr, sizeof(myAddr)) < 0)
    {
        perror("bind failed");
        return;
    }
}

void Server::Run()
{
	struct sockaddr_in address;
	int addrlen = 0;
	int new_socket;

    if (listen(_FD, 3) < 0) {
        perror("listen failed");
        return;
    }

	_emiterThread = thread(&Server::AudioEmiter, this);

    while (true)
    {
		addrlen = sizeof(address);
		if ((new_socket = accept(_FD, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		_clientsMutex.lock();
		Client* newClient = new Client(new_socket);
		newClient->ListenerThread = thread(&Server::ClientListener, this, newClient);
		newClient->NotifierThread = thread(&Server::ClientNotifier, this, newClient);
		//add new socket to array of sockets
		_clients.push_back(newClient);
		_clientsMutex.unlock();
		string msg = "Client "+to_string(new_socket) + " connected (" + string(inet_ntoa(address.sin_addr)) + ":" + to_string(ntohs(address.sin_port)) + ")";
		Utils::Print(msg);
    }

	for (auto& client : _clients)
	{
		client->Disconnect();
	}
	close(_FD);
}

void Server::ClientListener(Client* client)
{
	while (client != NULL && client->IsAlive())
	{
		client->LockReading();
		receiveMessage(client);
		client->UnlockReading();
	}
	removeClient(client);
	terminate();
}

void Server::ClientNotifier(Client* client)
{
	while (client != NULL && client->IsAlive())
	{
		if (client->GetSyncStatus() != SyncStatus::IDLE)
		{
			sendTrackInfo(client);
		}
	}
	terminate();
}

void Server::AudioEmiter()
{
	Client* client;
	vector<char> audioPackage(_packageSize);
	int sentBytes = 0;

	while (true)
	{
		// broadcasting buffered chunk of current track
		// emission speed <= _packageSize / _emissionInterruptDuration
		if (interruptEmission() || _clients.empty())
			continue;

		audioPackage.clear();
		// if it's the last package of current track
		if (_trackBuffer.size() < _packageSize)
		{
			_currentTrackMutex.lock();
			audioPackage = vector<char>(_trackBuffer.begin(), _trackBuffer.begin() + _trackBuffer.size());
			_currentTrackMutex.unlock();
			if (!_waitingQueue->IsEmpty())
			{
				NextTrack();
				scheduleSyncInfo();
			}
			/*
			* TODO:
			*	- Send new Track info to clients
			*/
		}
		else
		{
			_currentTrackMutex.lock();
			copy(_trackBuffer.begin(), _trackBuffer.begin() + _packageSize, back_inserter(audioPackage));
			//audioPackage = vector<char>(_trackBuffer.begin(), _trackBuffer.begin() + _packageSize);
			_trackBuffer = vector<char>(_trackBuffer.begin() + _packageSize, _trackBuffer.end());
			_currentTrackMutex.unlock();
		}

		for (int clientIndex = 0; clientIndex < _clients.size(); clientIndex++)
		{
			client = _clients[clientIndex];
			if (client == NULL || _currentTrack == NULL)
				continue;

			if (client->JoinedStream())
			{
				sentBytes = 0;
				client->LockWriting();
				while (!audioPackage.empty())
				{
					sentBytes = send(client->GetFD(), &audioPackage[0], 1024, 0);
					if (sentBytes > 0)
						audioPackage.erase(audioPackage.begin(), audioPackage.begin() + sentBytes);
					//usleep(100);
				}
				client->UnlockWriting();
			}
		}
	}
}

void Server::NextTrack()
{
	_currentTrackMutex.lock();
	if(_currentTrack != NULL)
		_playedQueue->PushBack(_currentTrack);
	_currentTrack = _waitingQueue->PopFront();
	if (_currentTrack == NULL)
	{
		_trackBuffer = vector<char>();
		return;
	}
	_trackBuffer = _currentTrack->LoadData();
	_currentTrackMutex.unlock();
	Utils::Print("Streaming started: '" + _currentTrack->GetTitle() + "'");
}

void Server::PrevTrack()
{
	_currentTrackMutex.lock();
	if(_currentTrack != NULL)
		_waitingQueue->PushFront(_currentTrack);
	_currentTrack = _playedQueue->PopBack();
	if (_currentTrack == NULL)
	{
		_trackBuffer = vector<char>();
		return;
	}
	_trackBuffer = _currentTrack->LoadData();
	_currentTrackMutex.unlock();
	Utils::Print("Streaming started: '" + _currentTrack->GetTitle() + "'");
}

void Server::scheduleSyncInfo(bool force)
{
	SyncStatus newStatus = force ? SyncStatus::FORCE : SyncStatus::STANDARD;
	Client* client;
	for (int clientIndex = 0; clientIndex < _clients.size(); clientIndex++)
	{
		client = _clients[clientIndex];
		if (client != NULL)
			client->ScheduleSync(newStatus);
	}
	client = nullptr;
	delete client;
}

void Server::removeClient(Client* client)
{
	_clientsMutex.lock();
	_clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
	delete client;
	_clientsMutex.unlock();
}

void Server::sendTrackInfo(Client* client)
{
	int cmd = 0;
	if (client->GetSyncStatus() == SyncStatus::FORCE)
	{
		cmd = Command::INFO_SYNC;
		vector<char> bytes = vector<char>(4);
		memcpy(&bytes[0], &cmd, 4);
		client->LockWriting();
		send(client->GetFD(), &bytes[0], bytes.size(), 0);
		client->UnlockWriting();
		printf("SENT SYNC INFO\n");
		client->ScheduleSync(SyncStatus::STANDARD);
		return;
	}
	stringstream ss;
	ss << "Title=" << _currentTrack->GetTitle() << ";Duration=" << _currentTrack->GetDuration() << ";Index=0;Time=0";
	string str = ss.str();
	cmd = Command::INFO_CURRENT_TRACK;
	vector<char> bytes = vector<char>(str.length() + 4);
	memcpy(&bytes[0], &cmd, 4);
	memcpy(&bytes[0] + 4, str.data(), str.length());
		//vector<char>(str[0], str.length());

	client->LockWriting();
	send(client->GetFD(), &bytes[0], bytes.size(), 0);
	client->UnlockWriting();
	client->ScheduleSync(SyncStatus::IDLE);
	printf("SENT TRACK INFO\n");
}

void Server::receiveMessage(Client* client)
{
	int readCount = 0;
	int buffSize = 1024 + 1;
	Command lastCmd = client->GetCommand();
	if (lastCmd == Command::TRANSFER)
		buffSize = 1024 * 10;
	char buff[buffSize];
	readCount = read(client->GetFD(), buff, buffSize);
	if (readCount == 0)
	{
		client->Disconnect();
		return;
	}

	buff[readCount] = '\0';
	Message message = Utils::ParseMessage(buff, readCount);

	if (lastCmd == Command::TRANSFER && message.command != Command::END_UPLOAD)
		message.command = Command::TRANSFER;

	if (message.command == Command::NONE)
		return;

	switch (message.command)
	{
		case Command::PLAY:
			client->Play();
			client->ScheduleSync(SyncStatus::STANDARD);
			break;
		case Command::STOP:
			client->Stop();
			break;
		case Command::BEGIN_UPLOAD:
			beginUpload(client, message.values);
			break;
		case Command::TRANSFER:
			receiveTransfer(client, buff, readCount);
			break;
		case Command::END_UPLOAD:
			endUpload(client);
			break;
		case Command::NEXT:
			client->SetCommand(Command::NEXT);
			NextTrack();
			scheduleSyncInfo(true);
			break;
		case Command::PREVIOUS:
			client->SetCommand(Command::PREVIOUS);
			PrevTrack();
			scheduleSyncInfo(true);
			break;
	}
}

bool Server::interruptEmission(bool set)
{ 
	if(set)
		_lastEmitTime = time(NULL);
	return _lastEmitTime + _emissionInterruptDuration > time(NULL);
}

void Server::beginUpload(Client* client, map<string, string> values)
{
	Command cmd = client->GetCommand();
	if (cmd == Command::BEGIN_UPLOAD || cmd == Command::TRANSFER)
	{
		Utils::Print("Uploading is already in progress...");
		return;
	}
	if (values.size() != 4)
	{
		Utils::Print("Uploading aborted. Wrong parameters count");
		return;
	}
	client->SetCommand(Command::BEGIN_UPLOAD);
	string title = values["Title"];
	string name = values["FileName"];
	long size = stol(values["FileSize"]);
	int duration = stoi(values["Duration"]);
	Track* track = new Track(title, name, duration, client);
	track->Data = vector<char>();

	_isBusy.lock();
	_uploadQueue->PushBack(track);
	_isBusy.unlock();

	stringstream msg;
	msg << "Client " << client->GetFD() << " started uploading '" << track->GetFileName() << "' (" << size / 1000.0 << " kB)";
	Utils::Print(msg.str());
	client->SetCommand(Command::TRANSFER);
}

void Server::receiveTransfer(Client* client, char* data, int size)
{
	Track* track = _uploadQueue->GetByClient(client);
	if (track == NULL)
	{
		Utils::Print("Uploading aborted. Track does not exists");
		return;
	}
	track->Data.insert(track->Data.end(), data, data + size);
	client->SetCommand(Command::TRANSFER);
}

void Server::endUpload(Client* client)
{
	client->SetCommand(Command::END_UPLOAD);
	Track* track = _uploadQueue->GetByClient(client);
	auto trackFile = std::fstream("tracks/" + track->GetFileName(), std::ios::out | std::ios::binary);
	trackFile.write(track->Data.data(), track->Data.size());
	trackFile.close();
	track->Data.clear();

	_isBusy.lock();
	_waitingQueue->PushBack(track);
	_uploadQueue->Remove(track);
	_isBusy.unlock();

	stringstream msg;
	msg << "Client " << client->GetFD() << " uploaded '" << track->GetFileName() << "' successfully";
	Utils::Print(msg.str());
}