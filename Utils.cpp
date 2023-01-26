#include "Utils.h"
#include <netinet/in.h>

void Utils::Print(string msg)
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    cout << "[" << now->tm_hour << ":" << now->tm_min << "] " << msg << endl;
}

Message Utils::ParseMessage(const char* package, int packageSize)
{
    Message message;

    // determines if it is data transfer message
    /*if (package[0] == (char) Command::TRANSFER)
    {
        vector<char> data = vector<char>(1024*512 - 1);
        memcpy(data.data(), &package[1], 1024 * 512 - 1);
        message.command = Command::TRANSFER;
        message.data = data;
        return message;
    }*/


    int command;
    memcpy(&command, &package[0], sizeof(int));

    /*int command = Command::END_UPLOAD;
    char commandBytes[sizeof(command)] = command;*/
    //memcpy(commandBytes, &command, sizeof(char));
    try
    {
        message.command = static_cast<Command>(command);
    }
    catch (const std::exception& e)
    {
        Utils::Print(e.what());
        return message;
    }
    char valueBuff[packageSize - sizeof(int)];
    memcpy(valueBuff, package + 4, packageSize);
    string msg(valueBuff);
    size_t cursor = string::npos;

    message.values = map<string, string>();
    while ((cursor = msg.find("=")) != string::npos)
    {
        char key[cursor];
        strncpy(key, msg.c_str(), cursor);
        key[cursor] = '\0';
        msg.erase(0, cursor + 1);

        if ((cursor = msg.find(";")) == string::npos)
            return message;
        char value[cursor];
        strncpy(value, msg.c_str(), cursor);
        value[cursor] = '\0';
        msg.erase(0, cursor + 1);
        message.values.insert(pair<string, string>(key, value));
    }
    return message;
}
