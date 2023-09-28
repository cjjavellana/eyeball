CC=gcc
CFLAGS=-g3 -Wall 

all: clean libyaml apr main

onlyme: clean main

main: main.o mastercfg.o 
	$(CC) main.o mastercfg.o -L./third_party/apr/.libs -L./third_party/yaml/src/.libs -L./third_party/pcre2/.libs -o eyeball -lyaml -lapr-1 -lpcre2-8

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

mastercfg.o: mastercfg.c
	$(CC) $(CFLAGS) -c mastercfg.c

pcre2:
	cd ./third_party/pcre2 && ./configure
	$(MAKE) -C ./third_party/pcre2
	rm -f ./third_party/pcre2/.libs/*.dylib

apr:
	cd ./third_party/apr && ./configure
	$(MAKE) -C ./third_party/apr
	rm -f ./third_party/apr/.libs/*.dylib

libyaml: 
	cd ./third_party/yaml && ./configure
	$(MAKE) -C ./third_party/yaml
	# By default Mac's linker will choose the dynamic library than the static library
	# Thus, delete the dynamic library from the library scan path
	#
	# Reference: https://gist.github.com/loderunner/b6846dd82967ac048439
	rm -f ./third_party/yaml/src/.libs/*.dylib

libyaml-clean:
	$(MAKE) clean -C ./third_party/yaml 

apr-clean:
	$(MAKE) clean -C ./third_party/apr

pcre2-clean:
	$(MAKE) clean -C ./third_party/pcre2

clean:
	$(RM) *.o *.out eyeball
