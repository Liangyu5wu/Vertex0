# Vertex Time Prediction Models

A modular framework for training transformer-based models for vertex t0 prediction with LAr Calorimeter in the ATLAS experiment.

## Project Structure

project_root/
├── config/                     # Configuration files
│   ├── base_config.py         # Base configuration class
│   └── transformer_config.py  # Transformer-specific configuration
├── src/                       # Source code
│   ├── data/                  # Data loading and processing
│   ├── models/                # Model architectures
│   ├── training/              # Training utilities
│   └── evaluation/            # Evaluation and visualization
├── scripts/                   # Main execution scripts
│   ├── train.py              # Training script
│   └── evaluate.py           # Evaluation script
├── models/                   # Saved models and results
│   └── [model_name]/         # Individual model directories
│       ├── model.keras       # Saved model
│       ├── config.json       # Model configuration
│       └── evaluation_plots/ # Evaluation visualizations
└── README.md

## Quick Start

# Installation
pip install tensorflow numpy h5py scikit-learn matplotlib seaborn

# Basic training
python scripts/train.py

# Custom parameters
python scripts/train.py \
    --model-name my_transformer \
    --data-dir /path/to/data \
    --epochs 100 \
    --batch-size 128 \
    --max-cells 50 \
    --use-spatial

# Evaluation
python scripts/evaluate.py --model-dir models/my_transformer --load-data

## Configuration

Key Transformer Parameters in transformer_config.py:
d_model = 128              # Model dimension
num_heads = 8              # Attention heads
max_cells = 40             # Maximum cells per event
learning_rate = 1e-4       # Learning rate
use_spatial_features = False  # Include spatial coordinates

## Data Format

HDF5 structure:
HSvertex/    # Vertex coordinates and target time
cells/       # Variable-length cell sequences with energy, position, timing

## Features

- Modular Design: Easy to extend and modify
- Transformer Architecture: Multi-head attention for sequence modeling
- Comprehensive Evaluation: Multiple metrics and visualizations
- Automatic Saving: Models, configs, and plots organized by experiment
- Resume Training: Continue from saved checkpoints

## Outputs

All results organized in models/[model_name]/ directory containing saved model, configuration, training history, evaluation plots, and prediction results.
