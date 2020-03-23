Author: Robert Tucker Swan, Juan Gaona
Login: rswan, jgaona
Lab3: Mailboxes and Process Scheduling
Due: 03/24/2020

How to Build Solution:
- In short, in a similar manner as with the given example

- Compile the OS using the following commands within the lab3/ folder
-- $ cd os/
-- $ make
- Compile the program using the following commands within the lab3/ folder
-- $ cd apps/
-- $ cd <desired question (eg: q2)>
-- $ make
- Change Arguments
-- $ cd apps/
-- $ cd <desired question (eg: q2)>
-- modify the arguments following "makeprocs.dlx.obj" and before the next ";" within the Makefile
--- argument order for q2 is number of S2 to inject followed by number of CO to inject
- Run the program
-- $ cd apps/
-- $ cd <desired question (eg: q2)>
-- make run

- Clean the program
-- $ cd os/
-- $ make clean
-- $ cd ../apps/
-- $ cd <desired question (eg: q2)>
-- $ make clean

Unusual Things Within Program:
- The final spawn_me prints "done!" after the expected output prints it

External Sources:
- Sanghyun Cho (TA)
