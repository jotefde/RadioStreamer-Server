#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Client.h"

using namespace std;

class Track
{
private:
	string _title;
	string _fileName;
	long _fileSize;
	int _duration;
	Client* _sendBy;

public:
	vector<char> Data;
	Track(string title, string fileName, int duration, Client* sendBy);
	vector<char> LoadData();
	string GetTitle();
	string GetFileName();
	long GetFileSize();
	int GetDuration();
	Client* GetSendBy();
};