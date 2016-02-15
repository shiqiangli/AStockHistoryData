#pragma once
#include <vector>
#include <string>
#include "zdb.h"

extern ConnectionPool_T dPool;

class AnalyzeHistoryOnce {

public:
	AnalyzeHistoryOnce (std::string historyStream, std::string stockCode);
	~AnalyzeHistoryOnce();
	void analyzeHistoryStream();
	void writeDatabase();
	void releaseConnection();
	int const & getCount() const { return _writeCount; }

private:
	std::string _historyStream;
	std::string _stockCode;
	std::vector<std::string> _historyItems;
	int _writeCount;
	Connection_T _conn;
};
