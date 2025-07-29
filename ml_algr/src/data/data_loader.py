"""Data loading utilities for HDF5 files."""

import os
import h5py
import numpy as np
from typing import List, Tuple, Optional
from config.base_config import BaseConfig


class DataLoader:
    """Load and preprocess data from HDF5 files."""
    
    def __init__(self, config: BaseConfig):
        """
        Initialize data loader.
        
        Args:
            config: Configuration object containing data parameters
        """
        self.config = config
        
    def get_file_paths(self) -> List[str]:
        """Get list of HDF5 file paths."""
        return [
            os.path.join(self.config.data_dir, f"output_{i:03d}.h5") 
            for i in range(self.config.num_files)
        ]
    
    def load_data_from_files(
        self, 
        file_paths: Optional[List[str]] = None
    ) -> Tuple[List[List[List[float]]], np.ndarray, np.ndarray, np.ndarray]:
        """
        Load data from HDF5 files.
        
        Args:
            file_paths: List of file paths to load. If None, uses default paths.
            
        Returns:
            Tuple of (cell_sequences, vertex_features, vertex_times, sequence_lengths)
        """
        if file_paths is None:
            file_paths = self.get_file_paths()
            
        all_cell_sequences = []
        all_vertex_features = []
        all_vertex_times = []
        sequence_lengths = []
        
        print(f"Using spatial features: {self.config.use_spatial_features}")
        print(f"Cell features used: {self.config.cell_features}")
        print(f"Min cells required: {self.config.min_cells}, Max cells considered: {self.config.max_cells}")
        
        for file_path in file_paths:
            if not os.path.exists(file_path):
                print(f"Warning: File {file_path} not found, skipping...")
                continue
                
            print(f"Processing {file_path}...")
            try:
                cell_seq, vertex_feat, vertex_time, seq_len = self._process_file(file_path)
                all_cell_sequences.extend(cell_seq)
                all_vertex_features.extend(vertex_feat)
                all_vertex_times.extend(vertex_time)
                sequence_lengths.extend(seq_len)
            except Exception as e:
                print(f"Error processing {file_path}: {e}")
                continue
        
        sequence_lengths = np.array(sequence_lengths)
        print(f"Processed {len(all_vertex_times)} valid events")
        self._print_sequence_statistics(sequence_lengths)
        
        return (all_cell_sequences, np.array(all_vertex_features), 
                np.array(all_vertex_times), sequence_lengths)
    
    def _process_file(self, file_path: str) -> Tuple[List, List, List, List]:
        """Process a single HDF5 file."""
        cell_sequences = []
        vertex_features = []
        vertex_times = []
        sequence_lengths = []
        
        with h5py.File(file_path, 'r') as f:
            vertex_data = f['HSvertex'][:]
            cells_data = f['cells'][:]
            
            for i in range(len(vertex_data)):
                # Extract vertex features for global context
                if self.config.use_spatial_features:
                    vertex_reco = [
                        vertex_data[i]['HSvertex_reco_x'],
                        vertex_data[i]['HSvertex_reco_y'],
                        vertex_data[i]['HSvertex_reco_z']
                    ]
                else:
                    vertex_reco = [0.0, 0.0, 0.0]
                
                # Process cells for this event
                event_cells = cells_data[i]
                valid_cells = event_cells[
                    (event_cells['valid'] == True) & 
                    (event_cells['matched_track_HS'] == 1)
                ]
                
                # Skip events with too few cells
                if len(valid_cells) < self.config.min_cells:
                    continue
                
                # Sort and select cells
                sequence = self._process_event_cells(valid_cells)
                if sequence is None:
                    continue
                
                cell_sequences.append(sequence)
                vertex_features.append(vertex_reco)
                vertex_times.append(vertex_data[i]['HSvertex_time'])
                sequence_lengths.append(len(sequence))
        
        return cell_sequences, vertex_features, vertex_times, sequence_lengths
    
    def _process_event_cells(self, valid_cells: np.ndarray) -> Optional[List[List[float]]]:
        """Process cells for a single event."""
        # Sort cells by selection feature
        if self.config.cell_selection_feature in ['Cell_e', 'Cell_significance', 'matched_track_pt']:
            sorted_indices = np.argsort(-valid_cells[self.config.cell_selection_feature])
        else:
            sorted_indices = np.argsort(-valid_cells[self.config.cell_selection_feature])
        
        # Take up to max_cells
        n_cells_to_use = min(len(valid_cells), self.config.max_cells)
        sorted_cells = valid_cells[sorted_indices[:n_cells_to_use]]
        
        # Create sequence of cell features
        sequence = []
        for cell_idx in range(n_cells_to_use):
            cell_features_values = []
            for feature in self.config.cell_features:
                cell_features_values.append(sorted_cells[feature][cell_idx])
            sequence.append(cell_features_values)
        
        return sequence
    
    def _print_sequence_statistics(self, sequence_lengths: np.ndarray):
        """Print statistics about sequence lengths."""
        print(f"Sequence length statistics:")
        print(f"  Mean: {np.mean(sequence_lengths):.2f}")
        print(f"  Std: {np.std(sequence_lengths):.2f}")
        print(f"  Min: {np.min(sequence_lengths)}")
        print(f"  Max: {np.max(sequence_lengths)}")
        print(f"  Median: {np.median(sequence_lengths):.2f}")
