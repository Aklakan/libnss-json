#!/bin/sh

gcc -oroot-wrapper root-wrapper.c && sudo chown root:root root-wrapper && sudo chmod 4755 root-wrapper
