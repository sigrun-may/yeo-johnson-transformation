# yeo-johnson-transformation

__ABOUT__

This project is about implementing an existing functionality from the python sklearn library in C Code.
The sklearn library provides the sklearn.preprocessing.PowerTransformer class to find a lambda which is used for the Yeo Johnson transformation.<br>
For more information to sklearn, see the [sklearn-powerTransformer] (https://scikit-learn.org/stable/modules/generated/sklearn.preprocessing.PowerTransformer.html)<br>
For more information to the Yeo-Johnson-Transformation, see the [yjpower.pdf] (https://www.stat.umn.edu/arc/yjpower.pdf)

__OBJECTIVE__

The aim is to implement said functionality in C to reach higher performance and enable its use for bigger data.

__REFERENCE__

This project is initiated by "Dipl. Inf. S. May" from "Ostfalia Hochschule Wolfenbüttel".<br>
Proof of concept by S. Biemer.<br>
In cooperation with "Universität Bielefeld".

__COMPATIBILITY__

To make the c function available to python a .so file is used (.dll for windows). Said file is to be compiled by a compiler for the system the library is supposed to be used on. Example: On a x64 machine with python_x64 a compiler for x64 is needed to be able to run the library on this system. A x86 compiler will run on x64 machine but the library won't be accesable by python_x64. Please consider only using compiler and python version same to your machine.
