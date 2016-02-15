#pragma once
#include <string>
#include <curl/curl.h>

class FetchUrlStream {

public:
	FetchUrlStream (std::string url);
	~FetchUrlStream();
	static int curlWriter(char*, int, int, std::string*);
	std::string const & getStream() const { return _curlBuffer; }

private:
	CURL * _curl;
	std::string _curlBuffer;
};
