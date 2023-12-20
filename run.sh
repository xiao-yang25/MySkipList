#!/bin/bash
cd build

if [[ " $* " == *" -t "* ]]; then
    ./skip_list_test
else
    ./skip_list_exe
fi
