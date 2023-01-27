#include "Playlist.h"

Playlist::Playlist(int trackLimit, bool isReverseMode)
{
	_tracks = vector<Track*>();
	Track* _current = nullptr;
	_trackLimit = trackLimit;
	_isReverseMode = isReverseMode;
}

Track* Playlist::Current()
{
	if(_tracks.empty())
		return nullptr;

	if (_isReverseMode)
		return _tracks.back();
	else
		return _tracks.front();
}

Track* Playlist::PopFront()
{
	if (_tracks.empty())
		return nullptr;
	Track* first = _tracks.front();
	_tracks.erase(_tracks.begin());
	return first;
}

Track* Playlist::PopBack()
{
	if (_tracks.empty())
		return nullptr;
	Track* last = _tracks.back();
	_tracks.pop_back();
	return last;
}

void Playlist::PushFront(Track* track)
{
	if (_tracks.size() >= _trackLimit)
		_tracks.erase(_tracks.end());
	_tracks.insert(_tracks.begin(), track);
}

void Playlist::PushBack(Track* track)
{
	if (_tracks.size() >= _trackLimit)
		_tracks.erase(_tracks.begin());
	_tracks.push_back(track);
}

void Playlist::Remove(Track* track)
{
	auto it = find(_tracks.begin(), _tracks.end(), track);
	_tracks.erase(it);
}

bool Playlist::IsEmpty()
{
	return _tracks.empty();
}

Track* Playlist::GetByClient(Client* client)
{
	Client* sendBy;
	for (auto& track : _tracks)
	{
		sendBy = track->GetSendBy();
		if (track == NULL || sendBy == NULL)
			continue;
		if (sendBy->GetFD() == client->GetFD())
			return track;
	}
	sendBy = nullptr;
	delete sendBy;
	return nullptr;
}
