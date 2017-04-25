#!/bin/bash

LD_LIBRARY_PATH=$PWD:$PWD/../build-colorado-shared-arch-Desktop-Release/:$PWD/../terf/build-terf-Desktop-Release:$PWD/../colorado/build-video-encoder-arch-Desktop-Release/

exec platformer/Platformer.exe
