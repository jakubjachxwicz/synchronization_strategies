#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$SCRIPT_DIR/../build/main"
HEADER="n,strategy,threads,cs_ns,elapsed_sec,ops_per_sec,fairness,correct"
OUT_DIR="$SCRIPT_DIR/high-contention"
mkdir -p "$OUT_DIR"

for lock in pthread mutex spin_then_park mcs_lock ; do
    OUT="$OUT_DIR/results_${lock}.csv"
    echo "$HEADER" | tee "$OUT"

    for threads in 2 4 6 8 10 12 14 16 ; do
        for n in {1..10} ; do
            "$BIN" --lock "$lock" --threads "$threads" \
                   --iters 20000 --critical 12000 --non_critical 100 \
                | sed "s/^/$n,/" | tee -a "$OUT"
        done
    done
done