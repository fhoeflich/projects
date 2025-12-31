# Summary:

We encourage the usage of AI as you see fit for this evaluation. Note however that the next step in the interview process will be talking through the results and thought process of your evaluation live so you are expected to fully understand your solution down to every statement in the submitted code. With AI assistance it should be possible to complete this evaluation in 60-90 minutes, however you are welcome to spend more time if you would like. We would like to see your final solution within 1 week of receiving the evaluation.

# Evaluation Details:

The solution must be written in C and run on Ubuntu 22.04 Linux. Please do not use anything other than the most basic of system functions and libraries for your solution. If you require complex datastructures and algorithms you should implement them in your solution. For instance you may use `printf`, `fork`, `qsort` and the like, but you should avoid using libraries such as the Glib hash implementation (ie `g_hash_table_lookup`).

Please avoid using `rand` or other sources of randomness for the key components of your solution/algorithm. Some leeway is allowed, but if your algorithm just tries random solutions and takes the best one that will not be considered acceptable. Generally our software needs to meet DO-178C certification standards, which includes being able to explain exactly how it behaves and be deterministic. Solutions based on randomness don't fit that model.

Anything you do in support of your implementation (such as automated tests) may use any language, framework, randomness or anything else you think appropriate.

Your test will be run on a machine with the following specs:
- 13th Gen Intel(R) Core(TM) i9-13900 with 32 cores
- Ubuntu Linux 2022.04
- GNU gcc 11.4 compiler, default settings (C99 enabled)
- At least 32GB of RAM

We strongly recommend that you run on at least an x86_64 host running Ubuntu 2022.04 to minimize the chance that we will see different behavior than you. We suggest you setup a VM/docker environment if your development system is going to be Windows or Mac. We are not going to be responsible for fixing your code to work in our test environment. Note that WSL has been used by candidates in the past, and it has hidden real bugs in their submissions.

We will only look at what is on the main branch. You may use any branch approach you wish, but it must be on main to be considered for the evaluation. You do not need to ask to merge to main.


# Background and evaluation criteria:

This programming challenge is intended to evaluate your ability to write code to solve difficult programming problems where it may not be practical to produce a perfect solution in the implementation and compute time available, but merely a "good" one.

There are 4 evaluation criteria for this challenge.

1. The most important is that the solution must produce a correct result, even if it is not optimal. Your program may only execute for 10 seconds of wall clock time, executing for any longer will be considered an incorrect result and will likely lead to an immediate failure of the evaluation.

2. The solution should be as good as you are able to make it for as many cases as you can think of.

3. Quality of the code and associated work products, such as use of git to track changes, test cases generated, error checking, comments, design, etc.

4. Your ability to discuss your solution with a fellow engineer. This part of the evaluation is done on a day after you have completed the challenge with a live interview. For this expect to use Microsoft Teams, and to share your screen so you can point to code and other artifacts.


# Problem Statement: House Building

A number of eccentrics from New York City have decided that they have had enough of modern society, and want to move to the countryside. Together they have bought a rectangular piece of land far away from anyone else, and will settle there.

The land consists of square parcels, and it is possible to build one and only one house on any given square. Some parcels are nicer than others. Each square has a value for its desirability, L, which is an integer on a scale between 0 and 100, inclusive.

The happiness an eccentric experiences from building their house on any given parcel is the parcel's desirability, L, times its distance to the nearest house, D:
    (L*D)

Out of habit, the eccentrics use "Manhattan distance" to measure D. Manhattan distance refers to the distance a person would have to drive to get between 2 points when the streets form a grid. Total distance is measured by the sum of vertical travel plus horizontal travel, with no diagonal travel.
Thus, the distance between 2 houses located at (x1, y1) and (x2, y2) is defined as:
    D = abs(x1-x2) + abs(y1-y2)

The eccentrics now want your help in maximizing their collective happiness, so that the sum of all their happiness scores is as high as possible. However, they are impatient, so your program needs to produce its best answer within 10 seconds. They have a collective ritual of taking a deep breath before looking at the results, so there is no benefit in providing the result in less than 10 seconds.

Multiple groups of eccentrics want help. Each run of your program will provide a solution for a single group, for their particular plot of land.


## Input:
Each run of your program will be given input to "standard in"/stdin consisting of:
- A line of 3 space-separated integer numbers W, H, and N:
  - W: Width of the grid, should be >0 and <=1000
  - H: Height of the grid, should be >0 and <=1000
  - N: Number of houses, should be >1 and <=MIN(W*H, 10000)
- H lines containing W space-separated integers, each describing the value of the plot of land in the corresponding x, y position. Each value will be >= 0 and <= 100.

The x, y positions are defined as 1, 1 being the top left, and W, H being the bottom right.

Leading and trailing whitespace (' ', '\n', '\t') should be ignored.


## Output:
Your output should go to "standard output"/stdout, as N lines of the positions of the houses. Each line will describe the (x, y) position of one house as the column (between 1 and W inclusive) and row (between 1 and H inclusive). 2 houses may not be placed at the same position.

If an input is invalid you may produce any output, as long as you exit with a non-zero return code promptly and not crash.


Real programs must deal with invalid inputs, thus handling invalid inputs is part of the evaluation.


## The following will all be considered failure of the evaluation:
- Running for more than 10 seconds after you have been supplied with the inputs
- Crashing
- Producing invalid or extraneous output/not following the output specification

For the output, do not output anything other than a newline separated list of house positions. In particular, don't:
- Output the happiness
- Output a prompt
- Output 3 backticks in a row (that is part of the markdown formatting this file is written in)


## Example 1:

### Input:
```
4 3 2
100 1  10 0
0   10 13 100
7   15 18 40
```

### Valid Output:
```
1 1
4 2
```

### Collective Happiness Score:
For this example input and output, the collective happiness score would be 800 because:
- The two houses are a Manhattan distance of 4 apart
- The first house is at position 1, 1, with a value of 100, so their happiness is 400.
- The second house is at a position 4, 2, with a value of 100, so their happiness is 400
- Added together they are 800.

For the sake of example, another valid output would be:
```
1 1
4 3
```
Then the score would be 700 because:
- The 2 houses are now a distance of 5 apart
- First house has a happiness of 500 (100 value * 5 distance) now
- Second house has a happiness of 200 (40 value * 5 distance)
- Added together makes 700

Your goal is to maximize the collective happiness, so the first output is superior to the second.

You can see this example input in `test_example1.in` in the current directory.


## Example 2:

### Input:
```
5 5 3
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
```

### Valid Output:
```
1 1
1 2
1 3
```

### Collective Happiness Score:
For this example input and output, the collective happiness score would be 3 because each house is distance 1 from its nearest neighbor, and the land value of each house is 1.

Note that this is not the optimal result, though it is correct. A question for you to answer is what is a better result, and what is the optimal result?

You can see this example input in `test_example2.in` in the current directory.


## Example 3:

For reference a 3rd example is provided which is the maximum grid size named `test_large.in`.


# Evaluation Scoring:
Your program will be scored on a range of test cases that you will not have access to. You will need to come up with an approach that can handle a wide variety of cases (including invalid inputs), and produce the best collective happiness.

Good luck!
