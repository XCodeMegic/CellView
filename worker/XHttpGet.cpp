#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include "XHttpGet.h"

#pragma comment(lib,"Ws2_32.lib")

static char *Requestfrm = "GET %s HTTP/1.1\r\n"
							"Accept-Language:zh_cn\r\n"
							"Host:%s:%d\r\n"
							"Connection:close\r\n"
							"\r\n";


//------------------------------------- XSocketInit --------------------------------
XSocketInit::XSocketInit()
{
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired = MAKEWORD(1, 1);
	WSAStartup(versionRequired, &wsaData);
}
XSocketInit::~XSocketInit()
{
	WSACleanup();
}

//--------------------------------------------------------------------------------------

class XUrl
{
private:
	std::string m_Ip, m_Path;
	UINT	m_port;
public:
	XUrl(std::string &url)
	{
		std::string tmp = url;
		int pos = tmp.find("//");
		tmp = url.substr(pos + 2);

		pos = tmp.find("/");
		//ip and path
		m_Ip = tmp.substr(0, pos);
		m_Path = tmp.substr(pos);

		pos = m_Ip.find(":");
		if (pos > 0 && pos < m_Ip.length())
		{
			std::string strport = m_Ip.substr(pos + 1);
			m_Ip = m_Ip.substr(0, pos);
			m_port = std::atoi(strport.c_str());
		}
		else
			m_port = 80;
	}
public:
	std::string &GetIP()
	{
		return m_Ip;
	}
	std::string &GetPath()
	{
		return m_Path;
	}
	UINT	GetPort() { return m_port; }
};

//---------------------------------------------------------
int GetFileLengthFromHeader(char *header)
{
	char *end = strstr(header, "\r\n\r\n");
	*(end + 2) = 0;
	std::string strHeader(header);
	int pos = strHeader.find("Content-Length");
	std::string Content = strHeader.substr(pos);
	pos = Content.find("\r\n");
	Content = Content.substr(0, pos);
	pos = Content.find(":");
	Content = Content.substr(pos + 1);
	int ret = std::atoi(Content.c_str());
	return ret;
}

int httpGetFile(std::string &url, std::string& saveFileName)
{
	int ret = 0;
	XUrl xurl(url);
	SOCKET	sock = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN sock_in;
	sock_in.sin_addr.S_un.S_addr = inet_addr(xurl.GetIP().c_str());
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(xurl.GetPort());

	char request[256];
	char recvbuf[256];
	char *buffer = new char[4096];
	memset(buffer, 0, 4096);
	int bufindex = 0;
	int needRead = 0;
	int readCount = 0;

	sprintf_s(request, Requestfrm, xurl.GetPath().c_str(), xurl.GetIP().c_str(), xurl.GetPort());

	FILE *f = NULL;
	fopen_s(&f, saveFileName.c_str(), "wb");

	if (f == NULL)
	{
		closesocket(sock);
		return 0;
	}

	try
	{
		if (connect(sock, (SOCKADDR*)&sock_in, sizeof SOCKADDR) == SOCKET_ERROR)
			return 0;
		send(sock, request, strlen(request), 0);

		char *headerFlag = NULL;
		do
		{
			recv(sock, recvbuf, 256, 0);
			memcpy(buffer + bufindex, recvbuf, 256);
			bufindex += 256;
			headerFlag = strstr(buffer, "\r\n\r\n");

		} while (!headerFlag);
		
		needRead = GetFileLengthFromHeader(buffer);

		readCount = bufindex - (headerFlag - buffer + 4);
		//write file
		//like writefile(headerFlag + 4,readCount)
		fwrite(headerFlag + 4, 1, readCount, f);

		while (readCount < needRead)
		{
			int readlen = (needRead - readCount)> 4096 ? 4096 : needRead - readCount;
			readlen = recv(sock, buffer, readlen, 0);

			//write file;
			//like write file(buffer,readlen)
			//
			fwrite(buffer, 1, readlen, f);
			readCount += readlen;
		}
		closesocket(sock);
		fclose(f);

		ret = needRead;
	}
	catch (...)
	{
		delete[] buffer;
		closesocket(sock);
		ret = 0;
	}
	delete[] buffer;
	return ret;
}
