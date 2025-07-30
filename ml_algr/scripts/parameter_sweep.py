"""Parameter sweep script for transformer model hyperparameter optimization."""

import os
import sys
import json
import yaml
import time
import itertools
import pandas as pd
from datetime import datetime
from typing import Dict, List, Any
import subprocess

# Add src to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from config.transformer_config import TransformerConfig


class ParameterSweep:
    """Handle parameter sweep experiments for transformer model."""
    
    def __init__(self, base_config_path: str, output_dir: str = None):
        """
        Initialize parameter sweep.
        
        Args:
            base_config_path: Path to base configuration YAML file
            output_dir: Directory to save sweep results
        """
        self.base_config_path = base_config_path
        
        if output_dir is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            self.output_dir = f"results/parameter_sweep_{timestamp}"
        else:
            self.output_dir = output_dir
            
        self.experiments_dir = os.path.join(self.output_dir, "experiments")
        self.configs_dir = os.path.join(self.output_dir, "configs")
        
        # Create directories
        os.makedirs(self.experiments_dir, exist_ok=True)
        os.makedirs(self.configs_dir, exist_ok=True)
        
        # Load base configuration
        self.base_config = TransformerConfig.from_yaml(base_config_path)
        
        # Results storage
        self.results = []
        
    def define_parameter_grid(self) -> Dict[str, List]:
        """
        Define parameter grid for sweeping.
        
        Returns:
            Dictionary with parameter names and their values to test
        """
        parameter_grid = {
            # Architecture parameters
            'd_model': [32, 64, 128],
            'num_heads': [2, 4, 8],
            'num_transformer_blocks': [2, 3, 4],
            'dropout_rate': [0.05, 0.1, 0.2],
            
            # Dense layer parameters
            'vertex_dense_units': [8, 16, 32],
            
            # Training parameters
            'learning_rate': [1e-5, 5e-5, 1e-4, 2e-4],
            'batch_size': [32, 64, 128],
        }
        
        return parameter_grid
    
    def define_quick_grid(self) -> Dict[str, List]:
        """
        Define a smaller grid for quick testing.
        
        Returns:
            Dictionary with fewer parameter combinations
        """
        quick_grid = {
            # Focus on most important parameters
            'd_model': [32, 64, 128],
            'learning_rate': [1e-5, 5e-5, 1e-4],
            'num_heads': [2, 4, 8],
            'dropout_rate': [0.1, 0.2],
        }
        
        return quick_grid
    
    def define_focused_grids(self) -> Dict[str, Dict[str, List]]:
        """
        Define focused grids for specific parameter groups.
        
        Returns:
            Dictionary of parameter group grids
        """
        focused_grids = {
            'architecture': {
                'd_model': [32, 64, 128, 256],
                'num_heads': [2, 4, 8, 16],
                'num_transformer_blocks': [1, 2, 3, 4, 5],
                'dff': [64, 128, 256, 512],
            },
            
            'regularization': {
                'dropout_rate': [0.0, 0.05, 0.1, 0.15, 0.2, 0.3],
                'final_dropout_rates': [
                    [0.1, 0.05, 0.05],
                    [0.2, 0.1, 0.1], 
                    [0.3, 0.2, 0.1],
                    [0.4, 0.3, 0.2]
                ],
            },
            
            'training': {
                'learning_rate': [5e-6, 1e-5, 2e-5, 5e-5, 1e-4, 2e-4, 5e-4],
                'batch_size': [16, 32, 64, 128, 256],
                'lr_reduction_factor': [0.3, 0.5, 0.7, 0.9],
            },
            
            'dense_layers': {
                'vertex_dense_units': [4, 8, 16, 32, 64],
                'final_dense_units': [
                    [16, 8],
                    [32, 16, 8],
                    [64, 32, 16],
                    [32, 16],
                    [64, 32]
                ],
            }
        }
        
        return focused_grids
    
    def generate_experiment_configs(self, parameter_grid: Dict[str, List]) -> List[Dict]:
        """
        Generate all combinations of parameters.
        
        Args:
            parameter_grid: Dictionary of parameters and their values
            
        Returns:
            List of parameter combinations
        """
        # Get all parameter names and values
        param_names = list(parameter_grid.keys())
        param_values = list(parameter_grid.values())
        
        # Generate all combinations
        combinations = list(itertools.product(*param_values))
        
        # Convert to list of dictionaries
        experiment_configs = []
        for i, combination in enumerate(combinations):
            config_dict = dict(zip(param_names, combination))
            config_dict['experiment_id'] = f"exp_{i:04d}"
            experiment_configs.append(config_dict)
        
        print(f"Generated {len(experiment_configs)} experiment configurations")
        return experiment_configs
    
    def validate_parameter_combination(self, params: Dict) -> bool:
        """
        Validate parameter combination for consistency.
        
        Args:
            params: Parameter dictionary
            
        Returns:
            True if valid, False otherwise
        """
        # Check d_model is divisible by num_heads
        if 'd_model' in params and 'num_heads' in params:
            if params['d_model'] % params['num_heads'] != 0:
                return False
        
        # Check reasonable parameter ranges
        if 'learning_rate' in params:
            if params['learning_rate'] > 1e-3 or params['learning_rate'] < 1e-6:
                return False
        
        if 'dropout_rate' in params:
            if params['dropout_rate'] < 0 or params['dropout_rate'] > 0.5:
                return False
                
        return True
    
    def create_experiment_config(self, exp_id: str, params: Dict) -> str:
        """
        Create configuration file for an experiment.
        
        Args:
            exp_id: Experiment ID
            params: Parameter dictionary
            
        Returns:
            Path to created configuration file
        """
        # Start with base configuration
        config = TransformerConfig.from_yaml(self.base_config_path)
        
        # Update with experiment parameters
        for param_name, param_value in params.items():
            if param_name != 'experiment_id' and hasattr(config, param_name):
                setattr(config, param_name, param_value)
        
        # Set experiment-specific model name
        config.model_name = f"sweep_{exp_id}"
        
        # Adjust dependent parameters
        if 'dff' not in params:
            # Set dff to 2x d_model if not specified
            config.dff = config.d_model * 2
        
        # Save configuration
        config_path = os.path.join(self.configs_dir, f"{exp_id}.yaml")
        config.save_yaml(config_path)
        
        return config_path
    
    def run_single_experiment(self, exp_id: str, config_path: str) -> Dict:
        """
        Run a single experiment.
        
        Args:
            exp_id: Experiment ID
            config_path: Path to configuration file
            
        Returns:
            Dictionary with experiment results
        """
        print(f"\nRunning experiment {exp_id}...")
        
        # Record start time
        start_time = time.time()
        
        try:
            # Run training script
            cmd = [
                "python", "scripts/train.py",
                "--config-file", config_path,
                "--verbose", "0"  # Minimal output
            ]
            
            result = subprocess.run(
                cmd, 
                capture_output=True, 
                text=True, 
                cwd=os.path.dirname(os.path.dirname(__file__))
            )
            
            # Record end time
            end_time = time.time()
            training_time = end_time - start_time
            
            if result.returncode == 0:
                # Training successful, extract results
                results = self.extract_training_results(exp_id)
                results['status'] = 'success'
                results['training_time'] = training_time
                print(f"  ✓ Completed in {training_time:.1f}s")
            else:
                # Training failed
                results = {
                    'status': 'failed',
                    'error': result.stderr,
                    'training_time': training_time
                }
                print(f"  ✗ Failed after {training_time:.1f}s")
                
        except Exception as e:
            results = {
                'status': 'error',
                'error': str(e),
                'training_time': time.time() - start_time
            }
            print(f"  ✗ Error: {e}")
        
        return results
    
    def extract_training_results(self, exp_id: str) -> Dict:
        """
        Extract results from completed training.
        
        Args:
            exp_id: Experiment ID
            
        Returns:
            Dictionary with extracted metrics
        """
        model_dir = os.path.join("models", f"sweep_{exp_id}")
        
        results = {}
        
        try:
            # Load training history
            history_path = os.path.join(model_dir, "training_history.npz")
            if os.path.exists(history_path):
                history_data = dict(np.load(history_path))
                
                # Extract final metrics
                results['final_train_loss'] = float(history_data['loss'][-1])
                results['final_val_loss'] = float(history_data['val_loss'][-1])
                results['final_train_mae'] = float(history_data['mae'][-1])
                results['final_val_mae'] = float(history_data['val_mae'][-1])
                
                # Extract best metrics
                best_epoch = int(np.argmin(history_data['val_loss']))
                results['best_epoch'] = best_epoch
                results['best_val_loss'] = float(history_data['val_loss'][best_epoch])
                results['best_val_mae'] = float(history_data['val_mae'][best_epoch])
                
                # Check for RMSE
                if 'val_root_mean_squared_error' in history_data:
                    results['final_val_rmse'] = float(history_data['val_root_mean_squared_error'][-1])
                    results['best_val_rmse'] = float(history_data['val_root_mean_squared_error'][best_epoch])
                
                # Training stability metrics
                results['total_epochs'] = len(history_data['loss'])
                results['val_loss_std'] = float(np.std(history_data['val_loss']))
                
                # Convergence check
                last_5_epochs = history_data['val_loss'][-5:]
                results['converged'] = bool(np.std(last_5_epochs) < 0.001)
                
        except Exception as e:
            results['extraction_error'] = str(e)
        
        return results
    
    def run_parameter_sweep(self, grid_type: str = 'quick', max_experiments: int = None):
        """
        Run complete parameter sweep.
        
        Args:
            grid_type: Type of grid ('full', 'quick', or 'architecture', 'training', etc.)
            max_experiments: Maximum number of experiments to run
        """
        print(f"Starting parameter sweep with grid type: {grid_type}")
        print(f"Output directory: {self.output_dir}")
        
        # Get parameter grid
        if grid_type == 'full':
            parameter_grid = self.define_parameter_grid()
        elif grid_type == 'quick':
            parameter_grid = self.define_quick_grid()
        else:
            focused_grids = self.define_focused_grids()
            if grid_type in focused_grids:
                parameter_grid = focused_grids[grid_type]
            else:
                raise ValueError(f"Unknown grid type: {grid_type}")
        
        # Generate experiment configurations
        experiment_configs = self.generate_experiment_configs(parameter_grid)
        
        # Filter valid configurations
        valid_configs = []
        for config in experiment_configs:
            if self.validate_parameter_combination(config):
                valid_configs.append(config)
        
        print(f"Valid configurations: {len(valid_configs)}")
        
        # Limit number of experiments if specified
        if max_experiments is not None:
            valid_configs = valid_configs[:max_experiments]
            print(f"Limited to {len(valid_configs)} experiments")
        
        # Save experiment plan
        self.save_experiment_plan(valid_configs)
        
        # Run experiments
        for i, config in enumerate(valid_configs):
            exp_id = config['experiment_id']
            print(f"\n[{i+1}/{len(valid_configs)}] Running {exp_id}")
            
            # Create configuration file
            config_path = self.create_experiment_config(exp_id, config)
            
            # Run experiment
            results = self.run_single_experiment(exp_id, config_path)
            
            # Combine configuration and results
            full_results = {**config, **results}
            self.results.append(full_results)
            
            # Save intermediate results
            self.save_results()
            
            # Print progress summary
            successful = sum(1 for r in self.results if r.get('status') == 'success')
            print(f"Progress: {i+1}/{len(valid_configs)}, Success rate: {successful}/{i+1}")
        
        print(f"\nParameter sweep completed!")
        print(f"Total experiments: {len(self.results)}")
        print(f"Successful: {sum(1 for r in self.results if r.get('status') == 'success')}")
        
        # Generate summary
        self.generate_summary()
    
    def save_experiment_plan(self, configs: List[Dict]):
        """Save experiment plan to file."""
        plan_path = os.path.join(self.output_dir, "experiment_plan.json")
        with open(plan_path, 'w') as f:
            json.dump(configs, f, indent=2)
        print(f"Experiment plan saved to: {plan_path}")
    
    def save_results(self):
        """Save current results to file."""
        results_path = os.path.join(self.output_dir, "results.json")
        with open(results_path, 'w') as f:
            json.dump(self.results, f, indent=2)
        
        # Also save as CSV for easy analysis
        if self.results:
            df = pd.DataFrame(self.results)
            csv_path = os.path.join(self.output_dir, "results.csv")
            df.to_csv(csv_path, index=False)
    
    def generate_summary(self): 
        """Generate summary of results."""
        if not self.results:
            return
        
        successful_results = [r for r in self.results if r.get('status') == 'success']
        
        if not successful_results:
            print("No successful experiments to summarize.")
            return
        
        df = pd.DataFrame(successful_results)
        
        # Sort by best validation loss
        df_sorted = df.sort_values('best_val_loss')
        
        print(f"\n" + "="*60)
        print("TOP 10 EXPERIMENTS (by validation loss)")
        print("="*60)
        
        for i, (_, row) in enumerate(df_sorted.head(10).iterrows()):
            print(f"\n{i+1}. Experiment {row['experiment_id']}")
            print(f"   Val Loss: {row['best_val_loss']:.6f}")
            print(f"   Val MAE:  {row['best_val_mae']:.6f}")
            print(f"   Epoch:    {row['best_epoch']}")
            print(f"   d_model: {row.get('d_model', 'N/A')}, "
                  f"heads: {row.get('num_heads', 'N/A')}, "
                  f"lr: {row.get('learning_rate', 'N/A')}")
        
        # Save summary
        summary_path = os.path.join(self.output_dir, "summary.csv")
        df_sorted.to_csv(summary_path, index=False)
        print(f"\nFull summary saved to: {summary_path}")


def main():
    """Main function for parameter sweep."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Run parameter sweep for transformer model')
    parser.add_argument('--base-config', type=str, required=True,
                       help='Path to base configuration YAML file')
    parser.add_argument('--grid-type', type=str, default='quick',
                       choices=['full', 'quick', 'architecture', 'regularization', 'training', 'dense_layers'],
                       help='Type of parameter grid to use')
    parser.add_argument('--max-experiments', type=int, default=None,
                       help='Maximum number of experiments to run')
    parser.add_argument('--output-dir', type=str, default=None,
                       help='Output directory for results')
    
    args = parser.parse_args()
    
    # Create and run parameter sweep
    sweep = ParameterSweep(
        base_config_path=args.base_config,
        output_dir=args.output_dir
    )
    
    sweep.run_parameter_sweep(
        grid_type=args.grid_type,
        max_experiments=args.max_experiments
    )


if __name__ == "__main__":
    main()