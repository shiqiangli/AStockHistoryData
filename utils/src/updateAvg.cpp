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

ConnectionPool_T dPool;

std::vector<double> computeAvg(std::vector<double> nums, int N) {
	std::vector<double> sum(nums.size()+1, 0);
	std::vector<double> ave(nums.size()+1, 0);
	int i = 0;
    while (i < nums.size() && i < N) {
        sum[i+1] = nums[i]+sum[i];
        ave[i+1] = sum[i+1]/(++i);
	}
    if (i == nums.size()) return ave;
    while (i < nums.size()) {
        sum[i+1] = nums[i]+sum[i]-nums[i-N];
        ave[i+1] = sum[i+1]/N;
        ++i;
	}
	return ave;
}


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
	ConnectionPool_start(dPool);
	Connection_T conn = ConnectionPool_getConnection(dPool);

	for(auto it : stockList) {
		std::vector<double> avg5, avg10, avg20, avg30, avg60;
		std::vector<double> result;
		std::string stockCode = it; //"601118";

		// Query all items in history table.
		std::string sql = "select id, last_close from " + stockCode + "_history";
		ResultSet_T r = Connection_executeQuery(conn, sql.c_str());
		while(ResultSet_next(r)) {
			result.push_back(ResultSet_getDouble(r, 2));
		}

		// Calculate Average numbers and write back to databases.
		avg5 = computeAvg(result, 5);
		avg10 = computeAvg(result, 10);
		avg20 = computeAvg(result, 20);
		avg30 = computeAvg(result, 30);
		avg60 = computeAvg(result, 60);

		for (int i = 1; i <= result.size(); i++) {
			sql = "update " + stockCode + "_history set avg5=" + std::to_string(avg5[i]) + ", avg10=" + std::to_string(avg10[i]) + ", avg20=" + std::to_string(avg20[i]) + ", avg30=" + std::to_string(avg30[i]) + ", avg60=" + std::to_string(avg60[i]) + " where id=" + std::to_string(i);
			//std::cout<<sql<<std::endl;
			Connection_execute(conn, sql.c_str());
		}
		std::cout<<"Update average column in table '"+ stockCode +"_history'."<<std::endl;
	}

	// free Database Pool
	Connection_close(conn);
	ConnectionPool_free(&dPool);
	URL_free(&dbURL);
	
	return 0;
}
