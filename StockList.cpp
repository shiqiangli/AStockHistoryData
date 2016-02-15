#include "StockList.h"
#include <fstream>
#include <iostream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

StockList::StockList(std::string filename)
{
	this->filename = filename;
}

StockList::~StockList()
{
}

void StockList::setStockList()
{
	std::string line;
	std::ifstream fd((this->filename).c_str());
	if(!fd)
	{
		std::cout<<"File Open Error!"<<std::endl;
		return;
	}
	
	while(std::getline(fd, line))
	{
		std::vector<std::string> temp;
		boost::split(temp, line, boost::is_any_of(" "), boost::token_compress_on);
		//std::cout<<":"<<temp[temp.size() - 1]<<std::endl;
		this->stockList.push_back(temp[temp.size() - 1]);
	}
}

std::vector<std::string> StockList::getStockList()
{
	setStockList();
	return this->stockList;
}
