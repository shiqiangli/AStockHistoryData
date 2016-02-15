#include "FetchUrlStream.h"
#include "ReadStockList.h"
//#include "AnalyzeHistoryOnce.h"
//#include "TradeDetailsPerDay.h"
#include "FetchStockRealTimeOnce.h"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
#include "stdlib.h"
#include "config.h"
#include "zdb.h"
#include "stdio.h"
#include <ctime>
#include <chrono>
#include "ThreadPool.h"

using namespace std::chrono;

typedef std::chrono::duration<int, std::ratio<1, 1>> frame_duration;

ConnectionPool_T dPool;

int main(int argc, char*argv[])
{
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
	//ConnectionPool_setMaxConnections (dPool, MAXDBCONNS);
	ConnectionPool_start(dPool);

	// Initialize Threads Pool
	ThreadPool tPool(MAXTHREADS);

	while (true) {
		auto start_time = std::chrono::system_clock::now();
		
		// Judge if during the trade time (09:30:00-11:30:00, 13:00:00-15:00:00)
		auto end_time = start_time + frame_duration(5);
		auto t = (std::chrono::system_clock::to_time_t(start_time));
		std::vector<std::string> str_t;
		std::string ss = std::ctime(&t);
		boost::split(str_t, ss, boost::is_any_of(" "), boost::token_compress_on);
		std::string currentTime = boost::algorithm::erase_all_copy(str_t[3], ":");
		int temp = stoi(currentTime);
		if (!((temp>OPENAM && temp <CLOSEAM) || (temp>OPENPM && temp<CLOSEPM))) {
			std::cout<<"Market Closed, Waiting..."<<std::endl;
			sleep(4);
			continue;
		}

		for (auto it : stockList) {
			std::string stockCode = it; //"601118";
			std::string url;
			if (atoi(stockCode.c_str()) < 600000)
				url = "http://hq.sinajs.cn/list=sz" + stockCode;
			else	
				url = "http://hq.sinajs.cn/list=sh" + stockCode;
			tPool.enqueue([url, stockCode](){
				FetchUrlStream fus(url);
				if (fus.getStream().size() > 25) { // get url stream successful.
					FetchStockRealTimeOnce fsrto(fus.getStream(), stockCode);
					fsrto.analyzeRealTimeStream();
					fsrto.writeDatabase();
					fsrto.releaseConnection();
				}
			});
		}
		std::cout<<".";
		std::this_thread::sleep_until(end_time);
	}
	
	// free Database Pool
	//ConnectionPool_free(&dPool);
	URL_free(&dbURL);
	
	return 0;
}
