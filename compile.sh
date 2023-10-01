#!/bin/bash

if ! command -v wine &> /dev/null; then
    echo "error: wine is not installed, compilation terminated."
    exit 1
fi

make -f makefile

if [ $? -ne 0 ]; then
    echo "error: compilation terminated."
    read -p "press enter to continue..."
    exit 1
fi

wine ./build/8964.exe   # wine required