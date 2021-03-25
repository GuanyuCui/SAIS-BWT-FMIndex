# SAIS-BWT-FMIndex
 SAIS-BWT-FMIndex implementation for substring query.

## Structure
+ README.md: This file.

+ codes: Codes used in this project.
	+ sais-lite-2.4.1: Fast & Lite SAIS algorithm implementation by Yuta Mori. (From: https://sites.google.com/site/yuta256/sais)
	+ BWT\_FM.h: Header file. Definition of the BWT_FM class.
	+ BWT\_FM.cpp: Source file. Implementation of the BWT_FM class' member functions.
	+ main.cpp: Main source file. A command line interface.
	
+ report.pdf: Introduction & analysis of the SAIS-BWT-FMIndex algorithm.

## Compile

g++ -std=c++14 -O3 BWT_FM.cpp main.cpp -o main && ./main

## Usage

HELP: Print usage.

FILELOAD [filename]: Load string from file and create index.

STRLOAD [string]: Load string and create index.

BWT: Give the result of BWT(string).

FILEMATCH [filename]: Match substring from file.

STRMATCH [string]: Match substring from string.

QUIT: Quit.