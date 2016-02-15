#include "TradeDetailsPerDay.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "config.h"

TradeDetailsPerDay::TradeDetailsPerDay (std::string tradeStream, std::string stockCode)
{
	this->_tradeStream = tradeStream;
	this->_tradeCount = 0;
	this->_stockCode = stockCode;
	this->_conn = ConnectionPool_getConnection(dPool);
	time_t t = time(0);
	_currentTime = localtime(&t);
	_currentTime->tm_hour = _currentTime->tm_min = _currentTime->tm_sec = 0; // we just need day, not accurate time.
	//_currentTime->tm_mday -= 1;
}

TradeDetailsPerDay::~TradeDetailsPerDay()
{
}

void TradeDetailsPerDay::releaseConnection()
{
	Connection_close(this->_conn);
}

bool TradeDetailsPerDay::analyzeTradeStream ()
{
	boost::regex reg("\'\\s*\\d{2}:\\d{2}:\\d{2}\\s*\',\\s+\'\\s*\\d+\\s*\',\\s+\'\\s*\\d+.\\d+\\s*\',\\s+\'\\s*\\w+\\s*\'");
	boost::regex_token_iterator<std::string::iterator> itrBegin(_tradeStream.begin(), _tradeStream.end(), reg);
	boost::regex_token_iterator<std::string::iterator> itrEnd;
	for (boost::regex_token_iterator<std::string::iterator> itr = itrBegin; itr != itrEnd; ++itr) {
		_tradeItem.push_back(*itr);
	}

	return true;
}

int TradeDetailsPerDay::writeDatabase()
{
	if (_tradeItem.size() == 0)
		return 0;
	_tradeCount = 0;

	std::vector<time_t> dbItems;
	char buf[11];
	sprintf(buf, "%4d-%02d-%02d", _currentTime->tm_year+1900, _currentTime->tm_mon+1, _currentTime->tm_mday);
	std::string day = buf;
	std::string sql = "select trade_time from " + _stockCode + "_trade where trade_time regexp '^" + day + "'";
	ResultSet_T r = Connection_executeQuery(_conn, sql.c_str());
	while (ResultSet_next(r)) {
		time_t tm0 = ResultSet_getTimestamp(r, 1);
		dbItems.push_back(tm0);
	}

	for (std::vector<std::string>::reverse_iterator item = _tradeItem.rbegin(); item != _tradeItem.rend(); ++item)
	{
		std::vector<std::string> temp;
		boost::split(temp, *item, boost::is_any_of(",' "), boost::token_compress_on); //temp: null, trade_time, volume, price, type, null

		std::tm tm1;
		std::memset(&tm1, 0, sizeof(tm1));
		strptime(temp[1].c_str(), "%H:%M:%S", &tm1);
		tm1.tm_year = _currentTime->tm_year;
		tm1.tm_mday = _currentTime->tm_mday;
		tm1.tm_mon = _currentTime->tm_mon;
		tm1.tm_hour += TIMEZONE; // see comments in ./src/analyzeHistoryOnce.cpp
		bool isExist = false;
		for (time_t tt : dbItems) {
			if (tt == mktime(&tm1)) {
				isExist = true;
				break;
			}
		}

		if (!isExist) {
			// trade_time, volume, price, type, amount
			sql = "insert into " + _stockCode + "_trade (trade_time, volume, price, type, amount) values (?, ?, ?, ?, ?)";
			PreparedStatement_T p = Connection_prepareStatement(_conn, sql.c_str());
			PreparedStatement_setTimestamp(p, 1, mktime(&tm1)); // trade_time
			PreparedStatement_setInt(p, 2, stod(temp[2])); // volume
			PreparedStatement_setDouble(p, 3, stod(temp[3])); // price
			std::string type = "0"; // equal
			if (temp[4] == "DOWN") // sell
				type = "-1";
			else if (temp[4] == "UP") // buy
				type = "1";
			PreparedStatement_setInt(p, 4, stod(type)); // type
			double amount = ((double)((int)(stod(temp[2]) * stod(temp[3]) * 100))) / 100;
			PreparedStatement_setDouble(p, 5, amount); // amount
			PreparedStatement_execute(p);
			_tradeCount++;
		} else {
			continue;
		}
	}
	std::cout<<"Insert _"<<_tradeCount<<"_ records into table '"<<_stockCode<<"_trade'.";
	return 1;
}
