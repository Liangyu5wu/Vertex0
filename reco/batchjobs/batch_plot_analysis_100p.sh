#!/bin/bash

# Script to batch process ROOT files and summarize results
# Usage: ./batch_plot_analysis.sh [event|jet]

# Parse command line argument for mode
MODE_TYPE=${1:-"event"}  # Default to event mode if no argument provided

if [[ "$MODE_TYPE" != "event" && "$MODE_TYPE" != "jet" ]]; then
    echo "Usage: $0 [event|jet]"
    echo "  event: Use event mode (default)"
    echo "  jet:   Use jet mode"
    exit 1
fi

echo "Running in $MODE_TYPE mode"

# Configuration
PT_MIN=30
PT_MAX=100000

OUTPUT_FOLDER="analysis_results_${MODE_TYPE}_${PT_MIN}_to_${PT_MAX}GeV"
SUMMARY_FILE="${OUTPUT_FOLDER}/analysis_summary_${MODE_TYPE}_${PT_MIN}_to_${PT_MAX}GeV.txt"

# Set ROOT macro based on mode
if [ "$MODE_TYPE" = "jet" ]; then
    ROOT_MACRO="plot_recotime_4jet.C"
else
    ROOT_MACRO="plot_recotime.C"
fi

mkdir -p $OUTPUT_FOLDER
echo "Created output directory: $OUTPUT_FOLDER"
echo "Using ROOT macro: $ROOT_MACRO"

# Define analysis tasks
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

# Mode-specific histogram analysis
if [ "$MODE_TYPE" = "event" ]; then
    # Extract eventCell histogram bin counts (event mode only)
    echo ""
    echo "Extracting eventCell histogram bin counts..."

    EVENT_COUNTS=()
    EVENT_COUNT_DESCRIPTIONS=()

    for i in "${!ANALYSIS_TASKS[@]}"; do
        TASK="${ANALYSIS_TASKS[i]}"
        IFS=',' read -r MODE INPUT_FILE OUTPUT_BASE FIT_MIN FIT_MAX DESCRIPTION <<< "$TASK"
        
        echo "  Checking eventCell histogram in: $INPUT_FILE"
        
        # Check if input file exists
        if [ ! -f "$INPUT_FILE" ]; then
            echo "    Warning: Input file not found. Skipping..."
            continue
        fi
        
        # Use direct ROOT command to extract bin count
        TEMP_OUTPUT=$(mktemp)
        {
            echo "TFile *f = TFile::Open(\"$INPUT_FILE\", \"READ\");"
            echo "if (!f || f->IsZombie()) { cout << \"ERROR: Cannot open file\" << endl; } else {"
            echo "  TH1F *h = (TH1F*)f->Get(\"eventCell\");"
            echo "  if (!h) { cout << \"ERROR: Cannot find eventCell histogram\" << endl; } else {"
            echo "    int bin_zero = h->FindBin(0.0);"
            echo "    double count_zero = h->GetBinContent(bin_zero);"
            echo "    cout << \"COUNT_AT_ZERO: \" << count_zero << endl;"
            echo "    double total_entries = h->GetEntries();"
            echo "    cout << \"TOTAL_ENTRIES: \" << total_entries << endl;"
            echo "  }"
            echo "  f->Close();"
            echo "}"
            echo ".q"
        } | timeout 60 root -l -b > $TEMP_OUTPUT 2>&1
        
        # Read the output and extract information
        ROOT_OUTPUT_CELL=$(cat $TEMP_OUTPUT)
        
        # Extract the count at x=0
        COUNT_AT_ZERO=$(echo "$ROOT_OUTPUT_CELL" | grep "COUNT_AT_ZERO:" | sed 's/COUNT_AT_ZERO: //')
        
        if [ ! -z "$COUNT_AT_ZERO" ]; then
            COUNT_INT=$(echo "$COUNT_AT_ZERO" | cut -d'.' -f1)
            EVENT_COUNTS+=("$COUNT_INT")
            EVENT_COUNT_DESCRIPTIONS+=("$DESCRIPTION")
            echo "    ✓ Found count at x=0: $COUNT_INT"
        else
            echo "    ✗ Failed to extract count"
        fi
        
        rm -f $TEMP_OUTPUT
    done

