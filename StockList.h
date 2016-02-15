#ifndef __STOCKLIST_H
#include <string>
#include <vector>

class StockList {

public:
	StockList(std::string);
	~StockList();

	void setStockList();
	std::vector<std::string> getStockList();

private:
	std::string filename;
	std::vector<std::string> stockList;
};


#endif
