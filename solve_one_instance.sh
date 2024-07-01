#!/bin/bash

solve () {
    local INSTANCE_SET=$1
    local INSTANCE_NUMBER=$2
    ./build/Release/BSG_CLP \
        problems/clp/benchs/BR/BR${INSTANCE_SET}.txt \
        -i ${INSTANCE_NUMBER} \
        --min_fr=0.98 \
        --timelimit=30 \
        --alpha=4 \
        --beta=1 \
        --gamma=0.2 \
        -p0.04 \
        -fBR \
        --seed=42
}
export -f solve

solve 15 0
