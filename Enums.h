#pragma once
#include <map>
#include <string>
#include <any>
#include <vector>

using namespace std;

enum Command
{
    NONE = 0,
    PLAY = 1337,
    STOP,
    NEXT,
    PREVIOUS,
    SHUTDOWN,

    BEGIN_UPLOAD,
    TRANSFER,
    END_UPLOAD,

    INFO_SYNC,
    INFO_CURRENT_TRACK,
    INFO_PLAYLIST_TRACK,
};

struct Message {
	Command command;
	map<string, string> values;
};

enum SyncStatus
{
    IDLE = 0,
    STANDARD,
    FORCE,
};