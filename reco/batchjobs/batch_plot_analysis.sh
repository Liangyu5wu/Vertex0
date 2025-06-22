#!/bin/bash

# Script to batch process ROOT files and summarize results
# Usage: ./batch_plot_analysis.sh

# Configuration
OUTPUT_FOLDER="analysis_results_jetpt"
SUMMARY_FILE="${OUTPUT_FOLDER}/analysis_summary_jetpt.txt"
ROOT_MACRO="plot_recotime.C"

mkdir -p $OUTPUT_FOLDER
echo "Created output directory: $OUTPUT_FOLDER"

# Define analysis tasks
# Format: "mode,inputFile,outputBase,fitMin,fitMax,description"
ANALYSIS_TASKS=(
    "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to1.00.root,jetPt30to100000,-120,120,jetPt30"
    "0,jetmatching_reconstruction_Eover2.0_jetPt40to100000_dR0.3_jetWidth0.00to1.00.root,jetPt40to100000,-120,120,jetPt40"
    "0,jetmatching_reconstruction_Eover2.0_jetPt50to100000_dR0.3_jetWidth0.00to1.00.root,jetPt50to100000,-120,120,jetPt50"
    "0,jetmatching_reconstruction_Eover2.0_jetPt60to100000_dR0.3_jetWidth0.00to1.00.root,jetPt60to100000,-120,120,jetPt60"
    "0,jetmatching_reconstruction_Eover2.0_jetPt70to100000_dR0.3_jetWidth0.00to1.00.root,jetPt70to100000,-120,120,jetPt70"
    "0,jetmatching_reconstruction_Eover2.0_jetPt80to100000_dR0.3_jetWidth0.00to1.00.root,jetPt80to100000,-120,120,jetPt80"
    "0,jetmatching_reconstruction_Eover2.0_jetPt90to100000_dR0.3_jetWidth0.00to1.00.root,jetPt90to100000,-120,120,jetPt90"
    "0,jetmatching_reconstruction_Eover2.0_jetPt100to100000_dR0.3_jetWidth0.00to1.00.root,jetPt100to100000,-120,120,jetPt100"
    "0,jetmatching_reconstruction_Eover2.0_jetPt110to100000_dR0.3_jetWidth0.00to1.00.root,jetPt110to100000,-120,120,jetPt110"
    "0,jetmatching_reconstruction_Eover2.0_jetPt120to100000_dR0.3_jetWidth0.00to1.00.root,jetPt120to100000,-120,120,jetPt120"
    "0,jetmatching_reconstruction_Eover2.0_jetPt140to100000_dR0.3_jetWidth0.00to1.00.root,jetPt140to100000,-120,120,jetPt140"
    "0,jetmatching_reconstruction_Eover2.0_jetPt160to100000_dR0.3_jetWidth0.00to1.00.root,jetPt160to100000,-120,120,jetPt160"
    "0,jetmatching_reconstruction_Eover2.0_jetPt180to100000_dR0.3_jetWidth0.00to1.00.root,jetPt180to100000,-120,120,jetPt180"
    "0,jetmatching_reconstruction_Eover2.0_jetPt200to100000_dR0.3_jetWidth0.00to1.00.root,jetPt200to100000,-120,120,jetPt200"
    "0,jetmatching_reconstruction_Eover2.0_jetPt400to100000_dR0.3_jetWidth0.00to1.00.root,jetPt400to100000,-120,120,jetPt400"
)

MEANS=()
SIGMAS=()
DESCRIPTIONS=()

echo "Starting batch analysis..."
echo "Total tasks: ${#ANALYSIS_TASKS[@]}"

