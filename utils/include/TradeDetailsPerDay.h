#pragma once
#include <vector>
#include <string>
#include "zdb.h"
#include <ctime>

extern ConnectionPool_T dPool;

class TradeDetailsPerDay {

public:
	TradeDetailsPerDay (std::string _tradeStream, std::string stockCode);
	~TradeDetailsPerDay();
	bool analyzeTradeStream();
	int writeDatabase();
	void releaseConnection();
	int const & getCount() const { return _tradeCount; }

private:
	std::string _tradeStream;
	std::vector<std::string> _tradeItem;
	std::string _stockCode;
	std::tm* _currentTime;
	int _tradeCount;
	Connection_T _conn;
};
