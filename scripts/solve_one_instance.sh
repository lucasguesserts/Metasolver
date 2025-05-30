#!/bin/bash

# Dirs
export SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
export SOURCE_DIR=$(realpath $SCRIPT_DIR/..)
export BUILD_DIR=$(realpath $SCRIPT_DIR/../build)

# Variables
export INPUT_DIR="${SOURCE_DIR}/instances/"
export SOLVER="${BUILD_DIR}/Release/BSG_CLP"
export SOLVER_TIMEOUT=5
export SOLVER_MIN_FR=0.98
export ARAYA_2014_SOLVER_OPTIONS="--seed=42 --alpha=0.0 --beta=0.0 --gamma=0.0 -p0.0 -fBR"
export ARAYA_2017_SOLVER_OPTIONS="--seed=42 --alpha=4 --beta=1 --gamma=0.2 -p0.04 -fBR"


solve () {
    local INSTANCE_SET=$1
    local INSTANCE_NUMBER=$2
    ${SOLVER} \
        ${INPUT_DIR}/BR${INSTANCE_SET}.txt \
        -i ${INSTANCE_NUMBER} \
        --timelimit=${SOLVER_TIMEOUT} \
        --min_fr=${SOLVER_MIN_FR} \
        ${ARAYA_2014_SOLVER_OPTIONS}
}
export -f solve

solve 2 2
