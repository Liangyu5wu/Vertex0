"""Easy-to-use script for running parameter sweep experiments."""

import os
import sys

def main():
    """Run different types of parameter sweeps."""
    
    # Base configuration file
    base_config = "config/configs/experiment2.yaml"
    
    print("Parameter Sweep Experiment Runner")
    print("=" * 40)
    print("\nAvailable sweep types:")
    print("1. Quick sweep (few parameters, fast)")
    print("2. Architecture sweep (model structure)")
    print("3. Training sweep (learning parameters)")
    print("4. Regularization sweep (dropout, etc.)")
    print("5. Dense layers sweep (final layers)")
    print("6. Custom sweep")
    
    choice = input("\nSelect sweep type (1-6): ")
    
    sweep_configs = {
        "1": ("quick", 20),
        "2": ("architecture", 50),
        "3": ("training", 30),
        "4": ("regularization", 25),
        "5": ("dense_layers", 15),
        "6": ("custom", None)
    }
    
    if choice not in sweep_configs:
        print("Invalid choice!")
        return
    
    grid_type, max_exp = sweep_configs[choice]
    
    if choice == "6":
        grid_type = input("Enter grid type: ")
        max_exp = input("Max experiments (or press Enter for no limit): ")
        max_exp = int(max_exp) if max_exp else None
    
    # Run parameter sweep
    cmd_parts = [
        "python", "scripts/parameter_sweep.py",
        "--base-config", base_config,
        "--grid-type", grid_type
    ]
    
    if max_exp is not None:
        cmd_parts.extend(["--max-experiments", str(max_exp)])
    
    print(f"\nRunning command: {' '.join(cmd_parts)}")
    print("This may take a while...\n")
    
    os.system(" ".join(cmd_parts))
    
    print("\nSweep completed! Run analysis with:")
    print("python scripts/analyze_sweep.py results/parameter_sweep_YYYYMMDD_HHMMSS/")


if __name__ == "__main__":
    main()