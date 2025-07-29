"""Base configuration class for all models."""

import os
from dataclasses import dataclass
from typing import List, Optional
import json


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
    
    @classmethod
    def load_config(cls, model_dir: str):
        """Load configuration from JSON file."""
        config_path = os.path.join(model_dir, "config.json")
        
        with open(config_path, 'r') as f:
            config_dict = json.load(f)
        
        return cls(**config_dict)
