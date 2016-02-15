#ifndef __CONFIG_H

#include "zdb.h"

// Configuration of DataBase
#define DBIP    	"localhost"
#define DBPORT    	"3306"
#define DBUSER		"root"
#define DBPASSWD	"123456"
#define DBNAME		"stocks_cn"

// Configuration of source of stock information
#define URLHEAD		"http://hq.sinajs.cn/list="
#define MARKETSH	"sh"
#define MARKETSZ	"sz"

// for Pools
#define MAXTHREADS	20
#define MAXDBCONNS	20

// for market open time
#define OPENAM		93000
#define OPENPM		130000
#define CLOSEAM		113000
#define CLOSEPM		150100

// for timestamp
#define TIMEZONE 8 // TimeZone

#endif
