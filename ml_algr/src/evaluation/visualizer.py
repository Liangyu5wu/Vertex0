"""Visualization utilities for model evaluation."""

import os
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, Any, Optional, Tuple
import seaborn as sns

from config.base_config import BaseConfig


class Visualizer:
    """Handle visualization of training and evaluation results."""
    
    def __init__(self, config: BaseConfig):
        """
        Initialize visualizer.
        
        Args:
            config: Configuration object
        """
        self.config = config
        
        # Set matplotlib parameters for better plots
        plt.rcParams['figure.dpi'] = 300
        plt.rcParams['savefig.dpi'] = 300
        plt.rcParams['font.size'] = 10
        plt.rcParams['axes.titlesize'] = 12
        plt.rcParams['axes.labelsize'] = 10
        plt.rcParams['xtick.labelsize'] = 9
        plt.rcParams['ytick.labelsize'] = 9
        plt.rcParams['legend.fontsize'] = 9
        
    def plot_training_history(
        self, 
        history: Dict[str, np.ndarray], 
        save_path: Optional[str] = None
    ):
        """
        Plot training history metrics.
        
        Args:
            history: Training history dictionary
            save_path: Path to save the plot
        """
        if save_path is None:
            save_path = os.path.join(self.config.plots_dir, "training_metrics.png")
        
        # Determine number of subplots needed
        metrics = [key for key in history.keys() if not key.startswith('val_')]
        n_metrics = len(metrics)
        
        fig, axes = plt.subplots(1, n_metrics, figsize=(5 * n_metrics, 5))
        if n_metrics == 1:
            axes = [axes]
        
        for i, metric in enumerate(metrics):
            ax = axes[i]
            
            # Plot training metric
            ax.plot(history[metric], label=f'Training {metric.upper()}', linewidth=2)
            
            # Plot validation metric if available
            val_metric = f'val_{metric}'
            if val_metric in history:
                ax.plot(history[val_metric], label=f'Validation {metric.upper()}', linewidth=2)
            
            ax.set_title(f'Model {metric.upper()}')
            ax.set_xlabel('Epoch')
            ax.set_ylabel(metric.upper())
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            # Find best epoch for validation metric
            if val_metric in history:
                best_epoch = np.argmin(history[val_metric]) if 'loss' in metric else np.argmax(history[val_metric])
                best_val = history[val_metric][best_epoch]
                ax.axvline(x=best_epoch, color='red', linestyle='--', alpha=0.7)
                ax.text(0.02, 0.98, f'Best: {best_val:.4f} (epoch {best_epoch + 1})', 
                       transform=ax.transAxes, verticalalignment='top',
                       bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
        
        plt.tight_layout()
        plt.savefig(save_path, bbox_inches='tight')
        print(f"Training history plot saved to: {save_path}")
        plt.close()
    
    def plot_prediction_results(
        self, 
        y_true: np.ndarray, 
        y_pred: np.ndarray, 
        metrics: Dict[str, float],
        save_path: Optional[str] = None
    ):
        """
        Plot predicted vs actual values scatter plot.
        
        Args:
            y_true: True values
            y_pred: Predicted values
            metrics: Evaluation metrics
            save_path: Path to save the plot
        """
        if save_path is None:
            save_path = os.path.join(self.config.plots_dir, "prediction_results.png")
        
        plt.figure(figsize=(10, 8))
        
        # Create scatter plot with alpha for density
        plt.scatter(y_true, y_pred, alpha=0.6, s=20)
        
        # Perfect prediction line
        min_val = min(np.min(y_true), np.min(y_pred))
        max_val = max(np.max(y_true), np.max(y_pred))
        plt.plot([min_val, max_val], [min_val, max_val], 'r--', linewidth=2, 
                label='Perfect Prediction')
        
        # Add metrics text box
        metrics_text = f"R² = {metrics['r_squared']:.4f}\n"
        metrics_text += f"RMSE = {metrics['rmse']:.4f}\n"
        metrics_text += f"MAE = {metrics['mae']:.4f}\n"
        metrics_text += f"Correlation = {metrics['correlation']:.4f}"
        
        plt.text(0.05, 0.95, metrics_text, transform=plt.gca().transAxes, 
                verticalalignment='top', bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
        
        plt.title(f'{self.config.model_name}: Predicted vs Actual Values')
        plt.xlabel('Actual Vertex Time')
        plt.ylabel('Predicted Vertex Time')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(save_path, bbox_inches='tight')
        print(f"Prediction results plot saved to: {save_path}")
        plt.close()
    
    def plot_histogram_comparison(
        self, 
        y_true: np.ndarray, 
        y_pred: np.ndarray,
        save_path: Optional[str] = None
    ):
        """
        Plot histogram comparison of actual vs predicted values.
        
        Args:
            y_true: True values
            y_pred: Predicted values
            save_path: Path to save the plot
        """
        if save_path is None:
            save_path = os.path.join(self.config.plots_dir, "histogram_comparison.png")
        
        plt.figure(figsize=(12, 8))
        
        # Calculate statistics
        true_mean = np.mean(y_true)
        true_std = np.std(y_true)
        true_rms = np.sqrt(np.mean(np.square(y_true)))
        
        pred_mean = np.mean(y_pred)
        pred_std = np.std(y_pred)
        pred_rms = np.sqrt(np.mean(np.square(y_pred)))
        
        # Determine bins
        min_val = min(np.min(y_true), np.min(y_pred))
        max_val = max(np.max(y_true), np.max(y_pred))
        n_bins = int(np.ceil(np.log2(len(y_true))) + 1)
        bins = np.linspace(min_val, max_val, n_bins)
        
        # Plot histograms
        plt.hist(y_true, bins=bins, alpha=0.6, 
                label=f'Actual (μ={true_mean:.4f}, σ={true_std:.4f}, RMS={true_rms:.4f})', 
                color='blue', density=True)
        plt.hist(y_pred, bins=bins, alpha=0.6, 
                label=f'Predicted (μ={pred_mean:.4f}, σ={pred_std:.4f}, RMS={pred_rms:.4f})', 
                color='red', density=True)
        
        plt.xlabel('Vertex Time')
        plt.ylabel('Density')
        plt.title(f'{self.config.model_name}: Distribution Comparison')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(save_path, bbox_inches='tight')
        print(f"Histogram comparison plot saved to: {save_path}")
        plt.close()
    
    def plot_error_distribution(
        self, 
        y_true: np.ndarray, 
        y_pred: np.ndarray,
        save_path: Optional[str] = None
    ):
        """
        Plot distribution of prediction errors.
        
        Args:
            y_true: True values
            y_pred: Predicted values
            save_path: Path to save the plot
        """
        if save_path is None:
            save_path = os.path.join(self.config.plots_dir, "error_distribution.png")
        
        errors = y_pred - y_true
        error_mean = np.mean(errors)
        error_std = np.std(errors)
        
        plt.figure(figsize=(10, 8))
        
        # Plot histogram
        plt.hist(errors, bins=50, alpha=0.7, color='green', edgecolor='black', density=True)
        
        # Add vertical lines for statistics
        plt.axvline(x=error_mean, color='k', linestyle='-', linewidth=2, 
                   label=f'Mean: {error_mean:.4f}')
        plt.axvline(x=error_mean + error_std, color='k', linestyle=':', linewidth=2, 
                   label=f'+1σ: {error_std:.4f}')
        plt.axvline(x=error_mean - error_std, color='k', linestyle=':', linewidth=2, 
                   label=f'-1σ')
        
        # Overlay normal distribution for comparison
        x_norm = np.linspace(errors.min(), errors.max(), 100)
        y_norm = (1 / (error_std * np.sqrt(2 * np.pi))) * np.exp(-0.5 * ((x_norm - error_mean) / error_std) ** 2)
        plt.plot(x_norm, y_norm, 'r--', linewidth=2, label='Normal Distribution')
        
        plt.xlabel('Prediction Error (Predicted - Actual)')
        plt.ylabel('Density')
        plt.title(f'{self.config.model_name}: Error Distribution')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(save_path, bbox_inches='tight')
        print(f"Error distribution plot saved to: {save_path}")
        plt.close()
    
    def plot_residuals_vs_predicted(
        self, 
        y_true: np.ndarray, 
        y_pred: np.ndarray,
        save_path: Optional[str] = None
    ):
        """
        Plot residuals vs predicted values to check for patterns.
        
        Args:
            y_true: True values
            y_pred: Predicted values
            save_path: Path to save the plot
        """
        if save_path is None:
            save_path = os.path.join(self.config.plots_dir, "residuals_vs_predicted.png")
        
        residuals = y_pred - y_true
        
        plt.figure(figsize=(10, 8))
        plt.scatter(y_pred, residuals, alpha=0.6, s=20)
        plt.axhline(y=0, color='r', linestyle='--', linewidth=2)
        
        plt.xlabel('Predicted Values')
        plt.ylabel('Residuals (Predicted - Actual)')
        plt.title(f'{self.config.model_name}: Residuals vs Predicted Values')
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(save_path, bbox_inches='tight')
        print(f"Residuals plot saved to: {save_path}")
        plt.close()
    
    def create_comprehensive_evaluation_plots(
        self, 
        y_true: np.ndarray, 
        y_pred: np.ndarray, 
        metrics: Dict[str, float],
        training_history: Optional[Dict[str, np.ndarray]] = None
    ):
        """
        Create all evaluation plots.
        
        Args:
            y_true: True values
            y_pred: Predicted values
            metrics: Evaluation metrics
            training_history: Optional training history for plotting
        """
        # Ensure plots directory exists
        os.makedirs(self.config.plots_dir, exist_ok=True)
        
        print("Creating evaluation plots...")
        
        # Plot training history if provided
        if training_history is not None:
            self.plot_training_history(training_history)
        
        # Plot prediction results
        self.plot_prediction_results(y_true, y_pred, metrics)
        
        # Plot histogram comparison
        self.plot_histogram_comparison(y_true, y_pred)
        
        # Plot error distribution
        self.plot_error_distribution(y_true, y_pred)
        
        # Plot residuals
        self.plot_residuals_vs_predicted(y_true, y_pred)
        
        print(f"All evaluation plots saved to: {self.config.plots_dir}")
    
    def plot_feature_importance(
        self, 
        feature_names: list, 
        importance_scores: np.ndarray,
        save_path: Optional[str] = None
    ):
        """
        Plot feature importance scores.
        
        Args:
            feature_names: List of feature names
            importance_scores: Array of importance scores
            save_path: Path to save the plot
        """
        if save_path is None:
            save_path = os.path.join(self.config.plots_dir, "feature_importance.png")
        
        # Sort features by importance
        sorted_indices = np.argsort(importance_scores)[::-1]
        sorted_features = [feature_names[i] for i in sorted_indices]
        sorted_scores = importance_scores[sorted_indices]
        
        plt.figure(figsize=(10, max(6, len(feature_names) * 0.3)))
        plt.barh(range(len(sorted_features)), sorted_scores)
        plt.yticks(range(len(sorted_features)), sorted_features)
        plt.xlabel('Importance Score')
        plt.title(f'{self.config.model_name}: Feature Importance')
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(save_path, bbox_inches='tight')
        print(f"Feature importance plot saved to: {save_path}")
        plt.close()
