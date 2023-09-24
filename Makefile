CC=gcc
CFLAGS=-g3 -Wall YAML_DECLARE_STATIC=1

all: clean main

main: main.o mastercfg.o 
	$(CC) main.o mastercfg.o -L./third_party/yaml/src/.libs -o eyeball -lyaml

main.o: main.c
	$(CC) -c main.c 

mastercfg.o: mastercfg.c
	$(CC) -c mastercfg.c 

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

clean:
	$(RM) *.o *.out eyeball