else
    # Extract jet width and matched jet width entries ratio (jet mode only)
    echo ""
    echo "Extracting jet width entries ratio..."

    JET_WIDTH_ENTRIES=()
    ALL_MATCHED_JET_WIDTH_ENTRIES=()
    JET_RATIOS=()
    JET_RATIO_DESCRIPTIONS=()

    for i in "${!ANALYSIS_TASKS[@]}"; do
        TASK="${ANALYSIS_TASKS[i]}"
        IFS=',' read -r MODE INPUT_FILE OUTPUT_BASE FIT_MIN FIT_MAX DESCRIPTION <<< "$TASK"
        
        echo "  Checking jet histograms in: $INPUT_FILE"
        
        # Check if input file exists
        if [ ! -f "$INPUT_FILE" ]; then
            echo "    Warning: Input file not found. Skipping..."
            continue
        fi
        
        # Use direct ROOT command to extract jet information
        TEMP_OUTPUT=$(mktemp)
        {
            echo "TFile *f = TFile::Open(\"$INPUT_FILE\", \"READ\");"
            echo "if (!f || f->IsZombie()) { cout << \"ERROR: Cannot open file\" << endl; } else {"
            echo "  TH1F *h_selected = (TH1F*)f->Get(\"selectedJetWidth\");"
            echo "  TH1F *h_all = (TH1F*)f->Get(\"allMatchedJetWidth\");"
            echo "  if (!h_selected) { cout << \"ERROR: Cannot find selectedJetWidth histogram\" << endl; }"
            echo "  if (!h_all) { cout << \"ERROR: Cannot find allMatchedJetWidth histogram\" << endl; }"
            echo "  if (h_selected && h_all) {"
            echo "    double selected_entries = h_selected->GetEntries();"
            echo "    double all_entries = h_all->GetEntries();"
            echo "    cout << \"SELECTED_JET_WIDTH_ENTRIES: \" << selected_entries << endl;"
            echo "    cout << \"ALL_MATCHED_JET_WIDTH_ENTRIES: \" << all_entries << endl;"
            echo "    if (all_entries > 0) {"
            echo "      double ratio = selected_entries / all_entries;"
            echo "      cout << \"JET_RATIO: \" << ratio << endl;"
            echo "    } else {"
            echo "      cout << \"JET_RATIO: 0\" << endl;"
            echo "    }"
            echo "  }"
            echo "  f->Close();"
            echo "}"
            echo ".q"
        } | timeout 60 root -l -b > $TEMP_OUTPUT 2>&1
        
        # Read the output and extract information
        ROOT_OUTPUT_JET=$(cat $TEMP_OUTPUT)
        
        echo "    ROOT output:"
        echo "$ROOT_OUTPUT_JET" | grep -E "(SELECTED_JET_WIDTH_ENTRIES|ALL_MATCHED_JET_WIDTH_ENTRIES|JET_RATIO|ERROR)" | sed 's/^/      /'
        
        # Extract the values
        SELECTED_ENTRIES=$(echo "$ROOT_OUTPUT_JET" | grep "SELECTED_JET_WIDTH_ENTRIES:" | sed 's/SELECTED_JET_WIDTH_ENTRIES: //')
        ALL_ENTRIES=$(echo "$ROOT_OUTPUT_JET" | grep "ALL_MATCHED_JET_WIDTH_ENTRIES:" | sed 's/ALL_MATCHED_JET_WIDTH_ENTRIES: //')
        RATIO=$(echo "$ROOT_OUTPUT_JET" | grep "JET_RATIO:" | sed 's/JET_RATIO: //')
        
        if [ ! -z "$RATIO" ]; then
            JET_WIDTH_ENTRIES+=("$SELECTED_ENTRIES")
            ALL_MATCHED_JET_WIDTH_ENTRIES+=("$ALL_ENTRIES")
            JET_RATIOS+=("$RATIO")
            JET_RATIO_DESCRIPTIONS+=("$DESCRIPTION")
            echo "    ✓ Found ratio: $RATIO (${SELECTED_ENTRIES}/${ALL_ENTRIES})"
        else
            echo "    ✗ Failed to extract jet ratio"
        fi
        
        rm -f $TEMP_OUTPUT
    done
fi

# Generate summary
echo ""
echo "Generating summary file: $SUMMARY_FILE"

