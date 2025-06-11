import os
import numpy as np
import h5py
import tensorflow as tf
from tensorflow.keras import layers, models, callbacks
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt

data_dir = "selected_h5/"
model_save_path = "vertex_time_predictor_model.keras" 

geometry_features = [
    'Cell_x', 'Cell_y', 'Cell_z', 'Cell_eta', 'Cell_phi', 'Cell_Barrel', 'Cell_layer'
]

physics_features = [
    'Cell_time_TOF_corrected', 'Cell_e', 'Cell_significance', 
    'matched_track_pt', 'matched_track_deltaR'
]

skip_normalization = ['Cell_eta', 'Cell_phi', 'Cell_Barrel', 'Cell_layer']

def load_data_from_files(file_paths):
    all_vertex_features = []
    all_cell_geometry = []
    all_cell_physics = []
    all_vertex_times = []
    
    for file_path in file_paths:
        print(f"Processing {file_path}...")
        with h5py.File(file_path, 'r') as f:
            vertex_data = f['HSvertex'][:]
            cells_data = f['cells'][:]
            
            for i in range(len(vertex_data)):
 
                vertex_reco = np.array([
                    vertex_data[i]['HSvertex_reco_x'],
                    vertex_data[i]['HSvertex_reco_y'],
                    vertex_data[i]['HSvertex_reco_z']
                ])
                
                event_cells = cells_data[i]
                
                valid_cells = event_cells[
                    (event_cells['valid'] == True) & 
                    (event_cells['matched_track_HS'] == 1)
                ]
                
                if len(valid_cells) < 30:
                    continue
                
                sorted_indices = np.argsort(-valid_cells['Cell_e'])[:30]
                sorted_cells = valid_cells[sorted_indices]
                
                geom_feature_array = np.zeros((30, len(geometry_features)))
                phys_feature_array = np.zeros((30, len(physics_features)))
                
                for j, feature in enumerate(geometry_features):
                    geom_feature_array[:, j] = sorted_cells[feature]
                
                for j, feature in enumerate(physics_features):
                    phys_feature_array[:, j] = sorted_cells[feature]
                
                all_vertex_features.append(vertex_reco)
                all_cell_geometry.append(geom_feature_array)
                all_cell_physics.append(phys_feature_array)
                all_vertex_times.append(vertex_data[i]['HSvertex_time'])
    
    return (
        np.array(all_vertex_features), 
        np.array(all_cell_geometry),
        np.array(all_cell_physics),
        np.array(all_vertex_times)
    )

file_paths = [os.path.join(data_dir, f"output_{i:03d}.h5") for i in range(50)]

print("Loading and processing data...")
vertex_features, cell_geometry, cell_physics, vertex_times = load_data_from_files(file_paths)
print(f"Processed {len(vertex_times)} valid events")

def normalize_features(train_data, val_data, test_data):
    train_vertex, train_geom, train_phys, train_y = train_data
    val_vertex, val_geom, val_phys, val_y = val_data
    test_vertex, test_geom, test_phys, test_y = test_data
    
    vertex_mean = np.mean(train_vertex, axis=0)
    vertex_std = np.std(train_vertex, axis=0)
    vertex_std = np.where(vertex_std == 0, 1, vertex_std)
    
    geom_means = np.zeros(train_geom.shape[2])
    geom_stds = np.ones(train_geom.shape[2])
    phys_means = np.zeros(train_phys.shape[2])
    phys_stds = np.ones(train_phys.shape[2])
    
    for j, feature in enumerate(geometry_features):
        if feature not in skip_normalization:
            values = train_geom[:, :, j].flatten()
            geom_means[j] = np.mean(values)
            std_val = np.std(values)
            geom_stds[j] = std_val if std_val > 0 else 1
    
    for j in range(train_phys.shape[2]):
        values = train_phys[:, :, j].flatten()
        phys_means[j] = np.mean(values)
        std_val = np.std(values)
        phys_stds[j] = std_val if std_val > 0 else 1
    
    y_mean = np.mean(train_y)
    y_std = np.std(train_y)
    if y_std == 0:
        y_std = 1
    
    train_vertex_norm = (train_vertex - vertex_mean) / vertex_std
    val_vertex_norm = (val_vertex - vertex_mean) / vertex_std
    test_vertex_norm = (test_vertex - vertex_mean) / vertex_std
    
    train_geom_norm = train_geom.copy()
    val_geom_norm = val_geom.copy()
    test_geom_norm = test_geom.copy()
    
    for j, feature in enumerate(geometry_features):
        if feature not in skip_normalization:
            train_geom_norm[:, :, j] = (train_geom[:, :, j] - geom_means[j]) / geom_stds[j]
            val_geom_norm[:, :, j] = (val_geom[:, :, j] - geom_means[j]) / geom_stds[j]
            test_geom_norm[:, :, j] = (test_geom[:, :, j] - geom_means[j]) / geom_stds[j]
    
    train_phys_norm = (train_phys - phys_means) / phys_stds
    val_phys_norm = (val_phys - phys_means) / phys_stds
    test_phys_norm = (test_phys - phys_means) / phys_stds
    
    train_y_norm = (train_y - y_mean) / y_std
    val_y_norm = (val_y - y_mean) / y_std
    test_y_norm = (test_y - y_mean) / y_std
    
    norm_params = {
        'vertex_mean': vertex_mean,
        'vertex_std': vertex_std,
        'geom_means': geom_means,
        'geom_stds': geom_stds,
        'phys_means': phys_means,
        'phys_stds': phys_stds,
        'y_mean': y_mean,
        'y_std': y_std,
        'skip_normalization': skip_normalization,
        'geometry_features': geometry_features
    }
    
    return (
        (train_vertex_norm, train_geom_norm, train_phys_norm, train_y_norm),
        (val_vertex_norm, val_geom_norm, val_phys_norm, val_y_norm),
        (test_vertex_norm, test_geom_norm, test_phys_norm, test_y_norm),
        norm_params
    )

