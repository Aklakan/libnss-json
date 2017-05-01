#!/bin/sh

#gcc -oroot-test root-test.c && sudo chown root:root root-test && sudo chmod u+s root-test
gcc -oroot-test root-test.c && sudo chown root:root root-test && sudo chmod 4755 root-test
