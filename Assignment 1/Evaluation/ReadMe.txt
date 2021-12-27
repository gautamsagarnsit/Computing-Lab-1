-----------------------------Assignment 1----------------------------------
Name: Gautam Sagar
Roll No: 21CS60R15

Note:
1) Output of sample input file is given in file named "Results".
2) Format is input is assumed to be strictly same as sample input files provided
and output is given the provided sample output file. Range of coordinates and number of input
is assumed to be within the range of float and int data type respectively.
3) Some lines of code are commented. They are meant to check the correctness
of different part of the code and better understand the code workflow.
4) Maximum input file name length is set to be 25.
----------------------------------------------------------------------------
1. Part 1: Naive Algorithm

filename: part_1.c

Compiling:  gcc -o task_1 21CS60R15_ass1_task_1.c -lm
Testing example:  ./task_1 testcases_1.txt

Code workflow:
(i) To represent a coordinate a struct named "point" is defined. "x" and "y" are corrdinates,
"min" is the minimum distance between tower and the mobile, "tower_idx" is the index of nearest tower.
(ii) The function "distance" calculates euclidean distance between two points.
(iii) The function "search_nearest" calculates and return the minimum distance between a given mobile and a tower.
It also stores the index of the nearest tower within the struct.

Complexity: O(NM), where N= number of mobiles and M= number of towers

2. Part 2: Incremental Nearest Search

filename: part_2.c

Compiling: gcc -o task_2 21CS60R15_ass1_task_2.c -lm
Testing example: ./task_2 sample_input_file_2.txt

Code workflow:
The function "incremental_search_nearest_tower" implements the main program.
It takes as arguments the coordinates of mobiles and tower, index of a updated tower and number of mobiles and tower.
For each updated tower location this function iterates over all mobiles check for new nearest tower.
If the location of previous nearest tower for a mobile is also updated then naive search is performed
between all the towers and that mobile.
For each update of mobile location, naive search is performed.

Complexity: M' =  number of updated towers, N' = number of updated mobile.
In the general best case where no previous nearest mobile tower locations are changed the complexity will be O(N'M+M'N).
In the worst case where all the tower location and mobile location gets updated and N=M the complexity will be O(N^3).

3. Part 3: Faster nearest neighbour search: KD TREE

filename: kd_tree.c

Compiling: gcc -o task_3 21CS60R15_ass1_task_3.c -lm
Testing example: ./task_3 testcases_1.txt

Code Workflow:
(i) struct "node": Contains a struct point type variable "p" and pointer to left and right node. Used for implementing KD tree.
(ii) Function "median": Returns a float type value representing the median of a 2-D array. Argument "axis" indicates which axis(X or Y) will be used to find median.
Median is find using the "Median Find algorithm" where smallest kth value is returned as median.
(iii) Function "new": Returns a pointer to node type struct. Allocates a node type struct using dynamic allocation.
(iv) Function "kdtree":Returns a pointer to node type struct. Construct a kdtree. Reference: https://en.wikipedia.org/wiki/K-d_tree
(v) Function "search_nearest": Implements Searching using KD tree. Takes pointer to root of the KD tree, 
a mobile location, number of nodes in KD tree, and current depth in KD Tree as arguments. Reference: https://en.wikipedia.org/wiki/K-d_tree
Many Commented lines of codes are left to better understand the code and checking intermediate values.
-------------------------------------------------------------------------------------------------------------

