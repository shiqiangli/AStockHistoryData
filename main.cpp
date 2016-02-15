#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include "config.h"
#include "ThreadPool.h"
#include "StockList.h"
#include "zdb.h"
#include "FetchStockRealtimeOnce.h"
#include "stdlib.h" // atoi()

using namespace std::chrono;

typedef std::chrono::duration<int, std::ratio<1, 1>> frame_duration;

//volatile ConnectionPool_T dPool;
ConnectionPool_T dPool;

int main(int argc, char *argv[])
{
	// get stock list filename.
	if (argc < 2) {
		std::cout<<"Please add stockList filename as argument."<<std::endl;
		return 1;
	}
	//std::string path(argv[1]);
	//std::string fileName(path.substr(path.find_last_of('/') + 1));
	std::string fileName(argv[1]);

	// Constant Configuration
    std::string urlHead(URLHEAD);
	std::string market(MARKETSH);
	std::string dbip(DBIP),dbPort(DBPORT), dbUser(DBUSER), dbPasswd(DBPASSWD), dbName(DBNAME);
	std::string dbPath = "mysql://" + dbUser + ":" + dbPasswd + "@" + dbip + ":" + dbPort + "/" + dbName;
	const char * c = dbPath.c_str();
         
	// Get Stock List
	StockList sl(fileName);
	std::vector<std::string> stockList = sl.getStockList();
	
	// Initialize Threads Pool
	ThreadPool tPool(MAXTHREADS);

	// Initialize Database Connections Pool
	URL_T dbURL = URL_new(c);
	//ConnectionPool_T dPool = ConnectionPool_new(dbURL);
	dPool = ConnectionPool_new(dbURL);
	ConnectionPool_setMaxConnections (dPool, MAXDBCONNS);
	ConnectionPool_start(dPool);

	while (true)
	{
		auto start_time = std::chrono::system_clock::now();
		auto end_time = start_time + frame_duration(5);
		auto t = (std::chrono::system_clock::to_time_t(start_time));
		std::cout <<std::ctime(&t);

		std::chrono::time_point<std::chrono::high_resolution_clock> m_begin = std::chrono::high_resolution_clock::now();
		for(std::vector<std::string>::iterator it = stockList.begin(); it != stockList.end(); it++)
		{
			std::string stockCode = *it;
			if(atoi(stockCode.c_str()) < 600000) // SZ
				market = MARKETSZ;
			else
				market = MARKETSH;
			std::string url = urlHead + market + stockCode;

			//std::cout<<typeid(stockCode).name()<<std::endl;
			tPool.enqueue([url, stockCode](){
				std::cout<<pthread_self()<<"--process stock:  "<<stockCode<<std::endl;
				FetchStockRealtimeOnce fsro(url, stockCode);
				fsro.getUrlBuffer();
				fsro.writeDatabase();
			});
		}
		long t1 = (duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - m_begin).count());
		//std::cout<<"Time Consumed Every Fetch: "<< t1<<" ms."<<std::endl;
		std::cout<<std::endl;
		std::this_thread::sleep_until(end_time);
	}
	//std::cout<<pthread_self()<<" called(main)."<<std::endl;
	//ConnectionPool_free(&dPool);
	//ConnectionPool_free(&((ConnectionPool_T)dPool));
	URL_free(&dbURL);

	return 0;
}
