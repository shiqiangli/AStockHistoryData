#ifndef __CURLOBJECT_H_
#include <string>
#include <curl/curl.h>
#include <vector>

class CurlObject{
public:
	CurlObject (std::string url);
	~CurlObject();

	static int curlWriter(char*, size_t, size_t,std::string*);
	std::vector<std::string> getStockRealtime();
private:
	CURL *curl;
	std::string curlBuffer;
	std::vector<std::string> stockRealtime;

};

#endif
