#pragma once
#include <string>
#include <vector>

class ReadStockList {

public:
	ReadStockList (std::string fileName);
	~ReadStockList();
	
	std::vector<std::string> const & getStockList() const { 
		//this->readStockList(); 
		return _stockList; 
	}

private:
	std::string _fileName;
	std::vector<std::string> _stockList;
	
	void readStockList();

};
