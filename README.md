# PySurfaceNets

Warning: Work in process.

Python port of SurfaceNets, a sharp-boundary preserving isosurface extraction method for multilabel segmentations.

## Original implementation

This piece of code is a port of SurfaceNets originally implemented by Sarah F. Frisken provided along with the paper "SurfaceNets for Multi-Label Segmentations with Preservation of Sharp Boundaries"(PMID: 36325473).
Original code can be obtained here[https://jcgt.org/published/0011/01/03/].

## How-tos

### Build

```
python setup.py bdist_wheel
pip install dist/PySurfaceNets-0.0.1-cp38-cp38-linux_x86_64.whl # use generated wheel name
```

### Usage

```python
import PySurfaceNets

labels, verts, faces = PySurfaceNets.SurfaceNets(label_array,
                                                 [0.5, 0.5, 1.0]   # spacing
                                                )
```
