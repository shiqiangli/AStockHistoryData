#include "AnalyzeHistoryOnce.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include "config.h"

AnalyzeHistoryOnce::AnalyzeHistoryOnce (std::string historyStream, std::string stockCode)
{
	this->_historyStream = historyStream;
	this->_writeCount = 0;
	this->_stockCode = stockCode;
	this->_conn = ConnectionPool_getConnection(dPool);
}

AnalyzeHistoryOnce::~AnalyzeHistoryOnce ()
{
}

void AnalyzeHistoryOnce::releaseConnection()
{
	Connection_close(this->_conn);
}

void AnalyzeHistoryOnce::analyzeHistoryStream ()
{
	boost::regex reg("\\s*\\d{8}\\s*,\\s*\\d+.\\d+\\s*,\\s*\\d+.\\d+\\s*,\\s*\\d+.\\d+\\s*,\\s*\\d+.\\d+\\s*,\\s*\\d+.\\d+\\s*,\\s*\\d+\\s*,\\s*\\d+\\s*");
	boost::regex_token_iterator<std::string::iterator> itrBegin(_historyStream.begin(), _historyStream.end(), reg);
	boost::regex_token_iterator<std::string::iterator> itrEnd;
	for (boost::regex_token_iterator<std::string::iterator> itr = itrBegin; itr != itrEnd; ++itr) {
		_historyItems.push_back(*itr); // content: 20150828,6.82,6.91,7.50,6.70,7.50,1160...,1897..
	}
}

void AnalyzeHistoryOnce::writeDatabase()
{
	_writeCount = 0;
	std::vector<std::tm> dbItems;
	std::string sql = "select trade_date from " + _stockCode + "_history";
	ResultSet_T r = Connection_executeQuery(_conn, sql.c_str());
	while (ResultSet_next(r)) {
		std::tm tm0 = ResultSet_getDateTime(r, 1); // get from databse
		tm0.tm_year -= 1900;
		dbItems.push_back(tm0);
	}

	for (std::string item : _historyItems) {
		std::vector<std::string> temp;
		boost::split(temp, item, boost::is_any_of(","), boost::token_compress_on);

		std::tm tm1; // get from url stream
		std::memset(&tm1, 0, sizeof(tm1));
		strptime(temp[0].c_str(), "%Y%m%d", &tm1);
		bool isExist = false;
		for (std::tm tt : dbItems) {
			if(mktime(&tt) == mktime(&tm1)) {
				isExist = true;
				break;
			}
		}

		if (!isExist) { // Record does not exists
			sql = "insert into " + _stockCode + "_history (trade_date, last_close, open, high, low, current, volume, amount) values (?, ?, ?, ?, ?, ?, ?, ?)";
			PreparedStatement_T p = Connection_prepareStatement(_conn, sql.c_str());
			/*
			 * In libzdb/src/db/mysql/MysqlPreparedStatement.c, 'std::tm' is 
			 * converted to 'struct tm' with 'gmtime_r()'(UTC), so we need to
			 * add the time_zone differences in tm1.
			 * It seems has nothing to do with local time zone, but it is a 
			 * temporary method. --by shiqiang, 2015.10.02
			 * TODO: Need a more common method.
			 */
			tm1.tm_hour += TIMEZONE;
			PreparedStatement_setTimestamp(p, 1, mktime(&tm1));
			for (int i = 2; i <= 6; i++) {
				PreparedStatement_setDouble(p, i, stod(temp[i - 1]));
			}
			PreparedStatement_setLLong(p, 7, std::atoll(temp[6].c_str()));
			PreparedStatement_setLLong(p, 8, std::atoll(temp[7].c_str()));
			PreparedStatement_execute(p);
			_writeCount++;
		} else {
			continue;
		}
	}
	std::cout<<"Insert _"<<_writeCount<<"_ records into table '"<<_stockCode<<"_history'.";
}
