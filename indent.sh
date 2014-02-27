#!/bin/sh
D=`dirname $0`
#indent -kr -nut -l620 "$1"
uncrustify -c "$D/uncrustify.cfg" -l CPP --no-backup "$1"