# Split data into train, validation and test sets (70%, 20%, 10%)
indices = np.arange(len(vertex_times))
train_indices, temp_indices = train_test_split(indices, test_size=0.3, random_state=42)
val_indices, test_indices = train_test_split(temp_indices, test_size=1/3, random_state=42)

train_data = (
    vertex_features[train_indices], 
    cell_geometry[train_indices], 
    cell_physics[train_indices], 
    vertex_times[train_indices]
)
val_data = (
    vertex_features[val_indices], 
    cell_geometry[val_indices], 
    cell_physics[val_indices], 
    vertex_times[val_indices]
)
test_data = (
    vertex_features[test_indices], 
    cell_geometry[test_indices], 
    cell_physics[test_indices], 
    vertex_times[test_indices]
)

train_data_norm, val_data_norm, test_data_norm, norm_params = normalize_features(
    train_data, val_data, test_data
)


def build_model(vertex_shape, geom_shape, phys_shape):
    # Input layers
    vertex_input = layers.Input(shape=vertex_shape, name="vertex_input")
    geometry_input = layers.Input(shape=geom_shape, name="geometry_input")
    physics_input = layers.Input(shape=phys_shape, name="physics_input")
    

    cell_features = layers.Concatenate(axis=-1)([geometry_input, physics_input])
    
    cell_processed = layers.TimeDistributed(layers.Dense(64, activation='relu'))(cell_features)
    cell_processed = layers.BatchNormalization()(cell_processed)
    cell_processed = layers.TimeDistributed(layers.Dense(32, activation='relu'))(cell_processed)
    cell_processed = layers.BatchNormalization()(cell_processed)
    
    cell_aggregated = layers.GlobalAveragePooling1D()(cell_processed)

    combined = layers.Concatenate()([vertex_input, cell_aggregated])
    

    x = layers.Dense(256, activation='relu')(combined)
    x = layers.BatchNormalization()(x)
    
    x = layers.Dense(128, activation='relu')(x)
    x = layers.BatchNormalization()(x)
    x = layers.Dropout(0.3)(x)
    
    x = layers.Dense(64, activation='relu')(x)
    x = layers.BatchNormalization()(x)
    x = layers.Dropout(0.2)(x)
    
    x = layers.Dense(32, activation='relu')(x)
    x = layers.BatchNormalization()(x)
    
    output = layers.Dense(1)(x)
    
    model = models.Model(
        inputs=[vertex_input, geometry_input, physics_input], 
        outputs=output
    )
    
    optimizer = tf.keras.optimizers.Adam(learning_rate=5e-4)
    
    model.compile(
        optimizer=optimizer,
        loss='mse',
        metrics=['mae']
    )
    
    return model

vertex_shape = train_data_norm[0].shape[1:]
geom_shape = train_data_norm[1].shape[1:]
phys_shape = train_data_norm[2].shape[1:]

print(f"Vertex feature shape: {vertex_shape}")
print(f"Geometry feature shape: {geom_shape}")
print(f"Physics feature shape: {phys_shape}")

print(f"Training set size: {len(train_data_norm[0])} events")
print(f"Validation set size: {len(val_data_norm[0])} events")
print(f"Test set size: {len(test_data_norm[0])} events")
print(f"Total events: {len(train_data_norm[0]) + len(val_data_norm[0]) + len(test_data_norm[0])}")

model = build_model(vertex_shape, geom_shape, phys_shape)
model.summary()

callbacks_list = [
    callbacks.EarlyStopping(
        monitor='val_loss',
        patience=40,
        restore_best_weights=True
    ),
    callbacks.ModelCheckpoint(
        filepath=model_save_path,
        monitor='val_loss',
        save_best_only=True
    ),
    callbacks.ReduceLROnPlateau(
        monitor='val_loss',
        factor=0.4,
        patience=10,
        min_lr=1e-8
    )
]

batch_size = 64
epochs = 200

# Train the model
history = model.fit(
    [train_data_norm[0], train_data_norm[1], train_data_norm[2]], 
    train_data_norm[3],
    epochs=epochs,
    batch_size=batch_size,
    validation_data=(
        [val_data_norm[0], val_data_norm[1], val_data_norm[2]], 
        val_data_norm[3]
    ),
    callbacks=callbacks_list,
    verbose=1
)

# Load the best model
best_model = models.load_model(model_save_path)

test_loss_norm, test_mae_norm = best_model.evaluate(
    [test_data_norm[0], test_data_norm[1], test_data_norm[2]], 
    test_data_norm[3],
    verbose=1
)

print(f"Test MSE (normalized): {test_loss_norm:.4f}")
print(f"Test MAE (normalized): {test_mae_norm:.4f}")

y_mean = norm_params['y_mean']
y_std = norm_params['y_std']

y_pred_norm = best_model.predict(
    [test_data_norm[0], test_data_norm[1], test_data_norm[2]]
)
y_pred_norm = y_pred_norm.flatten()

y_pred = y_pred_norm * y_std + y_mean
y_true = test_data_norm[3] * y_std + y_mean

test_mse = np.mean((y_pred - y_true) ** 2)
test_mae = np.mean(np.abs(y_pred - y_true))

print(f"Test MSE (original scale): {test_mse:.4f}")
print(f"Test MAE (original scale): {test_mae:.4f}")
