#pragma once
#include <iostream>
class XSocketInit
{
public:
	XSocketInit();
	~XSocketInit();
};

extern int httpGetFile(std::string &url,std::string& saveFileName);
