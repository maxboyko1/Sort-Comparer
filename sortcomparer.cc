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
 * the argument "summary" will print a summary of the results of of running 
 * each sorting algorithm over all the datasets in the file, specifying an 
 * average and total time performance for each algorithm. Supplying no
 * arguments will print both, with the individual results following the 
 * summary.
 */

#include "sortcomparer.h"

int main (int argc, char const *argv[])
{
    if (argc < 1 || argc > 2) {
        std::cerr << "USAGE: ./sortcomparer [results OR summary]" << std::endl;
        return -1;
    }

    if (argc == 2) {
        if (strcmp(argv[1], "results") && strcmp(argv[1], "summary")) {
            std::cerr << "ERROR: Invalid program argument, try 'results' or 'summary'" 
                << std::endl;
            return -2;
        }
    }

    TotalTimeMap total_times;     /* Map of algo names to total runtimes */
    ResultTimesMap result_times;  /* Map of algo names to lists of dataset runtimes */
    SortAlgoMap sort_algos;       /* Map of algo names to implementation functions */
    DatasetList datasets;         /* List of datasets input by the user */

    sort_algos["Insertion Sort"] = insertion_sort;
    sort_algos["Selection Sort"] = selection_sort;
    sort_algos["Bubble Sort"] = bubble_sort;
    sort_algos["Heap Sort"] = heap_sort;
    sort_algos["Merge Sort"] = merge_sort;
    sort_algos["Quick Sort"] = quick_sort;
    sort_algos["Shell Sort"] = shell_sort;
    const int num_algos = sort_algos.size();

    read_in_datasets(datasets);
    const int num_datasets = datasets.size();

    /* Remember which of the summary and/or results the user has requested */
    const bool summary_needed = (argc == 1) || (argc == 2 && !strcmp(argv[1], "summary"));
    const bool results_needed = (argc == 1) || (argc == 2 && !strcmp(argv[1], "results"));

    /* If the user has requested individual dataset results, initialize the
     * list of runtimes for each algorithm with enough capacity for all 
     * datasets. */
    if (results_needed)
        for (auto iter = sort_algos.begin(); iter != sort_algos.end(); ++iter)
            result_times[iter->first].reserve(num_datasets);

    /* Time the execution of each sorting algorithm on each dataset, and store
     * the resulting runtimes individually in that algorithm's list (if the 
     * user has requested individual results) and/or as a part of that algorithm's
     * sum total running time (if the user has requested a summary). */
    for (int i = 0; i < num_datasets; ++i) {
        std::cout << "Running sort algorithms on dataset " << (i + 1) << "..." << std::endl;

        for (auto iter = sort_algos.begin(); iter != sort_algos.end(); ++iter) {
            std::string algo = iter->first;
            long long duration;
                
            Clock::time_point start_time = Clock::now();
            iter->second(datasets[i]);
            Clock::time_point end_time = Clock::now(); 

            duration = std::chrono::duration_cast<std::chrono::microseconds>
                (end_time - start_time).count();

            if (summary_needed) {
                if (total_times.find(algo) == total_times.end())
                    total_times[algo] = duration;
                else
                    total_times[algo] += duration;
            }

            if (results_needed)
                result_times[algo].push_back(duration);
        }
    }

    std::cout << std::endl;

    /* Sort the list of sorting algorithms in increasing order of their total
     * execution time over all datasets, and print the algorithm names in order
     * with their total and average runtimes. */
    if (summary_needed) {
        std::vector<TimePair> fast_to_slow;
        fast_to_slow.reserve(num_algos);
        
        for (auto iter = total_times.begin(); iter != total_times.end(); ++iter)
            fast_to_slow.emplace_back(iter->first, iter->second);
        
        std::sort(fast_to_slow.begin(), fast_to_slow.end(), compare_times);

        std::cout << "==================== SUMMARY ====================" << std::endl;
        std::cout << std::setprecision(3) << std::fixed;

        for (int i = 0; i < num_algos; ++i) {
            std::string algo = fast_to_slow[i].first;
            long long total_time = fast_to_slow[i].second;
            double avg_time = (double) total_time / num_datasets;

            std::cout << (i + 1) << ". " << algo << ": total time " << total_time << 
                " microseconds, or " << avg_time << 
                " microseconds per dataset on average" << std::endl;
        }

        std::cout << std::endl;
    }

    if (results_needed) {
        std::vector<TimePair> fast_to_slow;
        fast_to_slow.reserve(num_algos);

        std::cout << "==================== RESULTS ====================" << std::endl;

        /* For each dataset, sort the list of sorting algorithms in increasing order
         * of execution time on that dataset, and print the algorithm names in order
         * with their runtimes. */
        for (int i = 0; i < num_datasets; ++i) {
            fast_to_slow.clear();            

            for (auto iter = result_times.begin(); iter != result_times.end(); ++iter)
                fast_to_slow.emplace_back(iter->first, iter->second[i]);

            std::sort(fast_to_slow.begin(), fast_to_slow.end(), compare_times);

            std::cout << "DATASET " << (i + 1) << ":" << std::endl;

            for (int j = 0; j < num_algos; ++j) {
                std::string algo = fast_to_slow[j].first;
                long long result_time = fast_to_slow[j].second;
                std::cout << (j + 1) << ". " << algo << ": " << result_time 
                    << " microseconds" << std::endl;
            }

            std::cout << std::endl;
        }
    }

    return 0;
}

