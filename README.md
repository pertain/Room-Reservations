# Reservations

res.c is a multithreaded room reservation simulator
for the TCNJ library. It was written as a project for my
Operating Systems course (Spring 2014). The user is prompted
to enter an integer that represents a number of concurrent
reservation requests. The program then asks for user-specific
info, such as user ID, email, and requested room, for each of
the "requests." Once all info is gathered, a separate thread
is dispatched for each "request." Each thread is responsible
for reserving one room. Once all threads have finished, they
are joined with the main thread and the program exits.

The program is very easy to follow. Simply follow the prompts
to simulate a room reservation system. One caveat is that the
program handles bad user input by exiting the program. As a
result, it is very important that input is entered correctly.

```
To compile:     gcc -pthread -o res res.c
To run:         ./res
```
