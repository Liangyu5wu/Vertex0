#!/bin/bash

# Usage: ./submit_multi_jetmatching_jobs.sh
# This script submits multiple jobs with predefined parameter sets

BASE_DIR="/sdf/data/atlas/u/liangyu/vertextiming/user.scheong.mc21_14TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.SuperNtuple.e8514_s4345_r15583.20250219_Output"
SUBMIT_DIR="jetmatching_jobs_$(date +%Y%m%d_%H%M%S)"
mkdir -p $SUBMIT_DIR
echo "Created submission directory: $SUBMIT_DIR"

PT_MIN=30.0
PT_MAX=100000.0
ETA_CUT=100
EM1F_CUT=0.0
EM12F_CUT=0.0
LongWidthCut=100000.0
LongWidthSigmaCut=100000.0

# PARAMETER_SETS=(
#     "2.0,1,46,30.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,40.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,50.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,60.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,70.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,80.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,90.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,100.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,110.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,120.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,140.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,160.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,180.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,200.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
#     "2.0,1,46,400.0,${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
# )

PARAMETER_SETS=(
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.015, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.030, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.045, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.060, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.075, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.090, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.105, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.120, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.135, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.150, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.165, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.180, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.195, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.210, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.225, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.240, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.255, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.270, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.285, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
    "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,0.300, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}, 5.0, 0.3"
)




# PARAMETER_SETS=(
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.0"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.1"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.2"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.3"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.4"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.5"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.6"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.7"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.8"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 0.9"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 1.0"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${EM1F_CUT}, 1.1"
# )

# PARAMETER_SETS=(
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 1600, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 1650, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 1800, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 1900, ${LongWidthSigmaCut}"
# )

# PARAMETER_SETS=(
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, 0"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, 500"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, 1000"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, 1500"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, 2000"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, 2500"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, 3000"
# )

# PARAMETER_SETS=(
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, ${LongWidthCut}, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 1000, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 1250, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 1500, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 1750, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 2000, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 2250, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 2500, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 2750, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 3000, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 3250, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 3500, ${LongWidthSigmaCut}"
#     "2.0,1,46,${PT_MIN},${PT_MAX},0.3,-1,0.00,1.000, ${ETA_CUT}, ${EM1F_CUT}, ${EM12F_CUT}, 4000, ${LongWidthSigmaCut}"
# )


# Optional: Define custom job names for each parameter set
# If not defined, job names will be auto-generated
JOB_NAMES=(
    "pt30"
    "pt40"
    "pt50"
)

# Check if arrays have the same length
if [ ${#JOB_NAMES[@]} -ne 0 ] && [ ${#JOB_NAMES[@]} -ne ${#PARAMETER_SETS[@]} ]; then
    echo "Warning: Number of job names doesn't match number of parameter sets. Using auto-generated names."
    JOB_NAMES=()
fi

for i in "${!PARAMETER_SETS[@]}"; do
    PARAMS="${PARAMETER_SETS[i]}"
    
    if [ ${#JOB_NAMES[@]} -eq 0 ]; then
        JOB_NAME="jetmatch_job_$((i+1))"
    else
        JOB_NAME="${JOB_NAMES[i]}"
    fi
    
    PAYLOAD_SCRIPT="${SUBMIT_DIR}/payload_${JOB_NAME}.sh"
    BATCH_SCRIPT="${SUBMIT_DIR}/jobsubmitter_${JOB_NAME}.sh"
    
    echo "Creating job files for: $JOB_NAME with parameters: $PARAMS"
    
    # Create payload script
    cat > $PAYLOAD_SCRIPT << EOF
#!/bin/bash
source ${BASE_DIR}/setup9.sh
echo "Setup complete!"
cd ${BASE_DIR}
root -l <<ROOTEOF
.L processmu200_jetmatching_lepton_reco.C
processmu200_jetmatching_reco($PARAMS)
.q
ROOTEOF
echo "Job completed!"
EOF
    
    chmod +x $PAYLOAD_SCRIPT
    
    # Create batch submission script
    cat > $BATCH_SCRIPT << EOF
#!/bin/bash
#
#SBATCH --account=atlas:default
#SBATCH --partition=roma
#SBATCH --job-name=${JOB_NAME}
#SBATCH --output=${SUBMIT_DIR}/${JOB_NAME}-%j.txt
#SBATCH --error=${SUBMIT_DIR}/${JOB_NAME}-%j_error.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=4g
#SBATCH --time=10:00:00

unset KRB5CCNAME
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
export ALRB_CONT_CMDOPTS="-B /sdf,/fs,/lscratch"
export ALRB_CONT_RUNPAYLOAD="source ${PAYLOAD_SCRIPT}"

source \$ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh -c el9 –pwd \$PWD
EOF
    
    chmod +x $BATCH_SCRIPT
    
    echo "Submitting job: $JOB_NAME"
    sbatch $BATCH_SCRIPT
    
    # Optional: Add a small delay between submissions
    sleep 1
done

echo "All jobs have been submitted"
echo "Total jobs submitted: ${#PARAMETER_SETS[@]}"
echo "All submission files are saved in directory: $SUBMIT_DIR"

# Print summary
echo ""
echo "=== Job Summary ==="
for i in "${!PARAMETER_SETS[@]}"; do
    if [ ${#JOB_NAMES[@]} -eq 0 ]; then
        JOB_NAME="jetmatch_job_$((i+1))"
    else
        JOB_NAME="${JOB_NAMES[i]}"
    fi
    echo "Job $((i+1)): $JOB_NAME -> processmu200_jetmatching_reco(${PARAMETER_SETS[i]})"
done
