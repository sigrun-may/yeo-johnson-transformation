# Yeo-Johnson-Transformation in C with Python Bindings

## Table of Contents

- [About](#About)
- [Installation](#installation)
- [Licensing](#licensing)

## About

This software provides an implementation of the Yeo-Johnson-Transformation
(see https://www.stat.umn.edu/arc/yjpower.pdf) with Python bindings. This implementation is especially useful for
high-dimensional data.
It speeds up the computation time and reduces the skewness to a greater extent than the standard
sklearn.preprocessing.PowerTransformer library
(see https://scikit-learn.org/stable/modules/generated/sklearn.preprocessing.PowerTransformer.html).
The sklearn library offers a much simpler but slower implementation with a slightly lower optimization quality.

## Installation

To make the C function available to Python, a .so file (Linux) or .dll file (Windows) is required.
This file must be generated by a compiler compatible with the respective target system.
Example: On a x64 machine with python_x64 a compiler for x64 is needed to be able to run the library on this system.
An x86 compiler will run on x64 machine but the library won't be accessible by python_x64.
Please consider only using compiler and Python version suitable for your machine.
<br>
<br>
Compile .so / .dll files:<br>
make dir<br>
make all<br>


## Licensing

Copyright (c) 2023 Jerome Brenig, Swen Biemer, Sigrun May, Ostfalia Hochschule für angewandte Wissenschaften

Licensed under the **MIT License** (the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License by reviewing the file
[LICENSE](https://github.com/sigrun-may/artificial-data-generator/blob/main/LICENSE) in the repository.
