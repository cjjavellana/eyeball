# Manual Testing on Ubuntu Linux x64-64
This section describes how to manually build and test this software on AWS Ubuntu Linux 

## AWS Ubuntu Instance Start Up Script
```bash
#!/bin/bash

sudo apt update
sudo apt install -y gcc make gdb autoconf libtool
git clone https://github.com/cjjavellana/eyeball.git
cd eyeball
make
./eyeball -f subject.yml -c demo.yml
```
