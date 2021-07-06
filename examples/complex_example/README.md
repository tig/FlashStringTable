This sample illustrates more complex usage of `FlashStringTable`. 

# Build
`pio ci examples/complex_example --board=uno --lib=src  --project-option="lib_deps=ArduinoLog, arduino-fsm"`

# Upload & Monitor
`pio run --target upload --project-conf=.\examples\complex_example\platformio.ini --project-dir=.\examples\complex_example\ -t monitor`

# Monitor
`pio device monitor --project-dir=.\examples\complex_example`

