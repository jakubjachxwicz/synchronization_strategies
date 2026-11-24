#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$SCRIPT_DIR/../build/main"
HEADER="n,strategy,threads,cs_ns,elapsed_sec,ops_per_sec,fairness,correct"
OUT_DIR="$SCRIPT_DIR/example"
mkdir -p "$OUT_DIR"

for lock in pthread_mutex mutex spin_then_park mcs ; do
    OUT="$OUT_DIR/results_${lock}.csv"
    echo "$HEADER" | tee "$OUT"

    for threads in 2 4 ; do
        for n in {1..4} ; do
            "$BIN" --lock "$lock" --threads "$threads" \
                   --iters 100000 --critical 10000 --non_critical 10 \
                | sed "s/^/$n,/" | tee -a "$OUT"
        done
    done
done