#!/bin/bash

pushd ../sounds > /dev/null

ls *.ogg | sort | while read line
do
	filename=$(basename "$line")
	filename="${filename%.*}"
	echo "$filename,"
done

popd > /dev/null
