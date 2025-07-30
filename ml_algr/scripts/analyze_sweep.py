"""Analysis script for parameter sweep results."""

import os
import sys
import json
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from typing import Dict, List, Optional
import argparse

# Set style for plots
plt.style.use('default')
sns.set_palette("husl")


class SweepAnalyzer:
    """Analyze parameter sweep results and generate insights."""
    
    def __init__(self, results_dir: str):
        """
        Initialize analyzer.
        
        Args:
            results_dir: Directory containing sweep results
        """
        self.results_dir = results_dir
        self.results_df = None
        self.successful_df = None
        self.plots_dir = os.path.join(results_dir, "analysis_plots")
        
        # Create plots directory
        os.makedirs(self.plots_dir, exist_ok=True)
        
        # Load results
        self.load_results()
    
    def load_results(self):
        """Load results from CSV file."""
        results_path = os.path.join(self.results_dir, "results.csv")
        
        if not os.path.exists(results_path):
            raise FileNotFoundError(f"Results file not found: {results_path}")
        
        self.results_df = pd.read_csv(results_path)
        
        # Filter successful experiments
        self.successful_df = self.results_df[
            self.results_df['status'] == 'success'
        ].copy()
        
        print(f"Loaded {len(self.results_df)} total experiments")
        print(f"Successful experiments: {len(self.successful_df)}")
        print(f"Success rate: {len(self.successful_df)/len(self.results_df)*100:.1f}%")
    
    def print_summary_statistics(self):
        """Print summary statistics of the sweep."""
        if len(self.successful_df) == 0:
            print("No successful experiments to analyze.")
            return
        
        print("\n" + "="*60)
        print("PARAMETER SWEEP SUMMARY STATISTICS")
        print("="*60)
        
        # Best experiment
        best_exp = self.successful_df.loc[self.successful_df['best_val_loss'].idxmin()]
        print(f"\nBest Experiment: {best_exp['experiment_id']}")
        print(f"  Validation Loss: {best_exp['best_val_loss']:.6f}")
        print(f"  Validation MAE:  {best_exp['best_val_mae']:.6f}")
        if 'best_val_rmse' in best_exp:
            print(f"  Validation RMSE: {best_exp['best_val_rmse']:.6f}")
        print(f"  Training Time:   {best_exp.get('training_time', 0):.1f}s")
        
        # Parameter values for best experiment
        param_cols = ['d_model', 'num_heads', 'num_transformer_blocks', 
                     'dropout_rate', 'learning_rate', 'batch_size', 'vertex_dense_units']
        print(f"  Parameters:")
        for param in param_cols:
            if param in best_exp:
                print(f"    {param}: {best_exp[param]}")
        
        # Performance statistics
        print(f"\nPerformance Statistics:")
        print(f"  Val Loss - Mean: {self.successful_df['best_val_loss'].mean():.6f}")
        print(f"  Val Loss - Std:  {self.successful_df['best_val_loss'].std():.6f}")
        print(f"  Val Loss - Min:  {self.successful_df['best_val_loss'].min():.6f}")
        print(f"  Val Loss - Max:  {self.successful_df['best_val_loss'].max():.6f}")
        
        print(f"  Val MAE - Mean:  {self.successful_df['best_val_mae'].mean():.6f}")
        print(f"  Val MAE - Std:   {self.successful_df['best_val_mae'].std():.6f}")
        print(f"  Val MAE - Min:   {self.successful_df['best_val_mae'].min():.6f}")
        print(f"  Val MAE - Max:   {self.successful_df['best_val_mae'].max():.6f}")
        
        # Training time statistics
        if 'training_time' in self.successful_df.columns:
            print(f"  Training Time - Mean: {self.successful_df['training_time'].mean():.1f}s")
            print(f"  Training Time - Min:  {self.successful_df['training_time'].min():.1f}s")
            print(f"  Training Time - Max:  {self.successful_df['training_time'].max():.1f}s")
    
    def analyze_parameter_importance(self) -> Dict[str, float]:
        """
        Analyze parameter importance using correlation with performance.
        
        Returns:
            Dictionary with parameter importance scores
        """
        param_cols = []
        for col in self.successful_df.columns:
            if col in ['d_model', 'num_heads', 'num_transformer_blocks', 
                      'dropout_rate', 'learning_rate', 'batch_size', 
                      'vertex_dense_units', 'dff']:
                if self.successful_df[col].nunique() > 1:  # Only varied parameters
                    param_cols.append(col)
        
        if not param_cols:
            return {}
        
        # Calculate correlation with validation loss
        correlations = {}
        for param in param_cols:
            corr = self.successful_df[param].corr(self.successful_df['best_val_loss'])
            correlations[param] = abs(corr)  # Use absolute correlation
        
        # Sort by importance
        importance = dict(sorted(correlations.items(), key=lambda x: x[1], reverse=True))
        
        print(f"\n" + "="*60)
        print("PARAMETER IMPORTANCE (correlation with validation loss)")
        print("="*60)
        for param, score in importance.items():
            print(f"  {param}: {score:.4f}")
        
        return importance
    
    def plot_parameter_distributions(self):
        """Plot distributions of parameter values."""
        param_cols = []
        for col in self.successful_df.columns:
            if col in ['d_model', 'num_heads', 'num_transformer_blocks', 
                      'dropout_rate', 'learning_rate', 'batch_size', 
                      'vertex_dense_units', 'dff']:
                if self.successful_df[col].nunique() > 1:
                    param_cols.append(col)
        
        if not param_cols:
            return
        
        n_params = len(param_cols)
        n_cols = min(3, n_params)
        n_rows = (n_params + n_cols - 1) // n_cols
        
        fig, axes = plt.subplots(n_rows, n_cols, figsize=(5*n_cols, 4*n_rows))
        if n_params == 1:
            axes = [axes]
        elif n_rows == 1:
            axes = axes.flatten()
        else:
            axes = axes.flatten()
        
        for i, param in enumerate(param_cols):
            ax = axes[i]
            
            # Create scatter plot colored by performance
            scatter = ax.scatter(
                self.successful_df[param], 
                self.successful_df['best_val_loss'],
                c=self.successful_df['best_val_loss'], 
                cmap='viridis_r', 
                alpha=0.7
            )
            
            ax.set_xlabel(param)
            ax.set_ylabel('Best Validation Loss')
            ax.set_title(f'{param} vs Performance')
            
            # Add colorbar
            plt.colorbar(scatter, ax=ax, label='Val Loss')
            
            # Add trend line if parameter is continuous
            if self.successful_df[param].dtype in ['float64', 'int64']:
                z = np.polyfit(self.successful_df[param], self.successful_df['best_val_loss'], 1)
                p = np.poly1d(z)
                ax.plot(self.successful_df[param], p(self.successful_df[param]), "r--", alpha=0.8)
        
        # Remove empty subplots
        for i in range(n_params, len(axes)):
            fig.delaxes(axes[i])
        
        plt.tight_layout()
        plt.savefig(os.path.join(self.plots_dir, 'parameter_distributions.png'), 
                   dpi=300, bbox_inches='tight')
        plt.close()
        print(f"Parameter distributions plot saved")
    
    def plot_performance_heatmap(self):
        """Plot heatmap of performance for parameter pairs."""
        # Find the most important parameters
        param_cols = []
        for col in self.successful_df.columns:
            if col in ['d_model', 'num_heads', 'learning_rate', 'dropout_rate', 'batch_size']:
                if self.successful_df[col].nunique() > 1:
                    param_cols.append(col)
        
        if len(param_cols) < 2:
            return
        
        # Create heatmaps for top parameter pairs
        n_pairs = min(6, len(param_cols) * (len(param_cols) - 1) // 2)
        
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        axes = axes.flatten()
        
        pair_idx = 0
        for i in range(len(param_cols)):
            for j in range(i + 1, len(param_cols)):
                if pair_idx >= n_pairs:
                    break
                
                param1, param2 = param_cols[i], param_cols[j]
                ax = axes[pair_idx]
                
                # Create pivot table for heatmap
                pivot_data = self.successful_df.pivot_table(
                    values='best_val_loss',
                    index=param1,
                    columns=param2,
                    aggfunc='mean'
                )
                
                # Plot heatmap
                sns.heatmap(pivot_data, annot=True, fmt='.4f', cmap='viridis_r', ax=ax)
                ax.set_title(f'{param1} vs {param2}')
                
                pair_idx += 1
                
                if pair_idx >= n_pairs:
                    break
        
        # Remove empty subplots
        for i in range(pair_idx, len(axes)):
            fig.delaxes(axes[i])
        
        plt.tight_layout()
        plt.savefig(os.path.join(self.plots_dir, 'performance_heatmaps.png'), 
                   dpi=300, bbox_inches='tight')
        plt.close()
        print(f"Performance heatmaps saved")
    
    def plot_training_efficiency(self):
        """Plot training efficiency (performance vs time)."""
        if 'training_time' not in self.successful_df.columns:
            return
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
        
        # Performance vs Training Time
        scatter = ax1.scatter(
            self.successful_df['training_time'],
            self.successful_df['best_val_loss'],
            c=self.successful_df['best_val_mae'],
            cmap='viridis_r',
            alpha=0.7,
            s=50
        )
        ax1.set_xlabel('Training Time (seconds)')
        ax1.set_ylabel('Best Validation Loss')
        ax1.set_title('Performance vs Training Time')
        plt.colorbar(scatter, ax=ax1, label='Val MAE')
        
        # Efficiency Score (1/loss per second)
        efficiency = 1 / (self.successful_df['best_val_loss'] * self.successful_df['training_time'])
        
        # Top experiments by efficiency
        top_efficient = self.successful_df.loc[efficiency.nlargest(10).index]
        
        ax2.scatter(
            top_efficient['training_time'],
            top_efficient['best_val_loss'],
            c='red',
            s=100,
            alpha=0.8,
            label='Top 10 Efficient'
        )
        ax2.scatter(
            self.successful_df['training_time'],
            self.successful_df['best_val_loss'],
            c='lightblue',
            alpha=0.5,
            s=30,
            label='All experiments'
        )
        ax2.set_xlabel('Training Time (seconds)')
        ax2.set_ylabel('Best Validation Loss')
        ax2.set_title('Most Efficient Experiments')
        ax2.legend()
        
        plt.tight_layout()
        plt.savefig(os.path.join(self.plots_dir, 'training_efficiency.png'), 
                   dpi=300, bbox_inches='tight')
        plt.close()
        print(f"Training efficiency plot saved")
    
    def plot_convergence_analysis(self):
        """Analyze convergence patterns."""
        if 'total_epochs' not in self.successful_df.columns:
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        
        # Epochs to convergence vs performance
        ax1 = axes[0, 0]
        scatter = ax1.scatter(
            self.successful_df['best_epoch'],
            self.successful_df['best_val_loss'],
            c=self.successful_df['total_epochs'],
            cmap='plasma',
            alpha=0.7
        )
        ax1.set_xlabel('Best Epoch')
        ax1.set_ylabel('Best Validation Loss')
        ax1.set_title('Convergence Speed vs Performance')
        plt.colorbar(scatter, ax=ax1, label='Total Epochs')
        
        # Learning rate vs convergence
        if 'learning_rate' in self.successful_df.columns:
            ax2 = axes[0, 1]
            ax2.scatter(
                self.successful_df['learning_rate'],
                self.successful_df['best_epoch'],
                c=self.successful_df['best_val_loss'],
                cmap='viridis_r',
                alpha=0.7
            )
            ax2.set_xlabel('Learning Rate')
            ax2.set_ylabel('Best Epoch')
            ax2.set_title('Learning Rate vs Convergence Speed')
            ax2.set_xscale('log')
        
        # Overfitting analysis
        if 'final_val_loss' in self.successful_df.columns:
            ax3 = axes[1, 0]
            overfitting = self.successful_df['final_val_loss'] - self.successful_df['best_val_loss']
            ax3.scatter(
                self.successful_df['best_val_loss'],
                overfitting,
                alpha=0.7
            )
            ax3.set_xlabel('Best Validation Loss')
            ax3.set_ylabel('Overfitting (Final - Best Val Loss)')
            ax3.set_title('Overfitting Analysis')
            ax3.axhline(y=0, color='red', linestyle='--', alpha=0.5)
        
        # Stability analysis
        if 'val_loss_std' in self.successful_df.columns:
            ax4 = axes[1, 1]
            ax4.scatter(
                self.successful_df['best_val_loss'],
                self.successful_df['val_loss_std'],
                alpha=0.7
            )
            ax4.set_xlabel('Best Validation Loss')
            ax4.set_ylabel('Validation Loss Std')
            ax4.set_title('Training Stability')
        
        plt.tight_layout()
        plt.savefig(os.path.join(self.plots_dir, 'convergence_analysis.png'), 
                   dpi=300, bbox_inches='tight')
        plt.close()
        print(f"Convergence analysis plot saved")
    
    def generate_recommendations(self) -> Dict[str, any]:
        """Generate recommendations based on analysis."""
        recommendations = {}
        
        if len(self.successful_df) == 0:
            return recommendations
        
        # Best overall configuration
        best_exp = self.successful_df.loc[self.successful_df['best_val_loss'].idxmin()]
        recommendations['best_config'] = {
            'experiment_id': best_exp['experiment_id'],
            'validation_loss': best_exp['best_val_loss'],
            'parameters': {}
        }
        
        # Extract best parameters
        param_cols = ['d_model', 'num_heads', 'num_transformer_blocks', 
                     'dropout_rate', 'learning_rate', 'batch_size', 'vertex_dense_units']
        for param in param_cols:
            if param in best_exp:
                recommendations['best_config']['parameters'][param] = best_exp[param]
        
        # Most efficient configuration (best performance per training time)
        if 'training_time' in self.successful_df.columns:
            efficiency = 1 / (self.successful_df['best_val_loss'] * self.successful_df['training_time'])
            most_efficient = self.successful_df.loc[efficiency.idxmax()]
            recommendations['most_efficient'] = {
                'experiment_id': most_efficient['experiment_id'],
                'validation_loss': most_efficient['best_val_loss'],
                'training_time': most_efficient['training_time'],
                'efficiency_score': efficiency.max()
            }
        
        # Parameter ranges for good performance (top 25%)
        top_25_percent = self.successful_df.nsmallest(
            int(len(self.successful_df) * 0.25), 'best_val_loss'
        )
        
        recommendations['good_parameter_ranges'] = {}
        for param in param_cols:
            if param in top_25_percent.columns and top_25_percent[param].nunique() > 1:
                recommendations['good_parameter_ranges'][param] = {
                    'min': top_25_percent[param].min(),
                    'max': top_25_percent[param].max(),
                    'mean': top_25_percent[param].mean(),
                    'std': top_25_percent[param].std()
                }
        
        return recommendations
    
    def save_analysis_report(self):
        """Save comprehensive analysis report."""
        report_path = os.path.join(self.results_dir, "analysis_report.txt")
        
        with open(report_path, 'w') as f:
            f.write("PARAMETER SWEEP ANALYSIS REPORT\n")
            f.write("="*50 + "\n\n")
            
            # Basic statistics
            f.write(f"Total experiments: {len(self.results_df)}\n")
            f.write(f"Successful experiments: {len(self.successful_df)}\n")
            f.write(f"Success rate: {len(self.successful_df)/len(self.results_df)*100:.1f}%\n\n")
            
            if len(self.successful_df) > 0:
                # Best experiment
                best_exp = self.successful_df.loc[self.successful_df['best_val_loss'].idxmin()]
                f.write(f"BEST EXPERIMENT: {best_exp['experiment_id']}\n")
                f.write(f"Validation Loss: {best_exp['best_val_loss']:.6f}\n")
                f.write(f"Validation MAE: {best_exp['best_val_mae']:.6f}\n\n")
                
                # Parameter importance
                importance = self.analyze_parameter_importance()
                f.write("PARAMETER IMPORTANCE:\n")
                for param, score in importance.items():
                    f.write(f"  {param}: {score:.4f}\n")
                f.write("\n")
                
                # Recommendations
                recommendations = self.generate_recommendations()
                f.write("RECOMMENDATIONS:\n")
                f.write(json.dumps(recommendations, indent=2))
        
        print(f"Analysis report saved to: {report_path}")
    
    def run_complete_analysis(self):
        """Run complete analysis pipeline."""
        print("Running complete parameter sweep analysis...")
        
        # Print summary statistics
        self.print_summary_statistics()
        
        # Analyze parameter importance
        self.analyze_parameter_importance()
        
        # Generate all plots
        print("\nGenerating analysis plots...")
        self.plot_parameter_distributions()
        self.plot_performance_heatmap()
        self.plot_training_efficiency()
        self.plot_convergence_analysis()
        
        # Generate recommendations
        recommendations = self.generate_recommendations()
        
        # Save analysis report
        self.save_analysis_report()
        
        print(f"\nAnalysis complete! Results saved to: {self.plots_dir}")
        
        return recommendations


def main():
    """Main function for sweep analysis."""
    parser = argparse.ArgumentParser(description='Analyze parameter sweep results')
    parser.add_argument('results_dir', type=str,
                       help='Directory containing sweep results')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.results_dir):
        print(f"Error: Results directory not found: {args.results_dir}")
        return
    
    # Create analyzer and run analysis
    analyzer = SweepAnalyzer(args.results_dir)
    recommendations = analyzer.run_complete_analysis()
    
    # Print top recommendations
    if recommendations:
        print("\n" + "="*60)
        print("TOP RECOMMENDATIONS")
        print("="*60)
        
        if 'best_config' in recommendations:
            best = recommendations['best_config']
            print(f"\nBest Configuration: {best['experiment_id']}")
            print(f"  Validation Loss: {best['validation_loss']:.6f}")
            print(f"  Parameters:")
            for param, value in best['parameters'].items():
                print(f"    {param}: {value}")


if __name__ == "__main__":
    main()