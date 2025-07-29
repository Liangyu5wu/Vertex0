"""Main training script for vertex time prediction models."""

import os
import sys
import argparse

# Add src to path
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))

from config.transformer_config import TransformerConfig
from src.data.data_loader import DataLoader
from src.data.data_processor import DataProcessor
from src.models.transformer_model import TransformerModel
from src.training.trainer import Trainer


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Train vertex time prediction model')
    
    parser.add_argument('--config', type=str, default='transformer',
                       help='Configuration to use (default: transformer)')
    parser.add_argument('--data-dir', type=str, default=None,
                       help='Directory containing HDF5 data files')
    parser.add_argument('--model-name', type=str, default=None,
                       help='Name for the model (used for saving)')
    parser.add_argument('--epochs', type=int, default=None,
                       help='Number of training epochs')
    parser.add_argument('--batch-size', type=int, default=None,
                       help='Batch size for training')
    parser.add_argument('--learning-rate', type=float, default=None,
                       help='Learning rate for optimizer')
    parser.add_argument('--max-cells', type=int, default=None,
                       help='Maximum number of cells per event')
    parser.add_argument('--min-cells', type=int, default=None,
                       help='Minimum number of cells per event')
    parser.add_argument('--use-spatial', action='store_true',
                       help='Use spatial features')
    parser.add_argument('--verbose', type=int, default=1,
                       help='Verbosity level (0, 1, 2)')
    
    return parser.parse_args()


def create_config(args):
    """Create configuration based on arguments."""
    if args.config == 'transformer':
        config = TransformerConfig()
    else:
        raise ValueError(f"Unknown config type: {args.config}")
    
    # Override config parameters from command line arguments
    if args.data_dir is not None:
        config.data_dir = args.data_dir
    if args.model_name is not None:
        config.model_name = args.model_name
    if args.epochs is not None:
        config.epochs = args.epochs
    if args.batch_size is not None:
        config.batch_size = args.batch_size
    if args.learning_rate is not None:
        config.learning_rate = args.learning_rate
    if args.max_cells is not None:
        config.max_cells = args.max_cells
    if args.min_cells is not None:
        config.min_cells = args.min_cells
    if args.use_spatial:
        config.use_spatial_features = True
    
    return config


def main():
    """Main training function."""
    args = parse_args()
    
    # Create configuration
    config = create_config(args)
    
    print("="*60)
    print("VERTEX TIME PREDICTION MODEL TRAINING")
    print("="*60)
    print(f"Model: {config.model_name}")
    print(f"Data directory: {config.data_dir}")
    print(f"Max cells: {config.max_cells}")
    print(f"Min cells: {config.min_cells}")
    print(f"Use spatial features: {config.use_spatial_features}")
    print(f"Batch size: {config.batch_size}")
    print(f"Epochs: {config.epochs}")
    if hasattr(config, 'learning_rate'):
        print(f"Learning rate: {config.learning_rate}")
    print("="*60)
    
    # Load data
    print("\n1. Loading and processing data...")
    data_loader = DataLoader(config)
    
    try:
        cell_sequences, vertex_features, vertex_times, sequence_lengths = \
            data_loader.load_data_from_files()
    except Exception as e:
        print(f"Error loading data: {e}")
        print("Please check that the data directory exists and contains HDF5 files.")
        return 1
    
    print(f"Loaded {len(vertex_times)} events")
    print(f"Cell feature dimension: {len(config.cell_features)}")
    print(f"Vertex feature dimension: {vertex_features.shape[1]}")
    
    # Process data
    print("\n2. Splitting and processing data...")
    data_processor = DataProcessor(config)
    
    # Split data
    (train_cells, val_cells, test_cells), \
    (train_vertex, val_vertex, test_vertex), \
    (train_times, val_times, test_times) = data_processor.split_data(
        cell_sequences, vertex_features, vertex_times
    )
    
    # Normalize features
    (train_cells_norm, val_cells_norm, test_cells_norm), \
    (train_vertex_norm, val_vertex_norm, test_vertex_norm), \
    norm_params = data_processor.normalize_features(
        train_cells, val_cells, test_cells,
        train_vertex, val_vertex, test_vertex
    )
    
    # Create datasets
    print("\n3. Creating TensorFlow datasets...")
    train_dataset = data_processor.create_padded_dataset(
        train_cells_norm, train_vertex_norm, train_times, shuffle=True
    )
    val_dataset = data_processor.create_padded_dataset(
        val_cells_norm, val_vertex_norm, val_times, shuffle=False
    )
    
    # Build model
    print("\n4. Building model...")
    model = TransformerModel(config)
    keras_model = model.build_model(len(config.cell_features), train_vertex_norm.shape[1])
    
    # Train model
    print("\n5. Training model...")
    trainer = Trainer(config, model)
    
    # Validate datasets before training
    trainer.validate_training_data(train_dataset, val_dataset)
    
    try:
        history = trainer.train(train_dataset, val_dataset, verbose=args.verbose)
        
        # Save training history
        trainer.save_training_history()
        
        # Print training summary
        summary = trainer.get_training_summary()
        print("\nTraining Summary:")
        for key, value in summary.items():
            print(f"  {key}: {value}")
        
        print(f"\nTraining completed successfully!")
        print(f"Model saved to: {config.model_path}")
        print(f"Training history saved to: {config.model_dir}")
        
        return 0
        
    except Exception as e:
        print(f"Error during training: {e}")
        return 1


if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
