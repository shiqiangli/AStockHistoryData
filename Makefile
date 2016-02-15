
SRCDIR    = .
INCDIR    = .
OBJDIR    = .
TARGET    = crawler.out

SRCS      = $(wildcard $(SRCDIR)/*.cpp)
#SRCS      =  ThreadPool.cpp StockList.cpp main.cpp
#SRCS      =  ConnPool.cpp StockList.cpp main.cpp
#SRCS      =  StockList.cpp main.cpp
NSRCS     = $(notdir $(SRCS))
OBJS      = $(patsubst %.cpp, $(OBJDIR)/%.o, $(NSRCS))

CC        = g++
LD	      = ld
CPPFLAGS  = -g -std=c++11 -O0 -Werror -I/usr/local/include/zdb
LDFLAGS   = -L/lib64 -L/usr/lib64/mysql -lmysqlclient -lcurl -lboost_locale -L/usr/local/lib -lzdb

all:$(OBJS)
	$(CC) $^ -o $(TARGET) -I$(INCDIR) $(LDFLAGS)

%.o:%.cpp
	$(CC) -c $< $(CPPFLAGS)


#all:$(OBJS)
#	echo $(SRCS)
#	#$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET)
#
#
#$(OBJS):$(SRCS)
##%.o:%.cpp
#tt:
#	$(GCC) $(LDFLAGS) -o $(TARGET) -I $(INCDIR) $(CPPFLAGS) $(SRCS)
#	#$(GCC) $(LDFLAGS) -o $@ -I $(INCDIR) $(CPPFLAGS) -c $+
#
clean:
	rm *.o *.out

