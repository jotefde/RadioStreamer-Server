#pragma once
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <filesystem>
#include<algorithm>
#include <any>
#include <sstream>
#include "Client.h"
#include "Playlist.h"

using namespace std;
namespace fs = std::filesystem;

class Server
{
private:
	static vector<Client*> _clients;

	int _FD;
	mutex _clientsMutex;
	mutex _currentTrackMutex;
	mutex _isBusy;
	thread _emiterThread;
	vector<char> _trackBuffer;
	int _packageSize = 1024 * 512;
	Playlist* _waitingQueue;
	Playlist* _playedQueue;
	Playlist* _uploadQueue;
	Track* _currentTrack = NULL;
	unsigned long int _lastEmitTime;
	int _emissionInterruptDuration = 1; // 1 sec

	Client* findClient(int);
	void receiveMessage(Client*);
	bool interruptEmission(bool set = true);
	void beginUpload(Client*, map<string, string>);
	void receiveTransfer(Client*, char*, int);
	void endUpload(Client*);
	void sendTrackInfo(Client*);
	void scheduleSyncInfo(bool force = false);
	void removeClient(Client*);

public:
	Server(long);
	void Run();
	void ClientListener(Client*);
	void ClientNotifier(Client*);
	void AudioEmiter();
	void NextTrack();
	void PrevTrack();
};
