#include "FetchStockRealTimeOnce.h"
#include "config.h"
#include <iostream>
#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/locale/encoding.hpp"
#include "boost/algorithm/string.hpp"
#include <chrono>
#include <ctime>

FetchStockRealTimeOnce::FetchStockRealTimeOnce (std::string realTimeStream, std::string stockCode) {
	this->_realTimeStream = realTimeStream;
	this->_stockCode = stockCode;
	this->_conn = ConnectionPool_getConnection(dPool);
}

FetchStockRealTimeOnce::~FetchStockRealTimeOnce () {}

void FetchStockRealTimeOnce::releaseConnection()
{
	Connection_close(this->_conn);
}

void FetchStockRealTimeOnce::analyzeRealTimeStream() {
	//_realTimeStream = boost::locale::conv::between(_realTimeStream.c_str(), "utf8", "gb2312");
	boost::split(_realTimeItem, _realTimeStream, boost::is_any_of(",\""), boost::token_compress_on);

}

void FetchStockRealTimeOnce::writeDatabase() {
	if (_realTimeItem.size() < 30) return;
	std::string sql = "insert into " + _stockCode + "_realtime (open, last_close, current, high, low, volume, amount, buy1num, buy1price, buy2num, buy2price, buy3num, buy3price, buy4num, buy4price, buy5num, buy5price, sell1num, sell1price, sell2num, sell2price, sell3num, sell3price, sell4num, sell4price, sell5num, sell5price, trade_date, trade_time) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	PreparedStatement_T p = Connection_prepareStatement(_conn, sql.c_str());
	int p_idx = 1, a_idx = 2;
	for (int j = 0; j < 5; j++) { // open, last_close, current, high, low
		PreparedStatement_setDouble(p, p_idx, stod(_realTimeItem[a_idx]));
		p_idx++; a_idx++;
	}
	a_idx += 2; // skip two elements: buy_price, sell_price
	PreparedStatement_setInt(p, p_idx++, stod(_realTimeItem[a_idx++])); // volume
	PreparedStatement_setDouble(p, p_idx++, stod(_realTimeItem[a_idx++])); // amount
	for (int j = 0; j < 10; j++) { // buy1num~bu5num, buy1price~buy5price (sell...)
		PreparedStatement_setInt(p, p_idx++, stod(_realTimeItem[a_idx]));
		PreparedStatement_setDouble(p, p_idx++, stod(_realTimeItem[a_idx + 1]));
		a_idx += 2;
	}
	std::tm tm1;
	std::memset(&tm1, 0, sizeof(tm1));
	strptime(_realTimeItem[a_idx++].c_str(), "%Y-%m-%d", &tm1);
	tm1.tm_hour += TIMEZONE;
	PreparedStatement_setTimestamp(p, p_idx++, mktime(&tm1)); // trade_date
	std::memset(&tm1, 0, sizeof(tm1));
	strptime(_realTimeItem[a_idx++].c_str(), "%H:%M:%S", &tm1);
	tm1.tm_year += 1970;
	tm1.tm_hour += TIMEZONE; // see comments in ./src/analyzeHistoryOnce.cpp
	PreparedStatement_setTimestamp(p, p_idx++, mktime(&tm1)); // trade_time
	PreparedStatement_execute(p);
}
