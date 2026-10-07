#!/bin/sh
set -eu

ELF="${1:-HGPS2.ELF}"
OUT="${2:-HGPS2-P0.1.iso}"
STAGE="${TMPDIR:-/tmp}/hgps2-iso-$$"

if [ ! -f "$ELF" ]; then
    echo "Missing ELF: $ELF" >&2
    exit 1
fi

MKISOFS="$(command -v mkisofs || command -v genisoimage || true)"
if [ -z "$MKISOFS" ]; then
    echo "mkisofs/genisoimage not found" >&2
    exit 1
fi

trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE"
cp "$ELF" "$STAGE/HGPS2.ELF"
cp iso/SYSTEM.CNF "$STAGE/SYSTEM.CNF"

"$MKISOFS" -o "$OUT" -V HGPS2_P01 -iso-level 1 "$STAGE"
echo "Created $OUT"
