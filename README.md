# CircuitLive : Audio plugin for Real-Time circuit emulations.
---
This repository is just the implementation of the Modified Nodal Analysis for **transient analysis** of electronic circuit, into an audio plugin. This is a work in progress and this deposit serves as a test environnement.
The aim is to create a functional program capable of loading a netlist in the form of a `.txt` file, where all the components of the circuit are referenced by their node connections, their values (Ohms, henrys, Farads...), and to simulate the behavior of the circuit for any input signal, and any location where we want to know the output voltage or current.

## Plugin's GUI
![CircuitLive plugin image](https://github.com/eliot-des/CircuitLive/assets/72158380/26101e9f-3620-48ee-8cf5-0f18be21b0e9)

## Plugin's workflow

The plugin is structured around 4 different zones. 

-<ins>{**Netlist Editor**}: text editing area for entering circuit information, commonly referred to as a netlist, on which each line represents a component. The first token defines the component type (e.g., voltage source, resistor), the following tokens specify the nodes connected to the component, and the last tokens indicate component values (e.g., resistance, capacitance, etc.).

-<ins>**The Netlist Controller**: area with sliders that can be associated with parameterizable components (poten-
tiometers, variable resistors), referenced in the netlist. For example, if you want to associate a potentiometer `P1` of maximum value 200 kilo Ohms, for which the end terminals are connected to nodes `0` and `4` and the wiper terminal is connected to node `2`, that you want to associate to the slider `3` of the netlist controller, you will have to write down in the netlist editor the following line : `P1 0 4 2 200e3 3`.

-**Simulation Parameters** zone: area allowing to control the number of Newton-Raphson iterations (this parameter is only used if your circuit contains non-linear components such as diodes or transistors), as well as the oversampling factor. The higher the number of Newton iterations, the more accurate the emulation. The higher the number of Newton steps, the more precise the emulation. The oversampling factor can be useful to avoid frequency aliasing for nonlinear circuits, and to emulate frequency response more faithfully for linear circuits that modify the transfer function close to Fs/2. In any case, oversampling implies a big drop in performance, the higher the factor and the bigger the circuit.

-**Audio** zone: controls input and output gain, as well as dry/wet percentage. Input gain is useful if your circuit is nonlinear, in which case it acts as a "drive" knob: the more you increase the gain value, the more likely you are to hear distortion/non-linear phenomena that give character to the sound. 

## Documentation
---
The **M**odified **N**odal Analysis **A**lgorithm (MNA) is quite well documented over different books/papers as the following ones :

- Laurence W. Nagel - SPICE2: A computer program to simulate semiconductor circuits 
- Lawrence T. Pillage, Ronald A. Rohrer, Chandramouli Visweswariah -  Electronic Ciruit And System Simulation Methods
- Stefan Jahn, Michael Margraf, Vincent Habchi, Raimund Jacob - QUCS Technical Papers

Anyway, if you want a **if you want a concise text on how the plugin works**, you can have a look at the [pdf](https://github.com/eliot-des/CircuitLive/blob/master/Real_Time_Circuit_Emulation_Eliot_Deschang.pdf) on my repository.

## Under the hood of the main algorithm (from QUCS technical papers)
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

or **LU** decomposition for example. Though this may be difficult by hand, it is straightforward and so is easily done by computer.

## Prerequisites
---
If you want to test this implementation, you will need to have the [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) library to perform matrix operations.
