#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$SCRIPT_DIR/../build/main"
HEADER="n,strategy,threads,cs_ns,elapsed_sec,ops_per_sec,correct"

for lock in futex adaptive mcs ; do
    OUT="$SCRIPT_DIR/results_${lock}.csv"
    echo "$HEADER" > "$OUT"

    for threads in 1 2 4 8 ; do
        for n in {1..4} ; do
            "$BIN" --lock "$lock" --threads "$threads" \
                   --iters 100000 --critical 10000 --non_critical 10 \
                | sed "s/^/$n,/" >> "$OUT"
        done
    done
done