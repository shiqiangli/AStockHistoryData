#include "FetchUrlStream.h"
#include "ReadStockList.h"
#include "AnalyzeHistoryOnce.h"
#include "TradeDetailsPerDay.h"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "stdlib.h"
#include "config.h"
#include "zdb.h"
#include "stdio.h"
#include <sys/time.h>

ConnectionPool_T dPool;

int main(int argc, char*argv[])
{
	timeval totalStart, totalStop;
	gettimeofday(&totalStart, NULL);
	// get stock list filename
	if(argc < 2) {
		std::cout<<"Please add stocklist filename as argument."<<std::endl;
		return 1;
	}
	std::string fileName(argv[1]);
//	std::string fileName = "../stockLists/shA.full.201509";
	ReadStockList rdsl(fileName);
	std::vector<std::string> stockList = rdsl.getStockList();

	// Initioalize Database Connection Pool
	std::string dbip(DBIP),dbPort(DBPORT), dbUser(DBUSER), dbPasswd(DBPASSWD), dbName(DBNAME);
	std::string dbPath = "mysql://" + dbUser + ":" + dbPasswd + "@" + dbip + ":" + dbPort + "/" + dbName;
	const char * c = dbPath.c_str();
	URL_T dbURL = URL_new(c);
	dPool = ConnectionPool_new(dbURL);
	ConnectionPool_start(dPool);

	int countTable = 0;
	long countItem = 0;
	int countUnresolved = 0;
	std::vector<std::string> unresolvedList;
	timeval itemStart, itemStop;
	long mtime, seconds, useconds;
	int i = 0;
	for (auto it : stockList) {
		i++;
		gettimeofday(&itemStart, NULL);
		std::string stockCode = it;//"601118";
		std::string url;
		if (atoi(stockCode.c_str()) < 600000) // SZ
			url = "http://vip.stock.finance.sina.com.cn/quotes_service/view/CN_TransListV2.php?num=100000&symbol=sz" + stockCode;
		else
			url = "http://vip.stock.finance.sina.com.cn/quotes_service/view/CN_TransListV2.php?num=100000&symbol=sh" + stockCode;
		FetchUrlStream fus(url);
		TradeDetailsPerDay tdpd(fus.getStream(), stockCode);
		tdpd.analyzeTradeStream();
		if (tdpd.writeDatabase() == 0) { // get url stream failed.
			std::cout<<"Parser "<<stockCode<<" failed, skip.."<<std::endl;
			countUnresolved++;
			unresolvedList.push_back(stockCode);
			continue;
		}
		tdpd.releaseConnection();
		if (tdpd.getCount()) countTable++;
		countItem += tdpd.getCount();
		gettimeofday(&itemStop, NULL);
		seconds = itemStop.tv_sec - itemStart.tv_sec;
		useconds = itemStop.tv_usec - itemStart.tv_usec;
		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
		std::cout<<"   consumed "<<(double)mtime/1000000<<" s.    "<<std::endl;
		sleep(2);
		if(i == 20) { // Every NUM stocks, sleep several seconds, avoid frequent access.
			std::cout<<"Sleep for 120 seconds..."<<std::endl;
			sleep(120);
			i = 0;
		}
	}
	std::cout<<"Unresolved Stocks:"<<std::endl;
	for (auto un : unresolvedList)
		std::cout<<un<<std::endl;

	gettimeofday(&totalStop, NULL);
	seconds = totalStop.tv_sec - totalStart.tv_sec;
	useconds = totalStop.tv_usec - totalStart.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	std::cout<<"update "<<countTable<<" tables; "<<countItem<<" records, ";
	std::cout<<"consumed "<<(double)mtime/1000000<<" s."<<std::endl;

	// free Database Pool
	ConnectionPool_free(&dPool);
	URL_free(&dbURL);
	
	return 0;
}
