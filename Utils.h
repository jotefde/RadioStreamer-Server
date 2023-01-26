#pragma once
#include <iostream>
#include <ctime>
#include <string>
#include <string.h>
#include "Enums.h"

using namespace std;

class Utils
{
public:
	static void Print(string msg);
	static Message ParseMessage(const char* package, int packageSize);
};