#include "Track.h"

Track::Track(string title, string fileName, int duration, Client* sendBy)
{
	_title = title;
	_fileName = fileName;
	_duration = duration;
	_sendBy = sendBy;
}

vector<char> Track::LoadData()
{
	std::ifstream file("tracks/" + _fileName, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	vector<char> buff = vector<char>(size);
	if (!file.read(buff.data(), size))
	{
		printf("File cannot be read");
		return vector<char>();
	}
	file.close();
	_fileSize = size;
	return buff;
}

string Track::GetTitle()
{
	return _title;
}

string Track::GetFileName()
{
	return _fileName;
}

long Track::GetFileSize()
{
	return _fileSize;
}

int Track::GetDuration()
{
	return _duration;
}

Client* Track::GetSendBy()
{
	return _sendBy;
}
