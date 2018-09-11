# Sort-Comparer
A program for testing the time performance of various sorting algorithms on a given list of integer datasets. For example, an input file to the program may look like the following list of 3 datasets, each containing a list of integers to sort.

```
8 29 10 99 1002 76741 888 3412 3 465
99 627 92929 11 738 93387 128302 224 929 99322 29394 11 333 77272 910 1124
1 2 3 4 5 6 7 9 8 10
```

The program can be run in one of three different ways:

```
./sortcomparer results < exampleinput.txt
./sortcomparer summary < exampleinput.txt
./sortcomparer < exampleinput.txt
```

Supplying the argument `results` will separately print the results of testing each sorting algorithm on each individual dataset in the input file. Supplying the argument `summary` will print a summary of the results of running each sorting algorithm over all the datasets in the file, specifying an average and total time performance for each algorithm. Supplying no arguments will print both, with the individual results following the summary.

See the repository for example input and output files.

Tested on Mac OS X 10.13.6.
