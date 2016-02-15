#include <iostream>
#include <fstream>
#include "stdlib.h"
#include "config.h"
#include "zdb.h"
#include "stdio.h"
#include "ReadStockList.h"

ConnectionPool_T dPool;

// return profit percentage.
double simpleAvg20(std::vector<std::string>, std::vector<double>, std::vector<double>);

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

	int totalNum = 0;
	int posNum = 0;
	std::cout<<"20140916 ~ 20150916: "<<std::endl;
	for(auto it : stockList) {
		std::vector<std::string> date;
		std::vector<double> avg20;
		std::vector<double> current;
		std::string stockCode = it; //"601118";

		// Query all items in history table.
		std::string sql = "select trade_date,current,avg20 from " + stockCode + "_history where trade_date>'20140916' and trade_date<'20150916'";
		//std::string sql = "select current, avg20 from " + stockCode + "_history order by id desc limit 1";
		ResultSet_T r = Connection_executeQuery(conn, sql.c_str());
		while(ResultSet_next(r)) {
			date.push_back(ResultSet_getString(r,1));
			current.push_back(ResultSet_getDouble(r, 2));
			avg20.push_back(ResultSet_getDouble(r, 3));
		}

		std::cout<<"Stock: "<<stockCode<<std::endl;
		double profitPerStock = simpleAvg20(date, avg20, current);
		std::cout<<"Average profit: "<<profitPerStock*100<<"%"<<std::endl;
		std::cout<<std::endl;
		totalNum++;
		if(profitPerStock > 0) posNum++;
	}
	std::cout<<"Total stocks: "<<totalNum<<", Postive Profit stocks: "<<posNum<<std::endl;

	// free Database Pool
	Connection_close(conn);
	ConnectionPool_free(&dPool);
	URL_free(&dbURL);
	
	return 0;
}

double simpleAvg20(std::vector<std::string> date, std::vector<double> avg20, std::vector<double> current){

	std::vector<std::string> buydate;
	std::vector<std::string> selldate;
	std::vector<double> buyprice;
	std::vector<double> sellprice;
	std::vector<double> profits;
	std::vector<int> holddays;
	double hold = 0.0;
	int holdday = 0;
	if (current.size() < 20) return 0.0;

	for (int i = 20; i < current.size(); i++) {
		if(current[i] > avg20[i]) {
			if(hold == 0) {
				hold = current[i]; // buy
				holdday = 1;
				buydate.push_back(date[i]);
				buyprice.push_back(current[i]);
			}
			if(holdday) holdday++;
		} else { // current < avg20
			if (hold !=0) {
				profits.push_back((current[i] - hold - current[i]*0.003) / hold);
				selldate.push_back(date[i]);
				sellprice.push_back(current[i]);
				holddays.push_back(holdday);
				hold = 0.0; // sell
			}
			holdday = 0;
		}
	}

	for (int i = 0; i < profits.size(); i++) {
		std::cout<<"Buy Date: "<<buydate[i];
		std::cout<<", Buy Price: "<<buyprice[i];
		std::cout<<", Sell Date: "<<selldate[i];
		std::cout<<", Sell Price: "<<sellprice[i];
		std::cout<<", Hold Days: "<<holddays[i];
		std::cout<<", Profit: "<<profits[i]*100<<"%."<<std::endl;
	}

	if (profits.size() <= 0) return 0.0;
	double sum = 0.0;
	for (auto it : profits)
		sum += it;
	return sum/profits.size();

}
