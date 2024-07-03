# Modified Nodal Analysis Algorithm
---
This repository is just the implementation of the Modified Nodal Analysis for **transient analysis** of electronic circuit, into an audio plugin. This is a work in progress and this deposit serves as a test environnement.
The aim is to create a functional program capable of loading a netlist in the form of a `.txt` file, where all the components of the circuit are referenced by their node connections, their values (Ohms, henrys, Farads...), and to simulate the behavior of the circuit for any input signal, and any location where we want to know the output voltage or current.

## Documentation
---
The **M**odified **N**odal Analysis **A**lgorithm (MNA) is quite well documented over different books/papers as the following ones :

- Laurence W. Nagel - SPICE2: A computer program to simulate semiconductor circuits 
- Lawrence T. Pillage, Ronald A. Rohrer, Chandramouli Visweswariah -  Electronic Ciruit And System Simulation Methods
- Stefan Jahn, Michael Margraf, Vincent Habchi, Raimund Jacob - QUCS Technical Papers

## Under the hood of the algorithm (from QUCS technical papers)
---
Many different kinds of network element are encountered in network analysis. For circuit analysis
it is necessary to formulate equations for circuits containing as many different types of network
elements as possible. There are various methods for equation formulation for a circuit. These
are based on three types of equations found in circuit theory:

* equations based on Kirchhoff’s voltage law (KVL)
* equations based on Kirchhoff’s current law (KCL)
* branch constitutive equations

The equations have to be formulated (represented in a computer program) automatically in a
simple, comprehensive manner. Once formulated, the system of equations has to be solved. There
are two main aspects to be considered when choosing algorithms for this purpose: accuracy and
speed. The MNA, has been proved to accomplish these tasks.
MNA applied to a circuit with passive elements, independent current and voltage sources and
active elements results in a matrix equation (linear system) of the form:

$$\mathbf{A}\cdot\mathbf{x} = \mathbf{b},$$

where $\mathbf{A}$ is a square Matrix, and where $\mathbf{x}$ and $\mathbf{b}$ are 1D column vectors (the way that both matrix and vectors are filled is really well documented in both 3 books that are referenced just above).
The circuit can therefore be solved by a simple matrix manipulation:

$$\mathbf{x} = \mathbf{A}^{-1}\mathbf{b}$$

or **LU** decomposition for example. Though this may be difficult by hand, it is straightforward and so is easily done by computer

## Prerequisites
---
If you want to test this implementation, you will need to have the [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) library to perform matrix operations.
