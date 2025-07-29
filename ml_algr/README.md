project_root/
├── config/
│   ├── __init__.py
│   ├── base_config.py
│   └── transformer_config.py
├── src/
│   ├── __init__.py
│   ├── data/
│   │   ├── __init__.py
│   │   ├── data_loader.py
│   │   └── data_processor.py
│   ├── models/
│   │   ├── __init__.py
│   │   ├── transformer_layers.py
│   │   └── transformer_model.py
│   ├── training/
│   │   ├── __init__.py
│   │   └── trainer.py
│   └── evaluation/
│       ├── __init__.py
│       ├── evaluator.py
│       └── visualizer.py
├── scripts/
│   ├── train.py
│   └── evaluate.py
├── models/
│   └── [model_name]/
│       ├── model.keras
│       ├── config.json
│       └── evaluation_plots/
│           ├── training_metrics.png
│           ├── prediction_results.png
│           ├── histogram_comparison.png
│           └── error_distribution.png
└── README.md
