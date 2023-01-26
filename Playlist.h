#pragma once
#include "Track.h"

class Playlist
{
private:
	vector<Track*> _tracks;
	int _trackLimit;
	bool _isReverseMode;
public:
	Playlist(int trackLimit = INT32_MAX, bool isReverseMode = false);
	Track* Current();
	Track* PopFront();
	Track* PopBack();
	void PushFront(Track* track);
	void PushBack(Track* track);
	void Remove(Track* track);
	bool IsEmpty();
	Track* GetByClient(Client* client);
};