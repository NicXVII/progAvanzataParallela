# Binary Search Tree Project

This project implements a binary search tree (BST) in C. It includes functionalities for creating a BST, inserting nodes, calculating the depth of the tree, and deleting the tree. The project is structured into multiple source files and header files for better organization and modularity.

## Project Structure

```
c-bst-project
├── src
│   ├── main.c          # Entry point of the application
│   ├── bst.c           # Implementation of binary search tree functions
│   └── t_node.c        # Implementation of binary tree node functions
├── include
│   ├── bst.h           # Header file for binary search tree
│   └── t_node.h        # Header file for binary tree nodes
├── Makefile             # Build script for compiling the project
└── README.md            # Documentation for the project
```

## Building the Project

To build the project, navigate to the project directory and run the following command:

```
make
```

This will compile the source files and create the executable.

## Running the Application

After building the project, you can run the application using:

```
./c-bst-project
```

You will be prompted to enter the number of nodes to insert into the binary search tree.

## Cleaning Up

To remove the compiled files and clean the project directory, use the following command:

```
make clean
```

## Dependencies

This project is written in C and does not have any external dependencies.

## Author

This project was developed as a demonstration of binary search tree implementation in C.