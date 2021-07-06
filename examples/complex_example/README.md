# Build
`pio ci .\examples\complex_example\ --lib=src  --project-conf=.\examples\complex_example\platformio.ini`

# Upload & Monitor
`pio run --target upload --project-conf=.\examples\complex_example\platformio.ini --project-dir=.\examples\complex_example\ -t monitor`

# Monitor
`pio device monitor --project-dir=.\examples\complex_example`