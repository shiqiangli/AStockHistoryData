#include "ReadStockList.h"
#include <fstream>
#include <iostream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

ReadStockList::ReadStockList(std::string fileName)
{
	this->_fileName = fileName;
	this->readStockList();
}

ReadStockList::~ReadStockList() {}

void ReadStockList::readStockList()
{
	std::string line;
	std::ifstream fd((this->_fileName).c_str());
	if(!fd)
	{
		std::cout<<"File '"<<_fileName<<"' Open Error!"<<std::endl;
		return;
	}

	while (std::getline(fd, line))
	{
		std::vector<std::string> temp;
		boost::split(temp, line, boost::is_any_of(" "), boost::token_compress_on);
		_stockList.push_back(temp[0]);
	}
}
