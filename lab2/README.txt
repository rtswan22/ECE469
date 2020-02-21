Author: Robert Tucker Swan, Juan Gaona
Login: rswan, jgaona
Lab1: Semaphores, Locks, and Conditional Variables
Due: 02/18/2020

How to Build Solution:
- In short, in a similar manner as with the given example

- Compile the OS using the following commands within the lab2/ folder
-- $ cd os/
-- $ make
- Compile the program using the following commands within the lab2/ folder
-- $ cd apps/
-- $ cd <desired question (eg: q2, q4, q5)>
-- $ make
- Change Arguments
-- $ cd apps/
-- $ cd <desired question (eg: q2, q4, q5)>
-- modify the arguments following "makeprocs.dlx.obj" and before the next ";"
--- q2 and q4 have 1 additional argument to the program call and q5 has 2 additional arguments.
- Run the program
-- $ cd apps/
-- $ cd <desired question (eg: q2, q4, q5)>
-- make run

- Clean the program
-- $ cd os/
-- $ make clean
-- $ cd ../apps/
-- $ cd <desired question (eg: q2, q4, q5)>
-- $ make clean

Unusual Things Within Program:
- There are additional print statements at the beginning and the end for q5 in addition to the required statements
- The semaphores and counts are all in shared memory for q5. They are in a struct that is defined in spawn.h

External Sources:
- Austin Ketterer (during lab hours)
- Class Lecture Notes on Conditional Variables