/*
 * Read the lists of integers entered by the user via stdin into the specified 
 * list of datasets. If the user enters a token that could not be parsed as an
 * integer, the program will notify the user of the failure but still attempt
 * to parse the rest of the dataset.
 */
void read_in_datasets (DatasetList& datasets) 
{
    bool errors_occured;       /* Whether any parse errors have occurred */
    int elem;                  /* Current element being parsed */
    int linecount;             /* Line counter, for error reporting */
    int position;              /* Position counter, for error reporting */
    std::string data_str;      /* Dataset entered on the current line */
    std::vector<int> dataset;  /* List of elements in current dataset */

    std::getline(std::cin, data_str);
    linecount = 1;
    errors_occured = false;

    while (!std::cin.eof() && !data_str.empty()) {
        dataset.clear();

        std::stringstream ss(data_str);
        position = 1;

        while (ss >> elem || !ss.eof()) {
            if (ss.fail()) {
                errors_occured = true;
                std::cerr << "ERROR: Failed to convert element at line " 
                    << linecount << ", position " << position 
                    << " to an integer" << std::endl;

                ss.clear();
                std::string temp;
                ss >> temp;
            } else {
                dataset.push_back(elem);
            }

            ++position;
        }

        datasets.push_back(dataset);
        
        std::getline(std::cin, data_str);
        ++linecount;
    }

    if (errors_occured)
        std::cerr << std::endl;
}

/*
 * Return whether the sorting algorithm represented by pair1 took less time
 * to execute than the one represented by pair2.
 */
bool compare_times (const TimePair& pair1, const TimePair& pair2)
{
    return pair1.second < pair2.second;
}

/*
 * Sort the given list using the Insertion Sort algorithm.
 */
void insertion_sort (std::vector<int> v)
{
    int i, j;  /* Indices */
    int n;     /* Length of list to sort */

    n = v.size();

    for (i = 1; i < n; ++i) {
        j = i;
        while (j > 0 && v[j - 1] > v[j]) {
            std::swap(v[j], v[j - 1]);
            --j;
        }
    }
}

/*
 * Sort the given list using the Selection Sort algorithm.
 */
void selection_sort (std::vector<int> v)
{
    int i, j;  /* Indices */
    int n;     /* Length of list to sort */
    int min;   /* Index of minimum element in unsorted sublist */

    n = v.size();

    for (i = 0; i < n - 1; ++i) {
        min = i;
        for (j = i + 1; j < n; ++j)
            if (v[j] < v[min])
                min = j;

        if (min != i)
            std::swap(v[i], v[min]);
    }
}

/*
 * Sort the given list using the Bubble Sort algorithm.
 */
void bubble_sort (std::vector<int> v)
{
    int i;         /* Index */
    int n;         /* Length of list to sort */
    bool swapped;  /* Indicates whether a swap has occurred on this pass */

    n = v.size();
    swapped = true;

    while (swapped) {
        swapped = false;

        for (i = 1; i < n; ++i) {
            if (v[i - 1] > v[i]) {
                std::swap(v[i - 1], v[i]);
                swapped = true;
            }       
        }
    }
}

/*
 * Sort the given list using the Heap Sort algorithm.
 */
void heap_sort (std::vector<int> v)
{
    int i;  /* Index */
    int n;  /* Length of list to sort */

    n = v.size();

    for (i = n / 2 - 1; i >= 0; --i)
        max_heapify(v, n, i);

    for (i = n - 1; i >= 0; --i) {
        std::swap(v[0], v[i]);
        max_heapify(v, i, 0);
    }
}

/*
 * Helper function for Heap Sort that constructs a max heap rooted at index i 
 * in the provided list of size n, assuming the binary trees rooted at i's 
 * left and right children are already max heaps.
 */
