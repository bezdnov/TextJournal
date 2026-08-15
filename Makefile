CXX = g++
CXXFLAGS = -std=c++17 -pthread

all: text_journal

test:
	echo 'Testing not yet implemented'

# journalapp
text_journal: libtext.so main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -L. -ltext -o text_journal

# library
libtext.so: libtext.o
	$(CXX) $(CXXFLAGS) -shared libtext.o -o libtext.so

libtext.o: libtext.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c libtext.cpp -o libtext.o

clean:
	rm -f *.so *.o text_journal


.PHONY: clean

