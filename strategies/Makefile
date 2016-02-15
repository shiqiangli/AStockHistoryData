
SRCDIR    = ./src
INCDIR    = ./include
OBJDIR    = ./obj
LIBDIR    = ./lib
BINDIR	  = ./bin
TARGET    = crawler.out

SRCS      = $(wildcard $(SRCDIR)/*.cpp)
#SRCS      =  ThreadPool.cpp StockList.cpp main.cpp
#SRCS      =  ConnPool.cpp StockList.cpp main.cpp
#SRCS      =  StockList.cpp main.cpp
NSRCS     = $(notdir $(SRCS))
OBJS      = $(patsubst %.cpp, $(OBJDIR)/%.o, $(NSRCS))

CC        = g++
LD	      = ld
AR		  = ar
CPPFLAGS  = -std=c++11 -O3 -Werror -I/usr/local/include/zdb -I$(INCDIR) -I../ -I../
LDFLAGS   = -L/lib64 -L/usr/lib64 -L/usr/lib64/mysql -lmysqlclient -lcurl -lboost_locale -L/usr/local/lib -lzdb -L$(LIBDIR) -lfetchurl -lboost_regex -lrdstocklist -lboost_date_time


all:$(OBJS)
	$(CC) $^ -o $(TARGET) -I$(INCDIR) $(LDFLAGS)

%.o:%.cpp
	$(CC) -c $< $(CPPFLAGS)

lib: libfetchurl librdstocklist
libfetchurl:
	$(CC) -c $(SRCDIR)/FetchUrlStream.cpp $(CPPFLAGS) -o $(OBJDIR)/FetchUrlStream.o
	$(AR) cr $(LIBDIR)/libfetchurl.a $(OBJDIR)/FetchUrlStream.o
librdstocklist:
	$(CC) -c $(SRCDIR)/ReadStockList.cpp $(CPPFLAGS) -o $(OBJDIR)/ReadStockList.o
	$(AR) cr $(LIBDIR)/librdstocklist.a $(OBJDIR)/ReadStockList.o

utils: fetchSHLists createtables updateHistory updateTradeInfo traceRealInfo updateAvg
fetchSHLists:
	$(CC) -c $(SRCDIR)/fetchSHLists.cpp $(CPPFLAGS) -o $(OBJDIR)/fetchSHLists.o
	$(CC) $(OBJDIR)/fetchSHLists.o -o fetchSHLists.out -I$(INCDIR) $(LDFLAGS)
	mv fetchSHLists.out $(BINDIR)/fetchSHLists.out
createtables:
	$(CC) -c $(SRCDIR)/createTables.cpp $(CPPFLAGS) -o $(OBJDIR)/createTables.o
	$(CC) $(OBJDIR)/createTables.o -o createTables.out -I$(INCDIR) $(LDFLAGS)
	mv createTables.out $(BINDIR)/createTables.out
updateHistory:
	$(CC) -c $(SRCDIR)/AnalyzeHistoryOnce.cpp $(CPPFLAGS) -o $(OBJDIR)/AnalyzeHistoryOnce.o
	$(CC) -c $(SRCDIR)/updateHistory.cpp $(CPPFLAGS) -o $(OBJDIR)/updateHistory.o
	$(CC) $(OBJDIR)/updateHistory.o $(OBJDIR)/AnalyzeHistoryOnce.o -o updateHistory.out -I$(INCDIR) $(LDFLAGS)
	mv updateHistory.out $(BINDIR)/updateHistory.out
updateTradeInfo:
	$(CC) -c $(SRCDIR)/TradeDetailsPerDay.cpp $(CPPFLAGS) -o $(OBJDIR)/TradeDetailsPerDay.o
	$(CC) -c $(SRCDIR)/updateTradeInfo.cpp $(CPPFLAGS) -o $(OBJDIR)/updateTradeInfo.o
	$(CC) $(OBJDIR)/updateTradeInfo.o $(OBJDIR)/TradeDetailsPerDay.o -o updateTradeInfo.out -I$(INCDIR) $(LDFLAGS)
	mv updateTradeInfo.out $(BINDIR)/updateTradeInfo.out
traceRealInfo:
	$(CC) -c $(SRCDIR)/FetchStockRealTimeOnce.cpp $(CPPFLAGS) -o $(OBJDIR)/FetchStockRealTimeOnce.o
	$(CC) -c $(SRCDIR)/traceStocksRealTime.cpp $(CPPFLAGS) -o $(OBJDIR)/traceStocksRealTime.o
	$(CC) $(OBJDIR)/traceStocksRealTime.o $(OBJDIR)/FetchStockRealTimeOnce.o -o traceStocksRealTime.out -I$(INCDIR) $(LDFLAGS)
	mv traceStocksRealTime.out $(BINDIR)/traceStocksRealTime.out
updateAvg:
	$(CC) -c $(SRCDIR)/updateAvg.cpp $(CPPFLAGS) -o $(OBJDIR)/updateAvg.o
	$(CC) $(OBJDIR)/updateAvg.o -o updateAvg.out -I$(INCDIR) $(LDFLAGS)
	mv updateAvg.out $(BINDIR)/updateAvg.out

test:
	$(CC) -c main.cpp $(CPPFLAGS) -o $(OBJDIR)/main.o
	$(CC) $(OBJDIR)/main.o  -o $(TARGET) -I$(INCDIR) $(LDFLAGS)

clean:
	rm $(OBJDIR)/*.o *.out

