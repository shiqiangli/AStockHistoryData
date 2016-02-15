#include <iostream>
#include "FetchUrlStream.h"

FetchUrlStream::FetchUrlStream (std::string url)
{
	_curl = curl_easy_init();
	if(!_curl)
		throw std::string ("Curl does not initialized!");

	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &FetchUrlStream::curlWriter);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_curlBuffer);
	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 60L);
	//curl_easy_setopt(_curl, CURLOPT_PROXY, "2604:a880:1:20::da:1:22:1080");
	CURLcode res = curl_easy_perform(_curl);
	if(res) // !CURLE_OK
		std::cerr << url << ": "<< curl_easy_strerror(res) <<std::endl;;
}

FetchUrlStream::~FetchUrlStream()
{
	curl_easy_cleanup(_curl);
}

int FetchUrlStream::curlWriter(char *data, int size, int nmemb, std::string *buffer)
{
	int result = 0;
	if (buffer != NULL)
	{
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}
