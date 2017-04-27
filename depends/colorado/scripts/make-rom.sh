#!/bin/bash

zip -u -9 rom.zip `lua rom-list.lua | xargs`
