#! /bin/sh

ABSDIR="$(dirname $(readlink -f $0))"

date

CFLAGS="-pedantic -Wall -Werror -ggdb3"
CLIBS="-lm -lgmp"
SRCDIR="${ABSDIR}/src"
OUTDIR="${ABSDIR}/output"

mkdir -p ${OUTDIR}

gcc ${SRCDIR}/main.c ${CFLAGS} ${CLIBS} -o ${OUTDIR}/main

if [[ $? -ne 0 ]]; then
    echo "Failed to compile"
    exit 1
fi

echo "Compiled successfully"
./output/main
# sleep 0.5
# viu ./output/graph.png
