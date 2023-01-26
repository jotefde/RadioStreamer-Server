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
	bool _isReady = false;
	vector<char> _trackBuffer;
	int _packageSize = 1024 * 512;
	Playlist* _waitingQueue;
	Playlist* _playedQueue;
	Playlist* _uploadQueue;
	Track* _currentTrack = nullptr;
	unsigned long int _lastEmitTime;
	int _emissionInterruptDuration = 1; // 1 sec

	Client* findClient(int);
	void emitAudio(Client*, vector<char>);
	void receiveMessage(Client*);
	bool emitInter(bool set = false);
	void beginUpload(Client*, map<string, string>);
	void receiveTransfer(Client*, char*, int);
	void endUpload(Client*);
	void syncTrackInfo();
	void sendTrackInfo(Client*);
	void scheduleSyncInfo(bool force = false);
public:
	static void RemoveClient(Client*);
	Server(long);
	void Run();
	void NextTrack();
	void PrevTrack();
};
