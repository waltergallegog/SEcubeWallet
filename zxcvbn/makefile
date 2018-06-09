CFLAGS ?= -O2 -Wall -Wextra -Wdeclaration-after-statement
CXXFLAGS ?= -O2 -Wall -Wextra

# default programs
CC ?= gcc
AR ?= ar
CXX ?= g++

# need zxcvbn.h prior to package installation
CPPFLAGS += -I.

#the dictionary txt list will be passed as argument to make
WORDS = words-eng_wiki.txt
#words-female.txt words-male.txt words-passwd.txt words-surname.txt words-tv_film.txt


dict-src.h: dictgen $(WORDS)
	./dictgen -o dict-src.h $(WORDS)

dictgen: dict-generate.cpp makefile
	$(CXX) $(CPPFLAGS) -std=c++11 $(CXXFLAGS) \
		-o dictgen dict-generate.cpp $(LDFLAGS)

clean:
	rm -f dict-*.h
	rm -f dictgen
