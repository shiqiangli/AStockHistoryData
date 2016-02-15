//#include "FetchUrlStream.h"
#include "ReadStockList.h"
#include "AnalyzeHistoryOnce.h"
//#include "TradeDetailsPerDay.h"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "stdlib.h"
#include "config.h"
#include "zdb.h"
#include "stdio.h"
#include <time.h>
#include <iomanip>
#include <ctime>
#include <stdio.h>

ConnectionPool_T dPool;

int main(int argc, char*argv[])
{
	std::string str = "20150911";
	std::tm tm1;
	strptime(str.c_str(), "%Y%m%d", &tm1);
	tm1.tm_hour = tm1.tm_min = tm1.tm_sec = 0;
	time_t t = mktime(&tm1);
	//sscanf(str.c_str(),"%4d%2d%2d",&tm1.tm_year,&tm1.tm_mon,&tm1.tm_mday);
	//tm1.tm_year -= 1900;
	//tm1.tm_mon -= 1;
	//tm1.tm_hour = tm1.tm_min = tm1.tm_sec = 0;
	std::cout<<tm1.tm_year<<" "<<tm1.tm_mon<<" "<<tm1.tm_mday<<" "<<t<<std::endl;
	//time_t time = mktime(&tm1);
	//std::cout<<asctime(localtime(&time))<<std::endl;
	// get stock list filename
/*	if(argc < 2) {
		std::cout<<"Please add stocklist filename as argument."<<std::endl;
		return 1;
	}
	std::string fileName(argv[1]);
//	std::string fileName = "../stockLists/shA.full.201509";
	ReadStockList rdsl(fileName);
	std::vector<std::string> stockList = rdsl.getStockList();
*/
	// Initioalize Database Connection Pool
	std::string dbip(DBIP),dbPort(DBPORT), dbUser(DBUSER), dbPasswd(DBPASSWD), dbName(DBNAME);
	std::string dbPath = "mysql://" + dbUser + ":" + dbPasswd + "@" + dbip + ":" + dbPort + "/" + dbName;
	const char * c = dbPath.c_str();
	URL_T dbURL = URL_new(c);
	dPool = ConnectionPool_new(dbURL);
	ConnectionPool_start(dPool);
	Connection_T conn = ConnectionPool_getConnection(dPool);

	int countTable = 0;
	long countItem = 0;
//	for (auto it : stockList) {
		std::string stockCode = "601118";
		std::string sql = "select * from "+stockCode+"_history order by trade_date desc limit 1";
		ResultSet_T r = Connection_executeQuery(conn, sql.c_str());
		double today_price;
		std::string today_date;
		std::string today_id;
		while (ResultSet_next(r)){
			today_id = ResultSet_getString(r, 1);
			today_date = ResultSet_getString(r, 2);
			today_price = ResultSet_getDouble(r,7);
		}
		double min_price;
		std::string min_date;
		if(today_id.size()) {
		sql = "select * from "+stockCode+"_history where id="+today_id+" and current > avg20";
		r = Connection_executeQuery(conn, sql.c_str());
		while (ResultSet_next(r)) {
			std::cout<<"current > avg20: "<<stockCode<<std::endl;
			//min_date = ResultSet_getString(r, 1);
			//min_price = ResultSet_getDouble(r,3);
		}
		}
		if(today_price <= min_price) {
			countTable++;
			//std::cout<<stockCode<<std::endl;
			//std::cout<<" current:"<<today_price<<" time:"<<today_date<<std::endl;
			//std::cout<<" min:"<<min_price<<" time:"<<min_date<<std::endl;
		}
//	}
	//std::cout<<countTable<<std::endl;

	Connection_close(conn);
	// free Database Pool
	ConnectionPool_free(&dPool);
	URL_free(&dbURL);
	return 0;
}