for i in "${!ANALYSIS_TASKS[@]}"; do
    TASK="${ANALYSIS_TASKS[i]}"
    IFS=',' read -r MODE INPUT_FILE OUTPUT_BASE FIT_MIN FIT_MAX DESCRIPTION <<< "$TASK"
    
    echo ""
    echo "Processing task $((i+1))/${#ANALYSIS_TASKS[@]}: $DESCRIPTION"
    echo "  File: $INPUT_FILE"
    
    # Check if input file exists
    if [ ! -f "$INPUT_FILE" ]; then
        echo "  Warning: Input file not found. Skipping..."
        continue
    fi
    
    # Execute ROOT command
    echo "  Running ROOT analysis..."
    TEMP_OUTPUT=$(mktemp)
    
    # Use pipe method for ROOT command
    {
        echo ".L ${ROOT_MACRO}"
        echo "plotAndFitHistograms($MODE, \"$INPUT_FILE\", \"$OUTPUT_BASE\", $FIT_MIN, $FIT_MAX)"
        echo ".q"
    } | timeout 180 root -l -b > $TEMP_OUTPUT 2>&1
    
    ROOT_OUTPUT=$(cat $TEMP_OUTPUT)
    
    # Extract mean and sigma values
    MEAN=$(echo "$ROOT_OUTPUT" | grep -E "Event.*Mean.*=" | sed -n 's/.*Mean = \([0-9.-]\+\).*/\1/p' | head -1)
    SIGMA=$(echo "$ROOT_OUTPUT" | grep -E "Event.*Sigma.*=" | sed -n 's/.*Sigma = \([0-9.-]\+\).*/\1/p' | head -1)
    
    # Try alternative extraction if first method fails
    if [ -z "$MEAN" ]; then
        MEAN=$(echo "$ROOT_OUTPUT" | grep -i "mean" | grep -o '[0-9.-]\+\.[0-9]\+' | head -1)
        SIGMA=$(echo "$ROOT_OUTPUT" | grep -i "sigma" | grep -o '[0-9.-]\+\.[0-9]\+' | head -1)
    fi
    
    echo "  Extracted: Mean=$MEAN, Sigma=$SIGMA"
    
    # Store results
    if [ ! -z "$MEAN" ]; then
        MEANS+=("$MEAN")
        SIGMAS+=("${SIGMA:-0}")
        DESCRIPTIONS+=("$DESCRIPTION")
        echo "  ✓ Success"
    else
        echo "  ✗ Failed to extract results"
        # Quick manual entry option
        read -t 5 -p "  Enter 'm' for manual input: " choice
        if [ "$choice" = "m" ]; then
            read -p "    Mean: " manual_mean
            read -p "    Sigma: " manual_sigma
            if [ ! -z "$manual_mean" ]; then
                MEANS+=("$manual_mean")
                SIGMAS+=("${manual_sigma:-0}")
                DESCRIPTIONS+=("$DESCRIPTION")
                echo "  ✓ Manual entry stored"
            fi
        fi
    fi
    
    rm -f $TEMP_OUTPUT
done

# Move generated plots
echo ""
echo "Moving plots to $OUTPUT_FOLDER..."
find . -maxdepth 1 -name "*.png" -type f -exec mv {} $OUTPUT_FOLDER/ \;

# Generate summary
echo "Generating summary file: $SUMMARY_FILE"

cat > $SUMMARY_FILE << EOF
Analysis Summary
================
Generated on: $(date)
Successful analyses: ${#MEANS[@]}/${#ANALYSIS_TASKS[@]}

Results:
EOF

for i in "${!DESCRIPTIONS[@]}"; do
    cat >> $SUMMARY_FILE << EOF
${DESCRIPTIONS[i]}: Mean=${MEANS[i]} ps, Sigma=${SIGMAS[i]} ps
EOF
done

cat >> $SUMMARY_FILE << EOF

Python Arrays:
means = [$(IFS=','; echo "${MEANS[*]}")]
sigmas = [$(IFS=','; echo "${SIGMAS[*]}")]
labels = [$(printf "'%s'," "${DESCRIPTIONS[@]}" | sed 's/,$//')]
EOF

echo ""
echo "=== Analysis Complete ==="
echo "Successful: ${#MEANS[@]}/${#ANALYSIS_TASKS[@]}"
echo ""
echo "Python-ready arrays:"
echo "means = [$(IFS=','; echo "${MEANS[*]}")]"
echo "sigmas = [$(IFS=','; echo "${SIGMAS[*]}")]"
echo "labels = [$(printf "'%s'," "${DESCRIPTIONS[@]}" | sed 's/,$//')]"
echo ""
echo "Files saved in: $OUTPUT_FOLDER"
