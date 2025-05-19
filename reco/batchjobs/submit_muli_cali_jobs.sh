#!/bin/bash

# Usage: ./submit_muli_cali_jobs.sh
# This script submits multiple jobs with predefined deltaR values

BASE_DIR="/sdf/data/atlas/u/liangyu/vertextiming/user.scheong.mc21_14TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.SuperNtuple.e8514_s4345_r15583.20250219_Output"

SUBMIT_DIR="job_submissions_$(date +%Y%m%d_%H%M%S)"
mkdir -p $SUBMIT_DIR
echo "Created submission directory: $SUBMIT_DIR"

DELTA_R_VALUES=("0.025" "0.03" "0.04" "0.05" "0.06" "0.07" "0.10" "0.15" "0.20")

for DELTA_R in "${DELTA_R_VALUES[@]}"; do

    JOB_NAME="job_deltaR_${DELTA_R}"
    BATCH_SCRIPT="${SUBMIT_DIR}/testbatch_${JOB_NAME}.sh"
    PAYLOAD_SCRIPT="${SUBMIT_DIR}/payload_${JOB_NAME}.sh"
    
    cat > $PAYLOAD_SCRIPT << EOF
#!/bin/bash
source ${BASE_DIR}/setup9.sh
echo "Setup complete!"
cd ${BASE_DIR}/reco
root -l <<ROOTEOF
.L processmu200_track_origin_cali.C
processmu200_cali(1,46,$DELTA_R)
.q
ROOTEOF
echo "Job completed with deltaR = $DELTA_R!"
EOF
    
    chmod +x $PAYLOAD_SCRIPT
    
    cat > $BATCH_SCRIPT << EOF
#!/bin/bash
#
#SBATCH --account=atlas:default
#SBATCH --partition=roma
#SBATCH --job-name=dR_${DELTA_R}
#SBATCH --output=${SUBMIT_DIR}/output_dR${DELTA_R}-%j.txt
#SBATCH --error=${SUBMIT_DIR}/error_dR${DELTA_R}-%j.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=4g
#SBATCH --time=10:00:00

unset KRB5CCNAME
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
export ALRB_CONT_CMDOPTS="-B /sdf,/fs,/lscratch"
export ALRB_CONT_RUNPAYLOAD="source /sdf/data/atlas/u/liangyu/vertextiming/user.scheong.mc21_14TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.SuperNtuple.e8514_s4345_r15583.20250219_Output/${PAYLOAD_SCRIPT}"

source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh -c el9 –pwd $PWD
EOF
    
    chmod +x $BATCH_SCRIPT
    
    echo "Submitting job for deltaR = $DELTA_R"
    sbatch $BATCH_SCRIPT
done

echo "All jobs have been submitted"
echo "All submission files are saved in directory: $SUBMIT_DIR"
