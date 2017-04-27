#!/bin/bash

LD_LIBRARY_PATH=$PWD:$PWD/graphics:$PWD/audio:$PWD/depends

exec apitrace trace platformer/bin/release/Platformer.exe
