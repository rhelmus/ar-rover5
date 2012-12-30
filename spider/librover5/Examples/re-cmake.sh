#!/bin/sh

STARTDIR=`pwd`

for F in `echo */build`; do
    read -p "Refreshing $F, continue?"
    [ "${REPLY}" != "y" ] && exit 1
    cd "${F}" && rm -rf * && cmake ..
    cd "${STARTDIR}"
done

