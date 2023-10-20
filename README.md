# kazhdan-lustzig-combinatorics

## Description and Basic Terminology

**Kazhdan-Lustzig** polynomials are objects that are created to further analyze [weyl groups](https://en.wikipedia.org/wiki/Weyl_group) of algebraic objects that are of interest. These are groups created by reflections, and the finite ones correspond to [symmetric groups](https://en.wikipedia.org/wiki/Symmetry_group). **Kazhdan-Lustzig** polynomials are mainly used in [Representation Theory](https://en.wikipedia.org/wiki/Representation_theory) to obtain building blocks, or one may also call it _irreducible representations_ of those groups. Obtaining these results in a reasonable time requires a computer program.

The program in this repository uses a combinatorial approach to find these polynomials. Given their nature, definitions in this area are quite recursive. These operations are quite challenging for symmetric groups bigger than order **8!** . Please check the minimum requirements and dependencies if you are interested.

## Dependencies and system requirements

 - A [terminal emulator](https://en.wikipedia.org/wiki/Terminal_emulator) of your choice 
 - A [C/C++ compiler](https://en.wikibooks.org/wiki/C%2B%2B_Programming/Compiler/Where_to_get)
 - [C++ Boost graph library](https://www.boost.org/doc/libs/release/libs/graph/doc/)
 - Preferrably `8GB` or more available ram
 - `2GB` minimum free space, up to `100GB` for groups of higher order (> 8!)

Compile directly from source code, and then run the executable output. Preferrably, you need to have the `make` utility installed on your system. In `MacOS or Linux` you may compile everything with the following, there is no need for root privileges:
```
$ make all
```

To get rid of compiled binary files:
```
$ make clean
```

## Using this program as a library

The repository includes a very simple file called `main-driver.cpp` to interact with the functions defined in `polynomials.cpp`, `bruhat-matrix.cpp`, `bruhat-order.cpp` and `permutation-basics.cpp`. These functions can be used independtly if the reader wishes to do so. Every functions is explained inside the sources files with comments to the best of my ability. The interested reader in encouraged to check out the paper in the following section, which dives deeper into the topic and explains the overall structure of the program.

Also, functions used a variety of global variables throughout the execution of the program for optimization purposes. To learn more about them, please check the header files (files with a **.h** extention).

## For more info
  
  - [An Approach to Calculating Kazhdan-Lustzig Polynomials](https://mathwizard.xyz/academic/An-Approach-to-KL-polynomials.pdf) (written by me)


