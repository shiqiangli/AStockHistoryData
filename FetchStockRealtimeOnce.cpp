#include <iostream>
#include "FetchStockRealtimeOnce.h"

FetchStockRealtimeOnce::FetchStockRealtimeOnce(std::string url, std::string stockCode):co(url)
{
	this->stockCode = stockCode;
	this->conn = ConnectionPool_getConnection(dPool);
}

FetchStockRealtimeOnce::~FetchStockRealtimeOnce()
{
	Connection_close(this->conn);
}

void FetchStockRealtimeOnce::getUrlBuffer()
{
	this->stockRealtime = co.getStockRealtime();
}

void FetchStockRealtimeOnce::generateSQL()
{
	std::string tableAttri = " (code_name, open, close, curprice, high, low, bidprice, auctionprice, volume, amount, buy1num, buy1price, buy2num, buy2price, buy3num, buy3price, buy4num, buy4price, buy5num, buy5price, sell1num, sell1price, sell2num, sell2price, sell3num, sell3price, sell4num, sell4price, sell5num, sell5price, time_date, time_acc, additioninfo) ";
	std::string recordValue = "values ('";
	for (std::vector<std::string>::iterator it = stockRealtime.begin(); it != stockRealtime.end(); ++it)
	{
                if (it == stockRealtime.begin())
                        recordValue = recordValue + *it + "', ";
                else if (it == stockRealtime.end() - 3 || it == stockRealtime.end() - 2)
                        recordValue = recordValue + "'" + *it +"', ";
                else if (it == stockRealtime.end() - 1)
                        recordValue = recordValue + "'" + *it + "'";
                else
                        recordValue = recordValue + *it + ", ";
	}
    recordValue += ")";
    sql = "insert into stock_real_";
	sql = sql + stockCode + tableAttri + recordValue;
    //std::cout<<" sql="<<sql<<std::endl;
}
void FetchStockRealtimeOnce::writeDatabase()
{
	generateSQL();
	PreparedStatement_T p = Connection_prepareStatement(this->conn, sql.c_str());
	PreparedStatement_execute(p);
}


