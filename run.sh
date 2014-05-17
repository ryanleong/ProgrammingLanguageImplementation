#!/bin/bash

if test "$1" = ""
then 
    echo "./run.sh <input wiz file>"
else
	# compile
	make

	# run wiz
	./wiz -p "$1" > out.temp

	echo ""
	echo "======================================================="

	# run oz
	cd oz
	make
	cd ..
	./oz/oz -i out.temp 

	echo "======================================================="
	echo ""

	# delete out.temp
	#rm -f out.temp

	echo "\nWIZ output file: out.temp"
fi


