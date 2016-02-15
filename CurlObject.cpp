#include "CurlObject.h"
#include <iostream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/locale/encoding.hpp>

CurlObject::CurlObject(std::string url)
{
	curl = curl_easy_init();
      	if(!curl)
        	throw std::string ("Curl did not initialize!");
 
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlObject::curlWriter);
      	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);
      	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      	curl_easy_perform(curl);
}

CurlObject::~CurlObject()
{
	curl_easy_cleanup(curl);
}

int CurlObject::curlWriter(char *data, size_t size, size_t nmemb, std::string *buffer)
{
      int result = 0;
      if (buffer != NULL) {
        buffer->append(data, size * nmemb);
        result = size * nmemb;
      }
      return result;

}

std::vector<std::string> CurlObject::getStockRealtime()
{
	curlBuffer = boost::locale::conv::between(curlBuffer.c_str(), "utf8", "gb2312");;
	boost::split(stockRealtime, curlBuffer, boost::is_any_of(",\""), boost::token_compress_on);

	// delete unnecessary elements
	stockRealtime.erase(stockRealtime.begin()); // "var"
	stockRealtime.pop_back(); // ";"

	return stockRealtime;
}
