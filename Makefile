CC = g++

CFLAGS = -std=c++1y -Wall -g

LIBS = -lcurl -lcurlpp

SRCS =  downloaderV4.cpp
OBJS = $(SRCS:.cpp=.o)

MAIN = downloader

.PHONY: depend clean

all:    $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
