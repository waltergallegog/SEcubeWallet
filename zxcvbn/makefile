CFLAGS ?= -O2 -Wall -Wextra -Wdeclaration-after-statement
CXXFLAGS ?= -O2 -Wall -Wextra

# default programs
CC ?= gcc
CXX ?= g++

# need zxcvbn.h prior to package installation
CPPFLAGS += -I.

# library metadata
TARGET_LIB = libzxcvbn.so.0.0.0
SONAME = libzxcvbn.so.0

#WORDS is an input argument
#WORDS = words-eng_wiki.txt words-female.txt words-male.txt words-passwd.txt words-surname.txt words-tv_film.txt

$(TARGET_LIB): zxcvbn-inline-pic.o
	$(CC) $(CPPFLAGS) $(CFLAGS) \
		-o $@ $^ -fPIC -shared -Wl,-soname,$(SONAME) $(LDFLAGS) -lm
	if [ ! -e $(SONAME) ]; then ln -s $(TARGET_LIB) $(SONAME); fi
	if [ ! -e libzxcvbn.so ]; then ln -s $(TARGET_LIB) libzxcvbn.so; fi

zxcvbn-inline-pic.o: zxcvbn.c dict-src.h zxcvbn.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c -o $@ $<

dict-src.h: dictgen $(WORDS)
	./dictgen -o dict-src.h $(WORDS)

dictgen: dict-generate.cpp makefile
	$(CXX) $(CPPFLAGS) -std=c++11 $(CXXFLAGS) \
		-o dictgen dict-generate.cpp $(LDFLAGS)

clean:
	rm -f zxcvbn-inline-pic.o
	rm -f dict-*.h
	rm -f dictgen
	rm -f ${TARGET_LIB} ${SONAME} libzxcvbn.so