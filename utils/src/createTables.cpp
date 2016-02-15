#include "FetchUrlStream.h"
#include "ReadStockList.h"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "stdlib.h"
#include "config.h"
#include "zdb.h"
#include "stdio.h"

int main(int argc, char*argv[])
{
	if (argc < 2) {
		std::cout<<"Please add stock list file name as argument."<<std::endl;
		return 1;
	}
	std::string fileName = argv[1];
	ReadStockList rdsl(fileName);
	std::vector<std::string> stockList = rdsl.getStockList();

	std::string dbip(DBIP),dbPort(DBPORT), dbUser(DBUSER), dbPasswd(DBPASSWD),     dbName(DBNAME);
	std::string dbPath = "mysql://" + dbUser + ":" + dbPasswd + "@" + dbip + ":" + dbPort + "/" + dbName;
	const char * c = dbPath.c_str();
	URL_T dbURL = URL_new(c);
	ConnectionPool_T dPool = ConnectionPool_new(dbURL);
	ConnectionPool_start(dPool);
	
	for(auto it : stockList)
	{
		std::string sql_hist = "create table if not exists ";
		sql_hist = sql_hist + it;
		sql_hist = sql_hist + "_history (id smallint(5) unsigned primary key auto_increment, trade_date date, last_close float, open float, high float, low float, current float, volume bigint, amount bigint, avg5 float, avg10 float, avg20 float, avg30 float, avg60 float)";
		std::string sql_base = "create table if not exists " + it + "_base (code_id varchar(6), code_name varchar(20))";
		std::string sql_realtime = "create table if not exists " + it + "_realtime (id mediumint unsigned primary key auto_increment, open float, last_close float, current float, high float, low float, volume int unsigned, amount bigint unsigned, buy1num int unsigned, buy1price float, buy2num int unsigned, buy2price float, buy3num int unsigned, buy3price float, buy4num int unsigned, buy4price float, buy5num int unsigned, buy5price float, sell1num int unsigned, sell1price float, sell2num int unsigned, sell2price float, sell3num int unsigned, sell3price float, sell4num int unsigned, sell4price float, sell5num int unsigned, sell5price float, trade_date date, trade_time Time)";
		std::string sql_trade = "create table if not exists " + it + "_trade (id mediumint unsigned primary key auto_increment, trade_time datetime, volume int unsigned, price float, type tinyint, amount float)"; // -1--sell, 0--equal, 1--buy
		Connection_T con = ConnectionPool_getConnection(dPool);
		TRY{
			Connection_execute(con, sql_hist.c_str());
			//Connection_execute(con, sql_base.c_str());
			Connection_execute(con, sql_realtime.c_str());
			Connection_execute(con, sql_trade.c_str());
		} CATCH (SQLException) {
			std::cout<<"SQLException -- "<<Exception_frame.message<<std::endl;
		} FINALLY {
			Connection_close(con);
		} END_TRY;
		std::cout<<sql_hist<<std::endl;
		//std::cout<<sql_base<<std::endl;
		std::cout<<sql_realtime<<std::endl;
		std::cout<<sql_trade<<std::endl;
		std::cout<<std::endl;
	}
	ConnectionPool_free(&dPool);
	URL_free(&dbURL);

	return 0;
}
