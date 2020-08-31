# scsdk Python Bindings

## Building

Our python bindings can be built by running the command.

```
python3 setup.py bdist_wheel && python3 setup.py test
```

A wheel is outputted to `dist`, that can be installed by doing

```
python3 -m pip install dist/scsdk-2.2.0-cp38-cp38-macosx_10_15_x86_64.whl 
```

## Samples

Sample script of our python bindings can be found in `scsdk_samples/ `. 
