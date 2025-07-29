"""Data preprocessing and normalization utilities."""

import numpy as np
import tensorflow as tf
from typing import List, Tuple, Dict, Any
from sklearn.model_selection import train_test_split
from config.base_config import BaseConfig


class DataProcessor:
    """Handle data preprocessing, normalization, and dataset creation."""
    
    def __init__(self, config: BaseConfig):
        """
        Initialize data processor.
        
        Args:
            config: Configuration object containing processing parameters
        """
        self.config = config
        
    def split_data(
        self, 
        cell_sequences: List[List[List[float]]], 
        vertex_features: np.ndarray, 
        vertex_times: np.ndarray
    ) -> Tuple[Tuple[List, List, List], Tuple[np.ndarray, np.ndarray, np.ndarray], 
               Tuple[np.ndarray, np.ndarray, np.ndarray]]:
        """
        Split data into train, validation, and test sets.
        
        Args:
            cell_sequences: List of cell sequences
            vertex_features: Array of vertex features
            vertex_times: Array of vertex times
            
        Returns:
            Tuple of ((train_cells, val_cells, test_cells), 
                     (train_vertex, val_vertex, test_vertex),
                     (train_times, val_times, test_times))
        """
        indices = np.arange(len(vertex_times))
        train_indices, temp_indices = train_test_split(
            indices, test_size=self.config.test_size, random_state=self.config.random_state
        )
        val_indices, test_indices = train_test_split(
            temp_indices, test_size=self.config.val_split, random_state=self.config.random_state
        )
        
        train_cells = [cell_sequences[i] for i in train_indices]
        val_cells = [cell_sequences[i] for i in val_indices]
        test_cells = [cell_sequences[i] for i in test_indices]
        
        train_vertex = vertex_features[train_indices]
        val_vertex = vertex_features[val_indices]
        test_vertex = vertex_features[test_indices]
        
        train_times = vertex_times[train_indices]
        val_times = vertex_times[val_indices]
        test_times = vertex_times[test_indices]
        
        print(f"Data split sizes:")
        print(f"Training set: {len(train_cells)} events")
        print(f"Validation set: {len(val_cells)} events")
        print(f"Test set: {len(test_cells)} events")
        
        return ((train_cells, val_cells, test_cells),
                (train_vertex, val_vertex, test_vertex),
                (train_times, val_times, test_times))
    
    def normalize_features(
        self,
        train_cells: List[List[List[float]]],
        val_cells: List[List[List[float]]],
        test_cells: List[List[List[float]]],
        train_vertex: np.ndarray,
        val_vertex: np.ndarray,
        test_vertex: np.ndarray
    ) -> Tuple[Tuple[List, List, List], Tuple[np.ndarray, np.ndarray, np.ndarray], Dict[str, Any]]:
        """
        Normalize cell and vertex features.
        
        Args:
            train_cells, val_cells, test_cells: Cell sequences for each split
            train_vertex, val_vertex, test_vertex: Vertex features for each split
            
        Returns:
            Tuple of normalized data and normalization parameters
        """
        # Normalize cell features
        train_cells_norm, val_cells_norm, test_cells_norm, cell_norm_params = \
            self._normalize_cell_features(train_cells, val_cells, test_cells)
        
        # Normalize vertex features
        train_vertex_norm, val_vertex_norm, test_vertex_norm, vertex_norm_params = \
            self._normalize_vertex_features(train_vertex, val_vertex, test_vertex)
        
        norm_params = {
            'cell_means': cell_norm_params['means'],
            'cell_stds': cell_norm_params['stds'],
            'vertex_means': vertex_norm_params['means'],
            'vertex_stds': vertex_norm_params['stds']
        }
        
        return ((train_cells_norm, val_cells_norm, test_cells_norm),
                (train_vertex_norm, val_vertex_norm, test_vertex_norm),
                norm_params)
    
    def _normalize_cell_features(
        self,
        train_cells: List[List[List[float]]],
        val_cells: List[List[List[float]]],
        test_cells: List[List[List[float]]]
    ) -> Tuple[List, List, List, Dict[str, List]]:
        """Normalize cell features based on training data statistics."""
        # Collect all training feature values
        all_train_values = [[] for _ in range(len(self.config.cell_features))]
        
        for sequence in train_cells:
            for cell in sequence:
                for feat_idx, value in enumerate(cell):
                    all_train_values[feat_idx].append(value)
        
        # Compute normalization parameters
        cell_means = []
        cell_stds = []
        skip_indices = self._get_skip_indices()
        
        for feat_idx in range(len(self.config.cell_features)):
            if feat_idx in skip_indices:
                cell_means.append(0.0)
                cell_stds.append(1.0)
            else:
                values = np.array(all_train_values[feat_idx])
                mean_val = np.mean(values)
                std_val = np.std(values)
                cell_means.append(mean_val)
                cell_stds.append(std_val if std_val > 0 else 1.0)
        
        # Apply normalization
        train_cells_norm = self._apply_cell_normalization(train_cells, cell_means, cell_stds)
        val_cells_norm = self._apply_cell_normalization(val_cells, cell_means, cell_stds)
        test_cells_norm = self._apply_cell_normalization(test_cells, cell_means, cell_stds)
        
        return (train_cells_norm, val_cells_norm, test_cells_norm,
                {'means': cell_means, 'stds': cell_stds})
    
    def _normalize_vertex_features(
        self,
        train_vertex: np.ndarray,
        val_vertex: np.ndarray,
        test_vertex: np.ndarray
    ) -> Tuple[np.ndarray, np.ndarray, np.ndarray, Dict[str, np.ndarray]]:
        """Normalize vertex features based on training data statistics."""
        vertex_means = np.mean(train_vertex, axis=0)
        vertex_stds = np.std(train_vertex, axis=0)
        vertex_stds = np.where(vertex_stds > 0, vertex_stds, 1)
        
        train_vertex_norm = (train_vertex - vertex_means) / vertex_stds
        val_vertex_norm = (val_vertex - vertex_means) / vertex_stds
        test_vertex_norm = (test_vertex - vertex_means) / vertex_stds
        
        return (train_vertex_norm, val_vertex_norm, test_vertex_norm,
                {'means': vertex_means, 'stds': vertex_stds})
    
    def _get_skip_indices(self) -> List[int]:
        """Get indices of features to skip normalization for."""
        skip_indices = []
        for feature in self.config.skip_normalization:
            if feature in self.config.cell_features:
                skip_indices.append(self.config.cell_features.index(feature))
        return skip_indices
    
    def _apply_cell_normalization(
        self,
        cell_sequences: List[List[List[float]]],
        means: List[float],
        stds: List[float]
    ) -> List[List[List[float]]]:
        """Apply normalization to cell sequences."""
        normalized_sequences = []
        for sequence in cell_sequences:
            normalized_sequence = []
            for cell in sequence:
                normalized_cell = []
                for feat_idx, value in enumerate(cell):
                    if stds[feat_idx] != 1.0:
                        normalized_value = (value - means[feat_idx]) / stds[feat_idx]
                    else:
                        normalized_value = value
                    normalized_cell.append(normalized_value)
                normalized_sequence.append(normalized_cell)
            normalized_sequences.append(normalized_sequence)
        return normalized_sequences
    
    def create_padded_dataset(
        self,
        cell_sequences: List[List[List[float]]],
        vertex_features: np.ndarray,
        vertex_times: np.ndarray,
        shuffle: bool = True
    ) -> tf.data.Dataset:
        """
        Create padded TensorFlow dataset from sequences.
        
        Args:
            cell_sequences: Variable-length cell sequences
            vertex_features: Vertex feature arrays
            vertex_times: Target vertex times
            shuffle: Whether to shuffle the dataset
            
        Returns:
            Batched and prefetched TensorFlow dataset
        """
        # Find maximum sequence length
        max_seq_len = max(len(seq) for seq in cell_sequences)
        
        # Pad all sequences to max length
        padded_cells = np.zeros((len(cell_sequences), max_seq_len, len(self.config.cell_features)))
        for i, seq in enumerate(cell_sequences):
            seq_len = len(seq)
            padded_cells[i, :seq_len, :] = seq
        
        # Create TensorFlow dataset
        dataset = tf.data.Dataset.from_tensor_slices((
            {'cell_sequence': padded_cells, 'vertex_features': vertex_features},
            vertex_times
        ))
        
        if shuffle:
            dataset = dataset.shuffle(buffer_size=10000)
        
        return dataset.batch(self.config.batch_size).prefetch(tf.data.AUTOTUNE)
    
    def create_prediction_batches(
        self,
        cell_sequences: List[List[List[float]]],
        vertex_features: np.ndarray,
        vertex_times: np.ndarray
    ):
        """
        Create batches for prediction from variable-length sequences.
        
        Args:
            cell_sequences: Variable-length cell sequences
            vertex_features: Vertex feature arrays
            vertex_times: Target vertex times
            
        Yields:
            Batches of data for prediction
        """
        # Sort by sequence length for more efficient batching
        lengths = [len(seq) for seq in cell_sequences]
        indices = np.argsort(lengths)
        
        for i in range(0, len(indices), self.config.batch_size):
            batch_indices = indices[i:i+self.config.batch_size]
            
            # Find max length in this batch
            batch_lengths = [lengths[idx] for idx in batch_indices]
            max_length = max(batch_lengths)
            
            # Pad sequences in this batch to max_length
            batch_cells = np.zeros((len(batch_indices), max_length, len(self.config.cell_features)))
            batch_vertex = np.zeros((len(batch_indices), len(vertex_features[0])))
            batch_times = np.zeros(len(batch_indices))
            
            for j, idx in enumerate(batch_indices):
                seq = cell_sequences[idx]
                seq_len = len(seq)
                
                # Fill in the actual sequence
                for k in range(seq_len):
                    batch_cells[j, k, :] = seq[k]
                # Padding is automatically 0 from np.zeros
                
                batch_vertex[j] = vertex_features[idx]
                batch_times[j] = vertex_times[idx]
            
            yield {'cell_sequence': batch_cells, 'vertex_features': batch_vertex}, batch_times
