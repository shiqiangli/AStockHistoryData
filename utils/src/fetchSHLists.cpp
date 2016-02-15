#include "FetchUrlStream.h"
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include "stdlib.h"

int main(int argc, char*argv[])
{
	// get File Name from input.
	if (argc < 3) {
		std::cout<<"Please add file name 'shA.full.2015xx' and 'shB.full.2015xx' as arguments."<<std::endl;
		std::cout<<"Path is needed!, Example: ../../stockLists/shA.full.201509."<<std::endl;
		return 1;
	}
	std::string shAFull = argv[1];
	std::string shBFull = argv[2];
	std::ofstream f1(shAFull.c_str());
	std::ofstream f2(shBFull.c_str());
	if (!f1 || !f2)
	{
		std::cout<<"Open Write File Error!"<<std::endl;
		return 2;
	}

	std::string url = "http://www.sse.com.cn/js/common/ssesuggestdata.js";
	FetchUrlStream fs(url);
	std::string urlBuffer = fs.getStream();
	// split url buffer, drop header and tailor
	int strBegin = urlBuffer.find("_t.push");
	int strEnd = urlBuffer.find("return");
	urlBuffer = urlBuffer.substr(strBegin, strEnd - strBegin);

	boost::regex reg("_t.push[\(][\{]|val:\"|\",val2:\"|\",val3:\"[*|a-z|A-Z]{0,5}\"..;");
	boost::cregex_token_iterator itrBegin = make_regex_token_iterator(urlBuffer.c_str(), reg, -1);
	boost::cregex_token_iterator itrEnd;
	bool flagA = true;
	for(boost::cregex_token_iterator itr = itrBegin; itr != itrEnd;++itr)
	{	
		std::string str = *itr;
		if (str.size() > 1) // drop unvaluable lines.
		    if(str.size() == 6) { // stock code
		    	if(atoi(str.c_str()) < 900000) { // sh A
		    		f1<<str<<" ";
		    		flagA = true;
		    	} else {
		    		f2<<str<<" ";
		    		flagA = false;
		    	}
		    } else { // stock name
		    	if (flagA)
					f1<<str<<std::endl;
		    	else
		    		f2<<str<<std::endl;
		    }
	}
	f1.close();
	f2.close();
	return 0;
}
