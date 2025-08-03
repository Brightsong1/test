CC = g++
CFLAGS = -std=c++11 -Wall -pthread
TARGETS = stereo syntez lkg

all: $(TARGETS)

stereo: stereo.cpp
	$(CC) $(CFLAGS) -o stereo stereo.cpp -lm

syntez: syntez.cpp
	$(CC) $(CFLAGS) -o syntez syntez.cpp -lm

lkg: lkg.cpp
	$(CC) $(CFLAGS) -o lkg lkg.cpp

clean:
	rm -f $(TARGETS)
