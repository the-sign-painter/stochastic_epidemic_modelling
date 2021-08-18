#! /bin/sh

ABSDIR="$(dirname $(readlink -f $0))"

date

CFLAGS="-pedantic -Werror -ggdb3"
CLIBS="-lm -lgmp -g"
SRCDIR="${ABSDIR}/src"
OUTDIR="${ABSDIR}/output"

mkdir -p ${OUTDIR}

gcc ${CLIBS} ${SRCDIR}/main.c ${CFLAGS} -o ${OUTDIR}/main

if [[ $? -ne 0 ]]; then
    echo "Failed to compile"
    exit 1
fi

echo "Compiled successfully"
./output/main

if [[ $? -ne 0 ]]; then
    echo "Execute failed"
    exit 1
fi
echo "Executed sucessfully"
# sleep 0.5
# viu ./output/graph.png
