#!/bin/bash

# Dirs
export SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
export SOURCE_DIR=$(realpath $SCRIPT_DIR/..)
export BUILD_DIR=$(realpath $SCRIPT_DIR/../build)

# Variables
export INPUT_DIR="${SOURCE_DIR}/problems/clp/benchs/BR/"
export SOLVER="${BUILD_DIR}/Release/BSG_CLP"

solve () {
    local INSTANCE_SET=$1
    local INSTANCE_NUMBER=$2
    ${SOLVER} \
        ${INPUT_DIR}/BR${INSTANCE_SET}.txt \
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
