#pragma once
#include <string>
#include <sstream>
#include "includes/json.hpp"

using namespace nlohmann;


#include <Windows.h>
#include <string>
#include <iostream>
#include <Wininet.h>
#include <string>
#pragma comment(lib, "wininet.lib")


std::string replaceAll(std::string subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::string DownloadString(std::string URL) {//Thanks https://www.unknowncheats.me/forum/c-and-c-/337204-download-string-url.html
	HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	HINTERNET urlFile;
	std::string rtn;
	if (interwebs) {
		urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
		if (urlFile) {
			char buffer[2000];
			DWORD bytesRead;
			do {
				InternetReadFile(urlFile, buffer, 2000, &bytesRead);
				rtn.append(buffer, bytesRead);
				memset(buffer, 0, 2000);
			} while (bytesRead);
			InternetCloseHandle(interwebs);
			InternetCloseHandle(urlFile);
			std::string p = replaceAll(rtn, "|n", "\r\n");
			return p;
		}
	}
	InternetCloseHandle(interwebs);
	std::string p = replaceAll(rtn, "|n", "\r\n");
	return p;
}

json GetcsgoOffsets()
{
	std::string hazedumperjson = DownloadString("https://raw.githubusercontent.com/frk1/hazedumper/master/csgo.min.json");
	return json::parse(hazedumperjson);
}
