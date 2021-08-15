#! /bin/bash

CFLAGS="-pedantic -Wall -Werror -ggdb3"
CLIBS="-lm -lgmp"
SRCDIR="src"
OUTDIR="output"

r=`gcc ${CLIBS} ${SRCDIR}/main.c ${CLIBS} -o ${OUTDIR}/main`
echo $r
#./${OUTDIR}/main
