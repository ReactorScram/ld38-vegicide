#!/bin/bash

LD_LIBRARY_PATH=$PWD:$PWD/graphics:$PWD/audio:$PWD/depends

exec platformer/bin/release/Platformer.exe
