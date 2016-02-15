#pragma once
#include <vector>
#include <string>
#include "zdb.h"

extern ConnectionPool_T dPool;

class FetchStockRealTimeOnce {

public:
	FetchStockRealTimeOnce(std::string realTimeStream, std::string stockCode);
	~FetchStockRealTimeOnce();
	void analyzeRealTimeStream();
	void writeDatabase();
	void releaseConnection();

private:
	std::string _realTimeStream;
	std::string _stockCode;
	Connection_T _conn;
	std::vector<std::string> _realTimeItem;
};
