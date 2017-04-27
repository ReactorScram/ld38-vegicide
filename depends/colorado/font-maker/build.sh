#!/bin/bash

exec g++ -o font-maker.so -O2 -fPIC -shared main.cpp
