# Advanced C++ Quiz Scoring and Analysis System

## Project Overview
This C++ program demonstrates advanced memory management and data processing techniques to analyze quiz results for multiple contestants. It showcases the use of smart pointers, dynamic memory allocation, and efficient data structures to process quiz data, calculate statistics, and generate comprehensive reports.

## Features
- Efficient processing of answer key and contestant responses using smart pointers
- Dynamic memory allocation for flexible handling of varying numbers of questions and contestants
- Calculation of individual contestant scores with detailed error reporting
- Generation of statistical measures including mean, median, and mode
- Identification of most frequently missed questions (60% or higher miss rate)
- Demonstration of modern C++ practices including RAII and exception safety

## Implementation Details
The program leverages several advanced C++ features and concepts:

1. **Smart Pointers**: 
   - Utilizes `std::unique_ptr` for automatic memory management
   - Ensures exception safety and prevents memory leaks

2. **Dynamic Memory Allocation**:
   - Dynamically allocates arrays for answer keys, contestant responses, and scores
   - Allows for flexible handling of quizzes with varying numbers of questions and contestants

3. **Struct and Array Management**:
   - Uses a `Contestant` struct to encapsulate contestant data
   - Manages arrays of `Contestant` objects and their responses efficiently

4. **File Processing**: 
   - `processAnswerKey`: Reads correct answers into a dynamically allocated array
   - `processContestantFile`: Populates a dynamic array of `Contestant` structs

5. **Score Calculation**: 
   - `calculateContestantScores`: Computes scores using pointer arithmetic for efficient array traversal

6. **Statistical Analysis**:
   - Implements algorithms for mean, median, and mode calculations
   - Uses `std::sort` with raw pointers for efficient sorting of scores

7. **Most Missed Questions Analysis**:
   - `findMostMissedQuestions`: Uses dynamic arrays to track question miss rates

8. **Reporting**:
   - Generates detailed reports using efficient string handling and formatting

## Pointer Usage and Memory Management
- **Smart Pointers**: `std::unique_ptr` is used extensively to manage dynamically allocated arrays, ensuring automatic cleanup and exception safety.
- **Raw Pointer Arithmetic**: Used within functions for efficient array traversal and manipulation.
- **Dynamic Arrays**: All major data structures (answer key, contestant responses, scores) are dynamically allocated to accommodate quizzes of any size.
- **RAII Principle**: Resource management adheres to the Resource Acquisition Is Initialization principle, leveraging C++'s stack unwinding for guaranteed cleanup.

## Input Files
The program requires two input files:

1. **Answer Key File** (e.g., `answerFile.txt`):
   - Contains the correct answers for each question
   - One answer per line

   Example:
   ```
   A
   B
   C
   A
   B
   C
   A
   B
   C
   D
   ```

2. **Contestant File** (e.g., `contestantFile.txt`):
   - Contains contestant IDs and their answers
   - Each line represents one contestant
   - Format: `[Contestant ID] [Answer1] [Answer2] ...`

   Example:
   ```
   0012345678 A B C A B C A B C D
   0012387654 A A A A A A A A A A
   0012364213 B A A A A A A A A A
   0012367214 C C C D B C A B A A
   ```

## Output
The program generates:
1. Individual contestant reports with scores and incorrect answers
2. Overall statistical summary including mean, median, and mode of scores
3. List of most frequently missed questions (if any have a miss rate of 60% or higher)

## Sample Files
The repository includes sample input files:
- `answerFile.txt`: Contains 10 correct answers
- `contestantFile.txt`: Contains responses from 4 contestants

These files demonstrate the input format and can be used to test the program's functionality.

## Notes
- The program showcases modern C++ features and should be compiled with C++14 or later.
- Error handling is implemented for file operations and memory allocations to ensure robustness.
- The project demonstrates efficient use of memory and processing resources, making it suitable for large-scale quiz analysis.
