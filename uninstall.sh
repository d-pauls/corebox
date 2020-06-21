#!/bin/sh

#####################################################
# It is not recommended to run this script manually #
# You should run `make install` instead             #
#####################################################

# exit when a command fails
set -e

# check if the arguments exist
if [ -z $1 ] || [ -z $2 ]; then
	echo "Usage: $0 <COREBOX> <TARGET>"
	exit 1
fi

COREBOX=$1
PREFIX=$2

# unlink (remove) the links to corebox
cd "$PREFIX"
for file in $(./$COREBOX | tail -1 | tr -d ','); do
	unlink $file;
done

# remove the binary
rm $COREBOX

echo "$COREBOX removed successfully from $PREFIX"
