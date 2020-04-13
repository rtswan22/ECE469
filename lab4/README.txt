Author: Robert Tucker Swan, Juan Gaona
Login: rswan, jgaona
Lab4: Memory Management
Due: 03/14/2020

How to Build Solution:
- In short, in a similar manner as with all previous labs

- First, select what build folder is being tested (one-level, fork, heap_management) and open it
- Compile the OS using the following commands within the build folder
-- $ cd os/
-- $ make
- Compile the program using the following commands within the build folder
-- $ cd apps/
--       ONE-LEVEL: $ cd example/
--            FORK: $ cd test_q4/
-- HEAP-MANAGEMENT: $ cd test_q5/
-- $ make
- Change Arguments (Applies to one-level/ ONLY)
-- $ cd apps/
-- ONE-LEVEL: $ cd example/
-- modify the arguments following "makeprocs.dlx.obj" and before the next ";" within the Makefile
--- This argument decides how many of the tests to run. Note that the test order is 1, 3, 4, 5, 6, then 2. So running with an argument of 3 would run tests 1, 3, and 4.
- Run the program
-- $ cd apps/
--       ONE-LEVEL: $ cd example/
--            FORK: $ cd test_q4/
-- HEAP-MANAGEMENT: $ cd test_q5/
-- make run

- Clean the program
-- $ cd os/
-- $ make clean
-- $ cd ../apps/
--       ONE-LEVEL: $ cd example/
--            FORK: $ cd test_q4/
-- HEAP-MANAGEMENT: $ cd test_q5/
-- $ make clean

Unusual Things Within Program:
- Q2: Makeprocs will print a "DONE" statement after each test and each test will print a "Done!" statement. If the test "Done!" statement does not print, it means the process was killed. The test order is 1, 3, 4, 5, 6, then 2 because test 2 closes the program because of accessing too high of a vaddr, as it should.
- Q4: The PTE also prints items related to the system stack to show how it changes on forks.
- Q5: The test print statements are spaced for readability. Additionally, the heap position is found independently of the print statements because of the way the loops were implemented.

External Sources:
- Sanghyun Cho (TA)
