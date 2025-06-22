#!/bin/bash

# Script to batch process ROOT files and summarize results
# Usage: ./batch_plot_analysis.sh

# Configuration
OUTPUT_FOLDER="analysis_results_pt"
SUMMARY_FILE="${OUTPUT_FOLDER}/analysis_summary_pt.txt"
ROOT_MACRO="plot_recotime.C"

mkdir -p $OUTPUT_FOLDER
echo "Created output directory: $OUTPUT_FOLDER"

# Define analysis tasks
# Format: "mode,inputFile,outputBase,fitMin,fitMax,description"
# ANALYSIS_TASKS=(
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.01.root,jetwidth001,-120,120,jetwidth0.01"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.03.root,jetwidth003,-120,120,jetwidth0.03"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.05.root,jetwidth005,-120,120,jetwidth0.05"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.06.root,jetwidth006,-120,120,jetPt60"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.08.root,jetwidth008,-120,120,jetPt70"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.09.root,jetwidth009,-120,120,jetPt80"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.10.root,jetwidth010,-120,120,jetPt90"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.12.root,jetwidth012,-120,120,jetPt100"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.14.root,jetwidth014,-120,120,jetPt110"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.15.root,jetwidth015,-120,120,jetPt120"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.17.root,jetwidth017,-120,120,jetPt140"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.18.root,jetwidth018,-120,120,jetPt160"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.19.root,jetwidth019,-120,120,jetPt180"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.21.root,jetwidth021,-120,120,jetPt200"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.22.root,jetwidth022,-120,120,jetPt400"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.24.root,jetwidth024,-120,120,jetPt400"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.25.root,jetwidth025,-120,120,jetPt400"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.27.root,jetwidth027,-120,120,jetPt400"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.28.root,jetwidth028,-120,120,jetPt400"
#     "0,jetmatching_reconstruction_Eover2.0_jetPt30to100000_dR0.3_jetWidth0.00to0.30.root,jetwidth030,-120,120,jetPt400"
# )

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

# Extract eventCell histogram bin counts
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
        echo "    cout << \"HISTOGRAM_INFO: Bins=\" << h->GetNbinsX() << \", Min=\" << h->GetXaxis()->GetXmin() << \", Max=\" << h->GetXaxis()->GetXmax() << endl;"
        echo "    int bin_zero = h->FindBin(0.0);"
        echo "    double count_zero = h->GetBinContent(bin_zero);"
        echo "    cout << \"BIN_AT_ZERO: \" << bin_zero << endl;"
        echo "    cout << \"COUNT_AT_ZERO: \" << count_zero << endl;"
        echo "    // Also check first few bins"
        echo "    for(int i=1; i<=5; i++) {"
        echo "      double count = h->GetBinContent(i);"
        echo "      double binCenter = h->GetBinCenter(i);"
        echo "      cout << \"BIN_\" << i << \": center=\" << binCenter << \", count=\" << count << endl;"
        echo "    }"
        echo "    double total_entries = h->GetEntries();"
        echo "    cout << \"TOTAL_ENTRIES: \" << total_entries << endl;"
        echo "  }"
        echo "  f->Close();"
        echo "}"
        echo ".q"
    } | timeout 60 root -l -b > $TEMP_OUTPUT 2>&1
    
    # Read the output and extract information
    ROOT_OUTPUT_CELL=$(cat $TEMP_OUTPUT)
    
    echo "    ROOT output:"
    echo "$ROOT_OUTPUT_CELL" | grep -E "(HISTOGRAM_INFO|BIN_AT_ZERO|COUNT_AT_ZERO|BIN_|TOTAL_ENTRIES|ERROR)" | sed 's/^/      /'
    
    # Extract the count at x=0
    COUNT_AT_ZERO=$(echo "$ROOT_OUTPUT_CELL" | grep "COUNT_AT_ZERO:" | sed 's/COUNT_AT_ZERO: //')
    
    if [ ! -z "$COUNT_AT_ZERO" ]; then
        # Convert to integer (remove decimal part if present)
        COUNT_INT=$(echo "$COUNT_AT_ZERO" | cut -d'.' -f1)
        EVENT_COUNTS+=("$COUNT_INT")
        EVENT_COUNT_DESCRIPTIONS+=("$DESCRIPTION")
        echo "    ✓ Found count at x=0: $COUNT_INT"
    else
        echo "    ✗ Failed to extract count"
        
        # Check if we can extract total entries as fallback
        TOTAL_ENTRIES=$(echo "$ROOT_OUTPUT_CELL" | grep "TOTAL_ENTRIES:" | sed 's/TOTAL_ENTRIES: //')
        if [ ! -z "$TOTAL_ENTRIES" ]; then
            TOTAL_INT=$(echo "$TOTAL_ENTRIES" | cut -d'.' -f1)
            echo "    Found total entries: $TOTAL_INT"
            read -t 10 -p "    Use total entries as count? (y/n): " use_total
            if [ "$use_total" = "y" ]; then
                EVENT_COUNTS+=("$TOTAL_INT")
                EVENT_COUNT_DESCRIPTIONS+=("$DESCRIPTION")
                echo "    ✓ Using total entries: $TOTAL_INT"
            fi
        else
            # Manual entry option
            read -t 10 -p "    Enter 'm' for manual input: " choice
            if [ "$choice" = "m" ]; then
                read -p "      Enter event count: " manual_count
                if [ ! -z "$manual_count" ]; then
                    EVENT_COUNTS+=("$manual_count")
                    EVENT_COUNT_DESCRIPTIONS+=("$DESCRIPTION")
                    echo "    ✓ Manual count stored: $manual_count"
                fi
            fi
        fi
    fi
    
    # Cleanup
    rm -f $TEMP_OUTPUT
