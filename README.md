<h2 align="center"> Wireworld </h2>
<p align="center"><img src="example.gif"width=35%></p>

Adaption of the [wireworld cellular automaton](https://en.wikipedia.org/wiki/Wireworld), completed in October 2019. Wireworld is formed from a 2D grid of cells, simulating digitical electronics. The four states for the cells are 'empty', 'electron head', 'electron tail', or 'copper'. The next generation of cells follows the rules, where n is the number of electron heads in the 8-surrounding cells: 
* empty − > empty
* electron head − > electron tail
* electron tail − > copper
* copper−> electronhead if n==1 or n==2 
* copper − > copper otherwise



## Usage
```
$ gcc wireworld.c neillncurses.c -Wall -Wfloat-equal -Wextra -O2 -pedantic -ansi -lncurses -lm -o wireworld
```
To run, enter the wireworld text file as the second argument (example .txt files are included in this repository)
```
$ wireworld wirefile.txt
```

### ncurses
This project uses neillncurses.h as a wrapper for ncurses, created by Neill Campbell at the University of Bristol. 

## Disclaimer
This work was submitted for the COMSM1201 Programming in C module at the University of Bristol. Please note that no student can use this work without my permission or attempt to pass this work off as their own. 
