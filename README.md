# **Real Time Timer**

## A demo implementation of an algorithm for a timer resistant to shifting.


Developed by *Dimitrios Karageorgiou*,\
during the course *Embedded and Realtime Systems*,\
*Aristotle University Of Thessaloniki, Greece,*\
*2017-2018.*


This repository contains a demo implementation of a timer that is resistant to shifting. It uses a simple algorithm and tools available on most POSIX compliant systems, like Linux.

Original target of this algorithm was ZSun Card Reader running OpenWRT, though it can be used on any embedded device or more generally on any application that requires a real time timer.   

For testing purposes, the demo extracts timestamps (as provided by *gettimeofday()*). A *MATLAB* script is provided under *analysis_tools* folder for obtaining some useful statistics and plots from extracted timestamps. Make sure to edit it and change the name of file that contains the extracted timestamps, before running it.

## Licensing:
All code in this repo available under *GNU GPL v3* license.
