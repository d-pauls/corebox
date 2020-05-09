#!/bin/sh

if [ $# -ne 1 ]; then
	echo "Usage: $0 [config]" 1>&2
	exit 1
fi

sed 's/#.*$//g;s/\s/ /g' $1 | sort | tr '\n' ' '
