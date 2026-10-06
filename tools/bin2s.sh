#!/bin/sh
set -eu

if [ "$#" -ne 3 ]; then
    echo "usage: bin2s.sh <input> <output.s> <symbol>" >&2
    exit 2
fi

input="$1"
output="$2"
symbol="$3"

cat > "$output" <<EOF
    .section .rodata
    .balign 16
    .global ${symbol}
    .type ${symbol}, @object
${symbol}:
    .incbin "${input}"
    .global size_${symbol}
    .type size_${symbol}, @object
    .balign 4
size_${symbol}:
    .word . - ${symbol}
EOF
