#pragma once
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <sys/time.h>
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
	int _packageSize = 1024 * 750; // 700 kB
	Playlist* _waitingQueue;
	Playlist* _playedQueue;
	Playlist* _uploadQueue;
	Track* _currentTrack = NULL;
	long int _lastEmitTime;
	int _emissionInterruptDuration = 2000; // 300ms
	int _currentTrackSentBytes = 0;
	int _isRequestedNextTrack = false;

	Client* findClient(int);
	void receiveMessage(Client*);
	bool interruptEmission(bool set = false);
	void beginUpload(Client*, map<string, string>);
	void receiveTransfer(Client*, char*, int);
	void endUpload(Client*);
	void sendTrackInfo(Client*);
	void scheduleSyncInfo(bool force = false);
	void removeClient(Client*);
	int calculatePackageSize(int, int);

public:
	Server(long);
	void Run();
	void ClientListener(Client*);
	void ClientNotifier(Client*);
	void AudioEmiter();
	void NextTrack();
	void PrevTrack();
};
