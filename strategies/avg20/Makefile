
SRCDIR    = ./
INCDIR    = ../../utils/include
OBJDIR    = ./
LIBDIR    = ../../utils/lib
BINDIR	  = .
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
CPPFLAGS  = -std=c++11 -O3 -Werror -I/usr/local/include/zdb -I$(INCDIR) -I../../
LDFLAGS   = -L/lib64 -L/usr/lib64 -L/usr/lib64/mysql -lmysqlclient -lcurl -lboost_locale -L/usr/local/lib -lzdb -L$(LIBDIR) -lfetchurl -lboost_regex -lrdstocklist -lboost_date_time


all:$(OBJS)
	$(CC) $^ -o $(TARGET) -I$(INCDIR) $(LDFLAGS)

%.o:%.cpp
	$(CC) -c $< $(CPPFLAGS)


test:
	$(CC) -c main.cpp $(CPPFLAGS) -o $(OBJDIR)/main.o
	$(CC) $(OBJDIR)/main.o  -o $(TARGET) -I$(INCDIR) $(LDFLAGS)

clean:
	rm $(OBJDIR)/*.o *.out