done

# Extract selectedJetCount histogram bin counts
echo ""
echo "Extracting selectedJetCount histogram bin counts..."

JET_COUNT_COUNTS=()
JET_COUNT_COUNT_DESCRIPTIONS=()

for i in "${!ANALYSIS_TASKS[@]}"; do
    TASK="${ANALYSIS_TASKS[i]}"
    IFS=',' read -r MODE INPUT_FILE OUTPUT_BASE FIT_MIN FIT_MAX DESCRIPTION <<< "$TASK"
    
    echo "  Checking selectedJetCount histogram in: $INPUT_FILE"
    
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
        echo "  TH1F *h = (TH1F*)f->Get(\"selectedJetCount\");"
        echo "  if (!h) { cout << \"ERROR: Cannot find selectedJetCount histogram\" << endl; } else {"
        echo "    cout << \"JETCOUNT_HISTOGRAM_INFO: Bins=\" << h->GetNbinsX() << \", Min=\" << h->GetXaxis()->GetXmin() << \", Max=\" << h->GetXaxis()->GetXmax() << endl;"
        echo "    int bin_zero = h->FindBin(0.0);"
        echo "    double count_zero = h->GetBinContent(bin_zero);"
        echo "    cout << \"JETCOUNT_BIN_AT_ZERO: \" << bin_zero << endl;"
        echo "    cout << \"JETCOUNT_COUNT_AT_ZERO: \" << count_zero << endl;"
        echo "    // Also check first few bins"
        echo "    for(int i=1; i<=5; i++) {"
        echo "      double count = h->GetBinContent(i);"
        echo "      double binCenter = h->GetBinCenter(i);"
        echo "      cout << \"JETCOUNT_BIN_\" << i << \": center=\" << binCenter << \", count=\" << count << endl;"
        echo "    }"
        echo "    double total_entries = h->GetEntries();"
        echo "    cout << \"JETCOUNT_TOTAL_ENTRIES: \" << total_entries << endl;"
        echo "  }"
        echo "  f->Close();"
        echo "}"
        echo ".q"
    } | timeout 60 root -l -b > $TEMP_OUTPUT 2>&1
    
    # Read the output and extract information
    ROOT_OUTPUT_JETCOUNT=$(cat $TEMP_OUTPUT)
    
    echo "    ROOT output:"
    echo "$ROOT_OUTPUT_JETCOUNT" | grep -E "(JETCOUNT_HISTOGRAM_INFO|JETCOUNT_BIN_AT_ZERO|JETCOUNT_COUNT_AT_ZERO|JETCOUNT_BIN_|JETCOUNT_TOTAL_ENTRIES|ERROR)" | sed 's/^/      /'
    
    # Extract the count at x=0
    JETCOUNT_COUNT_AT_ZERO=$(echo "$ROOT_OUTPUT_JETCOUNT" | grep "JETCOUNT_COUNT_AT_ZERO:" | sed 's/JETCOUNT_COUNT_AT_ZERO: //')
    
    if [ ! -z "$JETCOUNT_COUNT_AT_ZERO" ]; then
        # Convert to integer (remove decimal part if present)
        JETCOUNT_COUNT_INT=$(echo "$JETCOUNT_COUNT_AT_ZERO" | cut -d'.' -f1)
        JET_COUNT_COUNTS+=("$JETCOUNT_COUNT_INT")
        JET_COUNT_COUNT_DESCRIPTIONS+=("$DESCRIPTION")
        echo "    ✓ Found jet count at x=0: $JETCOUNT_COUNT_INT"
    else
        echo "    ✗ Failed to extract jet count"
        
        # Check if we can extract total entries as fallback
        JETCOUNT_TOTAL_ENTRIES=$(echo "$ROOT_OUTPUT_JETCOUNT" | grep "JETCOUNT_TOTAL_ENTRIES:" | sed 's/JETCOUNT_TOTAL_ENTRIES: //')
        if [ ! -z "$JETCOUNT_TOTAL_ENTRIES" ]; then
            JETCOUNT_TOTAL_INT=$(echo "$JETCOUNT_TOTAL_ENTRIES" | cut -d'.' -f1)
            echo "    Found total entries: $JETCOUNT_TOTAL_INT"
            read -t 10 -p "    Use total entries as jet count? (y/n): " use_total
            if [ "$use_total" = "y" ]; then
                JET_COUNT_COUNTS+=("$JETCOUNT_TOTAL_INT")
                JET_COUNT_COUNT_DESCRIPTIONS+=("$DESCRIPTION")
                echo "    ✓ Using total entries: $JETCOUNT_TOTAL_INT"
            fi
        else
            # Manual entry option
            read -t 10 -p "    Enter 'm' for manual input: " choice
            if [ "$choice" = "m" ]; then
                read -p "      Enter jet count: " manual_jet_count
                if [ ! -z "$manual_jet_count" ]; then
                    JET_COUNT_COUNTS+=("$manual_jet_count")
                    JET_COUNT_COUNT_DESCRIPTIONS+=("$DESCRIPTION")
                    echo "    ✓ Manual jet count stored: $manual_jet_count"
                fi
            fi
        fi
    fi
    
    # Cleanup
    rm -f $TEMP_OUTPUT
