#!/bin/bash

make -f MakeFile.linux

if [ $? -ne 0 ]; then
    echo "error: compilation terminated."
    read -p "press enter to continue..."
    exit 1
fi

./build/8964