# TOS-on-X86 The Train Operating System
This is the final project of the Advanced Operating System Course at SFSU.
At the end of this course, I was able to
- apply my knowledge of operating system to build my own OS.
- learn about PC architecture
- learn the basics of the Intel x86 CPU
- Learn how to program I/O devices.

## Details of the project:
Compile TOS with test cases using the following command 
cd ~/tos 
make tests 

## Run the TOS Test Center (TTC) 
Make sure .bochsrc includes the line: 
com2: enabled=1, mode=socket, dev=localhost:8899 

## Launch TTC with:  
java -jar ttc.jar 
Start (or re-start) Bochs  

TOS contains the binaries of a reference implementation. You can run 
a test case against the reference implementation to see what the 
output of the test case should look like. 
