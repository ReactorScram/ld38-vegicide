#!/bin/bash
export LD_LIBRARY_PATH=graphics:audio:depends:/mnt/utmp/codeblocks/usr/lib
exec platformer/bin/release/Platformer.exe