void max_heapify (std::vector<int>& v, int n, int i)
{
    int largest;  /* Index of largest list value among v[i] and its children */
    int left;     /* Index of v[i]'s left child, if it exists */
    int right;    /* Index of v[i]'s right child, if it exists */

    largest = i;
    left = 2 * i + 1;
    right = 2 * i + 2;

    if (left < n && v[left] > v[i])
        largest = left;
    if (right < n && v[right] > v[largest])
        largest = right;

    /* If v[i] is larger than its two children, then the max heap construction
     * is done. Otherwise, we need to swap v[i] with the larger of its two 
     * children and repeat the process on the subtree rooted there. */
    if (largest != i) {
        std::swap(v[i], v[largest]);
        max_heapify(v, n, largest);
    }
}

/*
 * Sort the given list using the Merge Sort algorithm.
 */
void merge_sort (std::vector<int> v)
{
    merge_sort_sublist(v, 0, v.size() - 1);
}

/*
 * Helper function for Merge Sort that sorts the sublist of the given list 
 * with the specified start and end indices, using the Merge Sort algorithm.
 */
void merge_sort_sublist (std::vector<int>& v, int start, int end)
{
    int middle;  /* Middle index of the sublist */

    if (start < end) {
        middle = start + ((end - start) / 2);
        merge_sort_sublist(v, start, middle);
        merge_sort_sublist(v, middle + 1, end);
        merge_sublists(v, start, middle, end);
    }
}

/*
 * Helper function for Merge Sort that sorts v[start..end] by merging the two 
 * sorted sublists v[start..middle] and v[middle + 1..end] into a single 
 * sorted list.
 */
void merge_sublists (std::vector<int>& v, int start, int middle, int end)
{
    int i;   /* Index into first initial sublist */
    int j;   /* Index into second initial sublist */
    int k;   /* Index into final merged sublist */
    int n1;  /* Length of first initial sublist */
    int n2;  /* Length of second initial sublist */

    n1 = middle - start + 1;
    n2 = end - middle;

    int left_sublist[n1];
    int right_sublist[n2];

    /* Initialize the two auxiliary sublists with the proper elements from 
     * the initial state of v[start..end]. */
    for (i = 0; i < n1; ++i)
        left_sublist[i] = v[start + i];
    for (j = 0; j < n2; ++j)
        right_sublist[j] = v[middle + 1 + j];

    i = 0;
    j = 0;
    k = start;

    /* Begin merging by repeatedly assigning the lesser of the two elements at
     * the front of either sublist to the proper position in v. */
    while (i < n1 && j < n2) {
        if (left_sublist[i] <= right_sublist[j])
            v[k++] = left_sublist[i++];
        else
            v[k++] = right_sublist[j++];
    }

    /* If elements remain in the left sublist that have not yet been added to
     * v, add them now. */
    while (i < n1)
        v[k++] = left_sublist[i++];

    /* Otherwise, add the rest of the elements in the right sublist to v. */
    while (j < n2)
        v[k++] = right_sublist[j++];
}

/*
 * Sort the given list using the Quick Sort algorithm.
 */
void quick_sort (std::vector<int> v)
{
    quick_sort_sublist(v, 0, v.size() - 1);
}

/*
 * Helper function for Quick Sort that sorts the sublist of the given list 
 * with the specified start and end indices, using the Quick Sort algorithm.
 */
void quick_sort_sublist (std::vector<int>& v, int start, int end)
{
    int middle;  /* Middle index of the sublist */

    if (start < end) {
        middle = partition_sublist(v, start, end);
        quick_sort_sublist(v, start, middle - 1);
        quick_sort_sublist(v, middle + 1, end);
    }
}

/*
 * Helper function for Quick Sort that partitions v[start..end] into two
 * sublists, the first with all elements less than or equal to a chosen pivot
 * element (in this case, the initial element at v[end]), and the rest with
 * all elements greater than this pivot element. Return the index of the end 
 * of the end of the first sublist.
 */
int partition_sublist (std::vector<int>& v, int start, int end)
{
    int i;  /* Last index of sublist of elements found to be the pivot */
    int j;  /* First index of sublist of elements yet to be examined */

    i = start - 1;
    
    for (j = start; j < end; ++j)
        if (v[j] <= v[end])
            std::swap(v[++i], v[j]);

    std::swap(v[++i], v[end]);
    return i;
}

/*
 * Sort the given list using the Shell Sort algorithm, with Donald Shell's
 * original proposed gap size sequence (n / 2, n / 4, n / 8, etc).
 */
void shell_sort (std::vector<int> v)
{
    int i, j;  /* Indices */
    int gap;   /* Distance between elements being compared */
    int n;     /* Length of list to sort */

    n = v.size();

    for (gap = n / 2; gap > 0; gap /= 2)
        for (i = gap; i < n; ++i)
            for (j = i; j >= gap && v[j - gap] > v[j]; j -= gap)
                std::swap(v[j], v[j - gap]);
}
