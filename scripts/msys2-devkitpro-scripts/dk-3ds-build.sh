#!/bin/bash
# $1 = $DEVKITPRO
# $2 = $DEVKITARM
# $3 = branch

[ -z "$1" ] && { echo "argument 1 required."; exit 1; }
[ -z "$2" ] && { echo "argument 2 required."; exit 1; }
[ -z "$3" ] && { echo "argument 3 required."; exit 1; }

export DEVKITPRO=$1
export DEVKITARM=$2
export PATH="$PATH:$DEVKITARM/bin"

cd /mzx-build-workingdir
mkdir -p zips
cd megazeux

git checkout $3

arch/3ds/CONFIG.3DS
make shaders
make debuglink -j8
make package
make archive

mv build/dist/3ds/* /mzx-build-workingdir/zips/

make distclean
