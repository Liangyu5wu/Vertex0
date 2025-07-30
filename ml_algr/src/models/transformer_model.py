"""Transformer model implementation for vertex time prediction."""

import tensorflow as tf
from tensorflow.keras import layers, models
import tensorflow.keras.backend as K
from typing import Dict, Any

from .transformer_layers import PositionalEncoding, MultiHeadSelfAttention, TransformerBlock
from config.transformer_config import TransformerConfig


def root_mean_squared_error(y_true, y_pred):
    """Custom RMSE metric."""
    return K.sqrt(K.mean(K.square(y_pred - y_true)))


class TransformerModel:
    """Transformer model for vertex time prediction."""
    
    def __init__(self, config: TransformerConfig):
        """
        Initialize transformer model.
        
        Args:
            config: Transformer configuration object
        """
        self.config = config
        self.model = None
        
    def build_model(self, feature_dim: int, vertex_dim: int) -> tf.keras.Model:
        """
        Build the transformer model architecture.
        
        Args:
            feature_dim: Dimension of cell features
            vertex_dim: Dimension of vertex features
            
        Returns:
            Compiled Keras model
        """
        # Validate configuration
        self.config.validate_config()
        
        # Cell sequence input (variable length, will be padded during batching)
        cell_inputs = layers.Input(shape=(None, feature_dim), name='cell_sequence')
        
        # Project cell features to d_model dimensions
        x = layers.Dense(self.config.d_model)(cell_inputs)
        
        # Add positional encoding
        x = PositionalEncoding(self.config.max_position, self.config.d_model)(x)
        
        # Stack transformer blocks
        for i in range(self.config.num_transformer_blocks):
            x = TransformerBlock(
                self.config.d_model, 
                self.config.num_heads, 
                self.config.dff, 
                rate=self.config.dropout_rate
            )(x)
        
        # Global average pooling (automatically handles variable lengths)
        cell_representation = layers.GlobalAveragePooling1D()(x)
        
        # Vertex features input
        vertex_inputs = layers.Input(shape=(vertex_dim,), name='vertex_features')
        vertex_dense = layers.Dense(
            self.config.vertex_dense_units, 
            activation='relu'
        )(vertex_inputs)
        
        # Combine cell and vertex representations
        combined = layers.Concatenate()([cell_representation, vertex_dense])
        
        # Final prediction layers
        x = combined
        for i, (units, dropout_rate) in enumerate(zip(
            self.config.final_dense_units, 
            self.config.final_dropout_rates
        )):
            x = layers.Dense(units, activation='relu')(x)
            
            if self.config.use_batch_norm:
                x = layers.BatchNormalization()(x)
                
            x = layers.Dropout(dropout_rate)(x)
        
        # Output layer
        output = layers.Dense(1, name='vertex_time')(x)
        
        # Create model
        model = models.Model(inputs=[cell_inputs, vertex_inputs], outputs=output)
        
        # Compile model
        optimizer = tf.keras.optimizers.Adam(learning_rate=self.config.learning_rate)
        model.compile(
            optimizer=optimizer,
            loss='mse',
            metrics=['mae', root_mean_squared_error, tf.keras.metrics.MeanSquaredError(name='mse_metric')]
        )
        
        self.model = model
        return model
    
    def get_model(self) -> tf.keras.Model:
        """Get the built model."""
        if self.model is None:
            raise ValueError("Model has not been built yet. Call build_model() first.")
        return self.model
    
    def save_model(self, filepath: str = None):
        """
        Save the model to file.
        
        Args:
            filepath: Path to save the model. If None, uses config path.
        """
        if self.model is None:
            raise ValueError("Model has not been built yet.")
            
        if filepath is None:
            filepath = self.config.model_path
            
        self.model.save(filepath)
        print(f"Model saved to {filepath}")
    
    @staticmethod
    def load_model(filepath: str) -> tf.keras.Model:
        """
        Load a saved model.
        
        Args:
            filepath: Path to the saved model
            
        Returns:
            Loaded Keras model
        """
        custom_objects = {
            'root_mean_squared_error': root_mean_squared_error,
            'PositionalEncoding': PositionalEncoding,
            'MultiHeadSelfAttention': MultiHeadSelfAttention,
            'TransformerBlock': TransformerBlock
        }
        
        return models.load_model(filepath, custom_objects=custom_objects)
    
    def get_model_summary(self) -> str:
        """Get model summary as string."""
        if self.model is None:
            raise ValueError("Model has not been built yet.")
            
        summary_lines = []
        self.model.summary(print_fn=lambda x: summary_lines.append(x))
        return '\n'.join(summary_lines)
    
    def count_parameters(self) -> Dict[str, int]:
        """
        Count model parameters.
        
        Returns:
            Dictionary with parameter counts
        """
        if self.model is None:
            raise ValueError("Model has not been built yet.")
            
        total_params = self.model.count_params()
        trainable_params = sum([K.count_params(w) for w in self.model.trainable_weights])
        non_trainable_params = total_params - trainable_params
        
        return {
            'total': total_params,
            'trainable': trainable_params,
            'non_trainable': non_trainable_params
        }