done

# Generate summary
echo ""
echo "Generating summary file: $SUMMARY_FILE"

cat > $SUMMARY_FILE << EOF
Analysis Summary
================
Generated on: $(date)
Successful fit analyses: ${#MEANS[@]}/${#ANALYSIS_TASKS[@]}
Successful bin counts: ${#EVENT_COUNTS[@]}/${#ANALYSIS_TASKS[@]}
Successful jet counts: ${#JET_COUNT_COUNTS[@]}/${#ANALYSIS_TASKS[@]}

Fit Results:
EOF

for i in "${!DESCRIPTIONS[@]}"; do
    cat >> $SUMMARY_FILE << EOF
${DESCRIPTIONS[i]}: Mean=${MEANS[i]} ps, Sigma=${SIGMAS[i]} ps
EOF
done

cat >> $SUMMARY_FILE << EOF

EventCell Bin Counts:
EOF

for i in "${!EVENT_COUNT_DESCRIPTIONS[@]}"; do
    cat >> $SUMMARY_FILE << EOF
${EVENT_COUNT_DESCRIPTIONS[i]}: ${EVENT_COUNTS[i]} events
EOF
done

cat >> $SUMMARY_FILE << EOF

SelectedJetCount Bin Counts:
EOF

for i in "${!JET_COUNT_COUNT_DESCRIPTIONS[@]}"; do
    cat >> $SUMMARY_FILE << EOF
${JET_COUNT_COUNT_DESCRIPTIONS[i]}: ${JET_COUNT_COUNTS[i]} events
EOF
done

cat >> $SUMMARY_FILE << EOF

Python Arrays:
# Fit results
means = [$(IFS=','; echo "${MEANS[*]}")]
sigmas = [$(IFS=','; echo "${SIGMAS[*]}")]
fit_labels = [$(printf "'%s'," "${DESCRIPTIONS[@]}" | sed 's/,$//')]

# Event counts
event_counts = [$(IFS=','; echo "${EVENT_COUNTS[*]}")]
count_labels = [$(printf "'%s'," "${EVENT_COUNT_DESCRIPTIONS[@]}" | sed 's/,$//')]

# Jet counts
jet_counts = [$(IFS=','; echo "${JET_COUNT_COUNTS[*]}")]
jet_count_labels = [$(printf "'%s'," "${JET_COUNT_COUNT_DESCRIPTIONS[@]}" | sed 's/,$//')]
EOF

echo ""
echo "=== Analysis Complete ==="
echo "Fit results: ${#MEANS[@]}/${#ANALYSIS_TASKS[@]}"
echo "Event counts: ${#EVENT_COUNTS[@]}/${#ANALYSIS_TASKS[@]}"
echo "Jet counts: ${#JET_COUNT_COUNTS[@]}/${#ANALYSIS_TASKS[@]}"
echo ""
echo "Python-ready arrays:"
echo "# Fit results"
echo "means = [$(IFS=','; echo "${MEANS[*]}")]"
echo "sigmas = [$(IFS=','; echo "${SIGMAS[*]}")]"
echo "fit_labels = [$(printf "'%s'," "${DESCRIPTIONS[@]}" | sed 's/,$//')]"
echo ""
echo "# Event counts"
echo "event_counts = [$(IFS=','; echo "${EVENT_COUNTS[*]}")]"
echo "count_labels = [$(printf "'%s'," "${EVENT_COUNT_DESCRIPTIONS[@]}" | sed 's/,$//')]"
echo ""
echo "# Jet counts"
echo "jet_counts = [$(IFS=','; echo "${JET_COUNT_COUNTS[*]}")]"
echo "jet_count_labels = [$(printf "'%s'," "${JET_COUNT_COUNT_DESCRIPTIONS[@]}" | sed 's/,$//')]"
echo ""
echo "Files saved in: $OUTPUT_FOLDER"
