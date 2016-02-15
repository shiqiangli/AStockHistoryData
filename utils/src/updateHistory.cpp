#include "FetchUrlStream.h"
#include "ReadStockList.h"
#include "AnalyzeHistoryOnce.h"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "stdlib.h"
#include "config.h"
#include "zdb.h"
#include "stdio.h"
#include "ctime"

ConnectionPool_T dPool;

int main(int argc, char*argv[])
{
	time_t totalStart, totalStop;
	totalStart = clock();
	// get stock list filename
	if(argc < 2) {
		std::cout<<"Please add stocklist filename as argument."<<std::endl;
		return 1;
	}
	std::string fileName(argv[1]);
	//std::string fileName = "../stockLists/shA.full.201509";
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
	time_t itemStart, itemStop;
	int i = 0;
	for (auto it : stockList) {
		i++;
		itemStart = clock();
		std::string stockCode = it;
		std::string url;
		if (atoi(stockCode.c_str()) < 600000 && atoi(stockCode.c_str()) != 1) // SZ
			url = "http://flashquote.stock.hexun.com/Quotejs/DA/2_" + stockCode + "_DA.html";
		else
			url = "http://flashquote.stock.hexun.com/Quotejs/DA/1_" + stockCode + "_DA.html";
		FetchUrlStream fus(url);
		AnalyzeHistoryOnce aho(fus.getStream(), stockCode);
		aho.analyzeHistoryStream();
		aho.writeDatabase();
		aho.releaseConnection();
		if (aho.getCount()) countTable++;
		countItem += aho.getCount();
		itemStop = clock();
		std::cout<<"    consumed "<<(double)(itemStop - itemStart)/CLOCKS_PER_SEC<<" s.    "<<std::endl;
		if(i == 20) {// Every NUM stocks, sleep several seconds, avoid frequent access.
			std::cout<<"Sleep for 60 seconds..."<<std::endl;
			sleep(60);
			i = 0;
		}
	}
	totalStop = clock();
	std::cout<<"update "<<countTable<<" tables; "<<countItem<<" records, "<<std::endl;
	std::cout<<"consumed "<<(double)(totalStop - totalStart)/CLOCKS_PER_SEC<<" s."<<std::endl;

	// free Database Pool
	ConnectionPool_free(&dPool);
	//std::cout<<"122"<<std::endl;
	URL_free(&dbURL);
	//std::cout<<"1444"<<std::endl;
	return 0;
}
