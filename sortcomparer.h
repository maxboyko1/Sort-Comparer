/**
 * A program for comparing the time performance of various sorting algorithms
 * on a given list of input integer datasets. For example, an input file to
 * the program may look like the following list of 3 datasets, each containing 
 * a list of integers to sort.
 * 
 * 8 29 10 99 1002 76741 888 3412 3 465
 * 99 627 92929 11 738 93387 128302 224 929 99322 29394 11 333 77272 910 1124
 * 1 2 3 4 5 6 7 9 8 10
 * 
 * The program can be run in one of three different ways:

 * ./sortcomparer results < exampleinput.txt
 * ./sortcomparer summary < exampleinput.txt
 * ./sortcomparer < exampleinput.txt
 * 
 * Supplying the argument "results" will print the results of testing each 
 * sorting algorithm on each individual dataset in the input file. Supplying 
 * the argument "summary" will print a summary of the results of running 
 * each sorting algorithm over all the datasets in the file, specifying an 
 * average and total time performance for each algorithm. Supplying no
 * arguments will print both, with the individual results following the 
 * summary.
 */

#ifndef SORTCOMPARER_H
#define SORTCOMPARER_H

/* include statements */
#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/* using declarations */
using Clock = std::chrono::high_resolution_clock;
using DatasetList = std::vector< std::vector<int> >;
using ResultTimesMap = std::unordered_map<std::string, std::vector<long long> >;
using SortAlgoMap = std::unordered_map<std::string, void (*) (std::vector<int>)>;
using TotalTimeMap = std::unordered_map<std::string,long long>;
using TimePair = std::pair<std::string,long long>;

/* function declarations */
void insertion_sort (std::vector<int> v);
void selection_sort (std::vector<int> v);
void bubble_sort (std::vector<int> v);
void heap_sort (std::vector<int> v);
void merge_sort (std::vector<int> v);
void quick_sort (std::vector<int> v);
void shell_sort (std::vector<int> v);

void read_in_datasets (DatasetList& datasets);
bool compare_times (const TimePair& pair1, const TimePair& pair2);
void max_heapify (std::vector<int>& v, int n, int i);
void merge_sort_sublist (std::vector<int>& v, int left, int right);
void merge_sublists (std::vector<int>& v, int start, int middle, int end);
void quick_sort_sublist (std::vector<int>& v, int start, int end);
int partition_sublist (std::vector<int>& v, int start, int end);

#endif // SORTCOMPARER_H