cat > $SUMMARY_FILE << EOF
Analysis Summary ($MODE_TYPE mode)
================
Generated on: $(date)
Successful fit analyses: ${#MEANS[@]}/${#ANALYSIS_TASKS[@]}
EOF

if [ "$MODE_TYPE" = "event" ]; then
    cat >> $SUMMARY_FILE << EOF
Successful bin counts: ${#EVENT_COUNTS[@]}/${#ANALYSIS_TASKS[@]}
EOF
else
    cat >> $SUMMARY_FILE << EOF
Successful jet ratios: ${#JET_RATIOS[@]}/${#ANALYSIS_TASKS[@]}
EOF
fi

cat >> $SUMMARY_FILE << EOF

Fit Results:
EOF

for i in "${!DESCRIPTIONS[@]}"; do
    cat >> $SUMMARY_FILE << EOF
${DESCRIPTIONS[i]}: Mean=${MEANS[i]} ps, Sigma=${SIGMAS[i]} ps
EOF
done

if [ "$MODE_TYPE" = "event" ]; then
    cat >> $SUMMARY_FILE << EOF

EventCell Bin Counts:
EOF

    for i in "${!EVENT_COUNT_DESCRIPTIONS[@]}"; do
        cat >> $SUMMARY_FILE << EOF
${EVENT_COUNT_DESCRIPTIONS[i]}: ${EVENT_COUNTS[i]} events
EOF
    done
else
    cat >> $SUMMARY_FILE << EOF

Jet Width Entries Ratios:
EOF

    for i in "${!JET_RATIO_DESCRIPTIONS[@]}"; do
        cat >> $SUMMARY_FILE << EOF
${JET_RATIO_DESCRIPTIONS[i]}: ${JET_RATIOS[i]} (${JET_WIDTH_ENTRIES[i]}/${ALL_MATCHED_JET_WIDTH_ENTRIES[i]})
EOF
    done
fi

cat >> $SUMMARY_FILE << EOF

Python Arrays:
# Fit results
means = [$(IFS=','; echo "${MEANS[*]}")]
sigmas = [$(IFS=','; echo "${SIGMAS[*]}")]
fit_labels = [$(printf "'%s'," "${DESCRIPTIONS[@]}" | sed 's/,$//')]
EOF

if [ "$MODE_TYPE" = "event" ]; then
    cat >> $SUMMARY_FILE << EOF

# Event counts
event_counts = [$(IFS=','; echo "${EVENT_COUNTS[*]}")]
count_labels = [$(printf "'%s'," "${EVENT_COUNT_DESCRIPTIONS[@]}" | sed 's/,$//')]
EOF
else
    cat >> $SUMMARY_FILE << EOF

# Jet ratios
jet_ratios = [$(IFS=','; echo "${JET_RATIOS[*]}")]
selected_jet_width_entries = [$(IFS=','; echo "${JET_WIDTH_ENTRIES[*]}")]
all_matched_jet_width_entries = [$(IFS=','; echo "${ALL_MATCHED_JET_WIDTH_ENTRIES[*]}")]
ratio_labels = [$(printf "'%s'," "${JET_RATIO_DESCRIPTIONS[@]}" | sed 's/,$//')]
EOF
fi

echo ""
echo "=== Analysis Complete ($MODE_TYPE mode) ==="
echo "Fit results: ${#MEANS[@]}/${#ANALYSIS_TASKS[@]}"

if [ "$MODE_TYPE" = "event" ]; then
    echo "Event counts: ${#EVENT_COUNTS[@]}/${#ANALYSIS_TASKS[@]}"
else
    echo "Jet ratios: ${#JET_RATIOS[@]}/${#ANALYSIS_TASKS[@]}"
fi

echo ""
echo "Python-ready arrays:"
echo "# Fit results"
echo "means = [$(IFS=','; echo "${MEANS[*]}")]"
echo "sigmas = [$(IFS=','; echo "${SIGMAS[*]}")]"
echo "fit_labels = [$(printf "'%s'," "${DESCRIPTIONS[@]}" | sed 's/,$//')]"
echo ""

if [ "$MODE_TYPE" = "event" ]; then
    echo "# Event counts"
    echo "event_counts = [$(IFS=','; echo "${EVENT_COUNTS[*]}")]"
    echo "count_labels = [$(printf "'%s'," "${EVENT_COUNT_DESCRIPTIONS[@]}" | sed 's/,$//')]"
else
    echo "# Jet ratios"
    echo "jet_ratios = [$(IFS=','; echo "${JET_RATIOS[*]}")]"
    echo "ratio_labels = [$(printf "'%s'," "${JET_RATIO_DESCRIPTIONS[@]}" | sed 's/,$//')]"
fi

echo ""
echo "Files saved in: $OUTPUT_FOLDER"
