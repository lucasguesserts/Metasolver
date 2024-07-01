#!/bin/sh

# Options
export INPUT_DIR="problems/clp/benchs/BR/"
export OUTPUT_DIR="out/"
export INDIVIDUAL_VOLUME_USAGE_FILE_NAME="individual_volume_usage.log"
export AVERAGE_VOLUME_USAGE_FILE_NAME="average_volume_usage.log"
export INSTANCE_SET_LIST=$(seq 0 1 15)
export INSTANCE_LIST=$(seq 0 1 9)
export NUMBER_OF_THREADS_SELECTED=$(expr `nproc --all` / 2)
export NUMBER_OF_THREADS=$(($NUMBER_OF_THREADS_SELECTED>0 ? $NUMBER_OF_THREADS_SELECTED : 1))
export SOLVER="./build/BSG_CLP"
export SOLVER_OPTIONS="--seed=42 --timelimit=30 --min_fr=0.98 --alpha=4 --beta=1 --gamma=0.2 -p0.04 -fBR --show_layout"


# auxiliary variables
export INDIVIDUAL_VOLUME_USAGE_FILE_PATH="$OUTPUT_DIR/$INDIVIDUAL_VOLUME_USAGE_FILE_NAME"
export AVERAGE_VOLUME_USAGE_FILE_PATH="$OUTPUT_DIR/$AVERAGE_VOLUME_USAGE_FILE_NAME"


# auxiliary functions
instance_set_output_dir () {
    INSTANCE_SET=$1
    echo "$OUTPUT_DIR/set_${INSTANCE_SET}/"
}
export -f instance_set_output_dir

solve_instance () {
    INSTANCE_SET=$1
    INSTANCE=$2

    echo -e "solving -> set $INSTANCE_SET - instance $INSTANCE"

    INSTANCE_SET_INPUT_FILE="${INPUT_DIR}/BR${INSTANCE_SET}.txt"
    INSTANCE_SET_OUTPUT_DIR=$(instance_set_output_dir $INSTANCE_SET)
    mkdir -p $INSTANCE_SET_OUTPUT_DIR
    LOG_FILE="${INSTANCE_SET_OUTPUT_DIR}/${INSTANCE}.log"

    $SOLVER $INSTANCE_SET_INPUT_FILE $SOLVER_OPTIONS -i $INSTANCE > $LOG_FILE
}
export -f solve_instance

extract_volume_usage () {
    # extract volume usage from an output file
    # if not found, for any reason, output "-1"
    INSTANCE_SET=$1
    INSTANCE=$2

    INSTANCE_SET_OUTPUT_DIR=$(instance_set_output_dir $INSTANCE_SET)
    OUTPUT_FILE="${INSTANCE_SET_OUTPUT_DIR}/${INSTANCE}.log"

    # Use grep to find the line number containing "% volume utilization"
    LINE_NUMBER=$(grep -n "% volume utilization" "$OUTPUT_FILE" | cut -d ":" -f 1)

    VOLUME_USAGE="-1"
    if [ -n "$LINE_NUMBER" ]; then
        # Use awk to extract the number from the next line
        EXTRACTED_VALUE=$(awk "NR==$(($LINE_NUMBER + 1)){print;exit}" "$OUTPUT_FILE" | grep -Eo "[0-9]+\.[0-9]+")
        if [ -n "$EXTRACTED_VALUE" ]; then
            VOLUME_USAGE=$EXTRACTED_VALUE
        fi
    fi
    echo "$INSTANCE_SET,$INSTANCE,$VOLUME_USAGE" >> $INDIVIDUAL_VOLUME_USAGE_FILE_PATH
}
export -f extract_volume_usage

compute_average_volume () {
    # Store the filename in a variable
    FILE_NAME=$1

    # Check if the file exists
    if [ ! -f "$FILE_NAME" ]; then
        echo "File $FILE_NAME not found."
        exit 1
    fi

    # Read the file line by line
    while IFS=',' read -r instance_set instance volume_usage; do
        # If instance_set is empty (header line), skip to the next line
        if [ -z "$instance_set" ]; then
            continue
        fi

        # Check if the instance_set has changed
        if [ "$instance_set" != "$prev_instance_set" ]; then
            # If it has, compute the average volume usage for the previous instance_set
            if [ ! -z "$prev_instance_set" ]; then
                average=$(echo "scale=2; $total_volume / $count" | bc)
                echo "Average volume usage for instance_set $prev_instance_set: $average"
            fi

            # Reset total_volume and count for the new instance_set
            total_volume=0
            count=0
            prev_instance_set=$instance_set
        fi

        # Add the current volume_usage to the total
        total_volume=$(echo "$total_volume + $volume_usage" | bc)
        count=$((count + 1))
    done < "$FILE_NAME"

    # Compute the average volume usage for the last instance_set
    average=$(echo "scale=2; $total_volume / $count" | bc)
    echo "Average volume usage for instance_set $prev_instance_set: $average"
}
export -f compute_average_volume


# solve the instances - compute the volume usage for each instance
rm -rf $OUTPUT_DIR
mkdir -p $OUTPUT_DIR
parallel  \
    -j $NUMBER_OF_THREADS \
    --keep-order \
    --linebuffer \
    solve_instance ::: ${INSTANCE_SET_LIST} ::: ${INSTANCE_LIST}


# log the individual volume usage to a single file
echo "instance_set,instance,volume_usage" > $INDIVIDUAL_VOLUME_USAGE_FILE_PATH
parallel  \
    -j $NUMBER_OF_THREADS \
    --keep-order \
    --linebuffer \
    extract_volume_usage ::: ${INSTANCE_SET_LIST} ::: ${INSTANCE_LIST}


# log the average volume usage per instance set to a single file
compute_average_volume $INDIVIDUAL_VOLUME_USAGE_FILE_PATH > $AVERAGE_VOLUME_USAGE_FILE_PATH
sed -i '1d' $AVERAGE_VOLUME_USAGE_FILE_PATH # remove first line


# logs
cat $AVERAGE_VOLUME_USAGE_FILE_PATH
