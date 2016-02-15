#ifndef __FETCHSTOCKREALTIMEONCE_H
#define __FETCHSTOCKREALTIMEONCE_H

#include <string>
#include <vector>
#include <zdb.h>
#include "CurlObject.h"

//extern volatile ConnectionPool_T dPool;
extern ConnectionPool_T dPool;

class FetchStockRealtimeOnce
{
public:
	FetchStockRealtimeOnce(std::string url, std::string stockCode);
	~FetchStockRealtimeOnce();
	void getUrlBuffer();
	void generateSQL();
	void writeDatabase();

private:
	CurlObject co;
	Connection_T conn;
	std::vector<std::string> stockRealtime;
	std::string sql;
	std::string stockCode;
};


#endif
