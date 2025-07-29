"""Base configuration class for all models with YAML support."""

import os
from dataclasses import dataclass, fields
from typing import List, Optional, Dict, Any, Union
import json
import yaml


@dataclass
class BaseConfig:
    """Base configuration class containing common parameters."""
    
    # Data parameters
    data_dir: str = "../selected_h5/"
    num_files: int = 50
    max_cells: int = 40
    min_cells: int = 3
    cell_selection_feature: str = 'Cell_e'
    use_spatial_features: bool = False
    
    # Data split parameters
    test_size: float = 0.3
    val_split: float = 1/3  # Fraction of test_size for validation
    random_state: int = 42
    
    # Training parameters
    batch_size: int = 64
    epochs: int = 50
    early_stopping_patience: int = 15
    lr_patience: int = 5
    min_lr: float = 1e-7
    
    # Model save parameters
    models_base_dir: str = "models"
    model_name: str = "base_model"
    
    # Feature definitions
    spatial_features: List[str] = None
    vertex_spatial_features: List[str] = None
    all_cell_features: List[str] = None
    skip_normalization: List[str] = None
    
    def __post_init__(self):
        """Initialize feature lists after dataclass initialization."""
        if self.spatial_features is None:
            self.spatial_features = ['Cell_x', 'Cell_y', 'Cell_z']
            
        if self.vertex_spatial_features is None:
            self.vertex_spatial_features = ["HSvertex_reco_x", "HSvertex_reco_y", "HSvertex_reco_z"]
            
        if self.all_cell_features is None:
            self.all_cell_features = [
                'Cell_x', 'Cell_y', 'Cell_z', 'Cell_eta', 'Cell_phi', 'Cell_Barrel', 'Cell_layer',
                'Cell_time_TOF_corrected', 'Cell_e', 'Cell_significance', 
                'matched_track_pt', 'matched_track_deltaR'
            ]
            
        if self.skip_normalization is None:
            self.skip_normalization = ['Cell_time_TOF_corrected', 'Cell_Barrel', 'Cell_layer']
    
    @property
    def cell_features(self) -> List[str]:
        """Get cell features based on spatial feature usage."""
        return [f for f in self.all_cell_features 
                if self.use_spatial_features or f not in self.spatial_features]
    
    @property
    def model_dir(self) -> str:
        """Get model directory path."""
        return os.path.join(self.models_base_dir, self.model_name)
    
    @property
    def model_path(self) -> str:
        """Get model file path."""
        return os.path.join(self.model_dir, "model.keras")
    
    @property
    def plots_dir(self) -> str:
        """Get evaluation plots directory."""
        return os.path.join(self.model_dir, "evaluation_plots")
    
    def create_directories(self):
        """Create necessary directories."""
        os.makedirs(self.model_dir, exist_ok=True)
        os.makedirs(self.plots_dir, exist_ok=True)
    
    def save_config(self):
        """Save configuration to JSON file."""
        self.create_directories()
        config_path = os.path.join(self.model_dir, "config.json")
        
        # Convert to dictionary, handling non-serializable types
        config_dict = {}
        for key, value in self.__dict__.items():
            if isinstance(value, (str, int, float, bool, list)):
                config_dict[key] = value
            else:
                config_dict[key] = str(value)
        
        with open(config_path, 'w') as f:
            json.dump(config_dict, f, indent=2)
    
    def save_yaml(self, filepath: str = None):
        """
        Save configuration to YAML file.
        
        Args:
            filepath: Path to save YAML file. If None, saves to model directory.
        """
        if filepath is None:
            self.create_directories()
            filepath = os.path.join(self.model_dir, "config.yaml")
        
        # Convert to dictionary
        config_dict = {}
        for key, value in self.__dict__.items():
            if isinstance(value, (str, int, float, bool, list)):
                config_dict[key] = value
            else:
                config_dict[key] = str(value)
        
        with open(filepath, 'w') as f:
            yaml.dump(config_dict, f, default_flow_style=False, indent=2)
        
        print(f"Configuration saved to: {filepath}")
    
    @classmethod
    def from_yaml(cls, yaml_path: str):
        """
        Load configuration from YAML file.
        
        Args:
            yaml_path: Path to YAML configuration file
            
        Returns:
            Configuration instance
        """
        if not os.path.exists(yaml_path):
            raise FileNotFoundError(f"YAML configuration file not found: {yaml_path}")
        
        with open(yaml_path, 'r') as f:
            yaml_data = yaml.safe_load(f)
        
        # Get valid field names for this class
        valid_fields = {field.name for field in fields(cls)}
        
        # Filter yaml_data to only include valid fields
        filtered_data = {k: v for k, v in yaml_data.items() if k in valid_fields}
        
        # Create instance with filtered data
        instance = cls(**filtered_data)
        
        print(f"Configuration loaded from: {yaml_path}")
        print(f"Model name: {instance.model_name}")
        
        return instance
    
    @classmethod
    def load_config(cls, model_dir: str):
        """Load configuration from JSON file."""
        config_path = os.path.join(model_dir, "config.json")
        
        with open(config_path, 'r') as f:
            config_dict = json.load(f)
        
        return cls(**config_dict)
    
    def update_from_dict(self, updates: Dict[str, Any]):
        """
        Update configuration parameters from dictionary.
        
        Args:
            updates: Dictionary of parameter updates
        """
        valid_fields = {field.name for field in fields(self)}
        
        for key, value in updates.items():
            if key in valid_fields:
                setattr(self, key, value)
            else:
                print(f"Warning: Unknown parameter '{key}' ignored")
    
    def update_from_args(self, args):
        """
        Update configuration from command line arguments.
        
        Args:
            args: Parsed command line arguments
        """
        # Map of argument names to config attribute names
        arg_mapping = {
            'data_dir': 'data_dir',
            'model_name': 'model_name',
            'epochs': 'epochs',
            'batch_size': 'batch_size',
            'learning_rate': 'learning_rate',
            'max_cells': 'max_cells',
            'min_cells': 'min_cells',
            'use_spatial': 'use_spatial_features'
        }
        
        for arg_name, config_attr in arg_mapping.items():
            if hasattr(args, arg_name) and getattr(args, arg_name) is not None:
                setattr(self, config_attr, getattr(args, arg_name))
    
    def print_config(self):
        """Print configuration parameters in a formatted way."""
        print("=" * 60)
        print(f"CONFIGURATION: {self.model_name}")
        print("=" * 60)
        
        # Group parameters by category
        categories = {
            "Data Parameters": [
                'data_dir', 'num_files', 'max_cells', 'min_cells', 
                'cell_selection_feature', 'use_spatial_features'
            ],
            "Training Parameters": [
                'batch_size', 'epochs', 'early_stopping_patience', 
                'lr_patience', 'min_lr'
            ],
            "Model Save Parameters": [
                'models_base_dir', 'model_name'
            ]
        }
        
        for category, params in categories.items():
            print(f"\n{category}:")
            for param in params:
                if hasattr(self, param):
                    value = getattr(self, param)
                    print(f"  {param}: {value}")
        
        # Add architecture parameters if they exist
        arch_params = ['d_model', 'num_heads', 'dff', 'num_transformer_blocks', 'dropout_rate']
        if any(hasattr(self, param) for param in arch_params):
            print(f"\nArchitecture Parameters:")
            for param in arch_params:
                if hasattr(self, param):
                    value = getattr(self, param)
                    print(f"  {param}: {value}")
        
        print("=" * 60)
    
    def validate_config(self):
        """Validate configuration parameters."""
        # Basic validations
        assert self.max_cells > 0, "max_cells must be positive"
        assert self.min_cells > 0, "min_cells must be positive"
        assert self.min_cells <= self.max_cells, "min_cells must be <= max_cells"
        assert 0 < self.test_size < 1, "test_size must be between 0 and 1"
        assert 0 < self.val_split < 1, "val_split must be between 0 and 1"
        assert self.epochs > 0, "epochs must be positive"
        assert self.batch_size > 0, "batch_size must be positive"
        
        print("Configuration validation passed.")
