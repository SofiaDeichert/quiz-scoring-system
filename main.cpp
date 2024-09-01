#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

//Create a structure to represent our contestant, who has an ID and answer choices
struct Contestant
{
	string id{};				   //create a string object to store the contestant ID in
	unique_ptr<char[]> answers{};  //create a unique pointer to a character array to store the contestant's answers
};

//Function forward declarations so our functions are known before they are called

//Data(file) processing
unique_ptr<char[]> processAnswerKey(const string& answerKeyFilename, size_t& numQuestions);
unique_ptr<Contestant[]> processContestantFile(const string& contestantsFilename, const size_t numQuestions, size_t& numContestants);
//Core logic
unique_ptr<double[]> calculateContestantScores(const unique_ptr<Contestant[]>& contestants, size_t numContestants, const unique_ptr<char[]>& correctAnswers, size_t numQuestions);
double calculateMean(const unique_ptr<double[]>& allScores, size_t numContestants);
double calculateMedian(const unique_ptr<double[]>& sortedScores, size_t numContestants);
vector<double> calculateModes(const unique_ptr<double[]>& sortedScores, size_t numContestants);
unique_ptr<double[]> sortScoresArray(const unique_ptr<double[]>& allScores, size_t numContestants);
unique_ptr<size_t[]> findMostMissedQuestions(const unique_ptr<Contestant[]>& contestants, size_t numContestants, const unique_ptr<char[]>& correctAnswers, size_t numQuestions);
//Output
void printContestantReports(const unique_ptr<Contestant[]>& contestants, const unique_ptr<double[]>& allScores, size_t numContestants, const unique_ptr<char[]>& correctAnswers, size_t numQuestions);
void printMean(const unique_ptr<double[]>& allScores, size_t numContestants);
void printMedian(const unique_ptr<double[]>& sortedScores, size_t numContestants);
void printModes(const vector<double>& modes);
void printMostMissedQuestions(const unique_ptr<size_t[]> missedQuestionsCounter, size_t numContestants, size_t numQuestions);
//Report summary
void createReportSummary(const unique_ptr<double[]>& allScores, size_t numContestants, const unique_ptr<Contestant[]>& contestants, const unique_ptr<char[]>& correctAnswers, size_t numQuestions, const unique_ptr<double[]>& sortedScores, const vector<double>& modes);

int main()
{
	string answerKeyFilename{};		//create string variable to store answer key filename
	string contestantsFilename{};   //create string variable to store contestant responses file name

	//prompt user to enter name of answer key file, and store it in 'answerKeyFilename' variable
	cout << "Enter answer file name: ";
	cin >> answerKeyFilename;
	cout << '\n';

	//prompt user to enter name of the contestant responses file, and store it in 'contestantsFilename' variable
	cout << "Enter contestant file name: ";
	cin >> contestantsFilename;

	size_t numQuestions{ 0 };   //variable to keep track of number of questions in quiz - will be modified by 'processAnswerKey'
	size_t numContestants{ 0 }; //variable to keep track of number of contestants - will be modified by 'processContestantFile'

	//Call 'processAnswerKey' function to read the correct answers from the file:
	//create smart pointer for dynamic array and initialize with returned dynamic array of correct answers from function call
	//also modifies 'numQuestions' to reflect actual count
	unique_ptr<char[]> correctAnswers = processAnswerKey(answerKeyFilename, numQuestions);

	//Call 'processContestantFile' function to read the contestants' info from the file:
	//create smart pointer for dynamic array and initialize with returned dynamic array of 'Contestant' objects from function call
	//also modifies 'numContestants' to reflect actual count
	unique_ptr<Contestant[]> contestants = processContestantFile(contestantsFilename, numQuestions, numContestants);

	//Call 'calculateContestantScores' function to calcaulate scores for each contestant based on their answers:
	//create smart pointer for dynamic array and initialize with returned dynamic array of all contestants' scores from function call
	unique_ptr<double[]> allScores = calculateContestantScores(contestants, numContestants, correctAnswers, numQuestions);

	//Call 'printContestantReports' to print detailed report for each contestant
	printContestantReports(contestants, allScores, numContestants, correctAnswers, numQuestions);

	//Call 'sortScoresArray' to create a sorted scores dynamic array for report summary calculations
	unique_ptr<double[]> sortedScores = sortScoresArray(allScores, numContestants);

	//Call 'calculateModes' to find mode(s) for score distribution
	vector<double> modes = calculateModes(sortedScores, numContestants);

	//call 'createReportSummary' to create and display summary report based on all contestants' scores:
	//this includes calculating and displaying the mean, median, mode, and most missed questions
	createReportSummary(allScores, numContestants, contestants, correctAnswers, numQuestions, sortedScores, modes);

	return 0; //return 0 to indicate successful completion of program
}

//Function to process the answer key file and store answers into dynamically allocated array
unique_ptr<char[]> processAnswerKey(const string& answerKeyFilename, size_t& numQuestions) 
{
	//create input filestream object for answerkey file(in reading mode)
	ifstream afile(answerKeyFilename, ios::in);

	if (!afile) //if answer key file cannot be successfully opened
	{
		cerr << "answer file could not be opened for reading.\n"; //print an error to the console 
		exit(1); //and exit the program
	}
	
	char correctAnswer{}; //create temporary variable to hold each correct answer read from file

	//read characters(correct answers) from answerkey file one by one until reaching the end
	while (afile >> correctAnswer)
	{
		++numQuestions; //each time a character(answer) is read in, increase our numQuestions counter by one
	}

	/* Allocate memory dynamically for storing correct answers :
	   'unique_ptr<char[]>' is a smart pointer(stores memory address of another variable) providing automatic memory management
	   'new char[numQuestions]' creates new array of characters with size equal to 'numQuestions' we counted above
	    array is then managed by 'answers', which is our 'unique_ptr  */
	unique_ptr<char[]> answers(new char[numQuestions]);

	afile.clear();				//clear our end of file flag so the file can be read again
	afile.seekg(0, ios::beg);	//move read position of file pointer back to the beginning

	//iterate through each correct answer in the answerkey file
	for (size_t i = 0; i < numQuestions; ++i)
	 /* read correct answers into dynamic array managed by unique_ptr(smart pointer) :
	    1. 'answers.get()' gets raw pointer from smart pointer which points to beginning of dynamic array
	    2. Adding 'i' to this pointer moves location it points to forward by 'i' places 
	    3. '*' before '(answers.get() + i)' dereferences pointer, meaning it gets value that's located at memory address now pointed to
	    4. 'afile >>' reads one character from file and stores it in location pointed to by dereferenced pointer */
	{
		afile >> *(answers.get() + i); 
	}
	afile.close(); //close answer key file
	return answers; //return unique_ptr(smart pointer) managing dynamically allocated array of correct answers
}

//Function to process the Contestant file; reads contestant data(id and contestant answers) into dynamically allocated array
unique_ptr<Contestant[]> processContestantFile(const string& contestantsFilename, const size_t numQuestions, size_t& numContestants)	
{
	//create input filestream object for contestant file(reading mode)
	ifstream cfile(contestantsFilename, ios::in);

	//if contestant file cannot be successfully opened
	if (!cfile)
	{
		cerr << "contestant file could not be opened for reading.\n"; //print an error to the console
		exit(1); //and exit the program
	}

	//create temporary variable to hold each line from file(in order to count number of lines - which is total number of contestants)_
	string temp{};

	//while there are still lines to process, store each line into temp string object (each line represents one contestant)
	while (getline(cfile, temp)) 
	{
		++numContestants;  //increase number of contestants by one for each line read
	}

	cfile.clear();				//after counting, clear our end of file flag so the file can be read again
	cfile.seekg(0, ios::beg);   //move read position of file pointer back to the beginning

	//create dynamically allocated array of type 'Contestant'(holds the id and answers) with size of 'numContestants' counted above
	unique_ptr<Contestant[]> contestants(new Contestant[numContestants]);

	//iterate through each contestant in file
	for (size_t i = 0; i < numContestants; ++i)
	{
		//read and store contestant's id number
		cfile >> (contestants.get() + i) -> id;

		//allocate memory for contestant's answers
		(contestants.get() + i)->answers = unique_ptr<char[]>(new char[numQuestions]);

		//iterate through each question for the contestant
		for (size_t j = 0; j < numQuestions; ++j)
		{
			/* store each answer :
			   1. '(contestants.get() + i)' gets address of ith contestant
			   2. '->answers.get()' accesses unique_ptr managing answers array for this contestant
			   3. Adding 'j' moves to correct position for this particular answer
			   4. The '*' operator then 'dereferences'(gets the value for) this address, allowing us to store answer read from file */
			cfile >> *((contestants.get() + i)->answers.get() + j);
		}
	}
	cfile.close(); //close contestant file
	return contestants; //return unique_ptr managing dynamically allocated array of Contestants, which holds their IDs and answers
}

//Function to calculate contestant scores
unique_ptr<double[]> calculateContestantScores(const unique_ptr<Contestant[]>& contestants, size_t numContestants, const unique_ptr<char[]>& correctAnswers, size_t numQuestions)
{
	//create dynamically allocated array to store scores for all contestants
	unique_ptr<double[]> allScores(new double[numContestants]);

	//iterate through each contestant to calculate score and evaluate answers
	for (size_t i = 0; i < numContestants; ++i)
	{
		size_t correctAnswerCount{ 0 }; //create variable to track number of correct answers for current contestant
	
		//for each question of current contestant
		for (size_t j = 0; j < numQuestions; ++j)
		{
		//if correct answer matches the contestant answer
			if (*(correctAnswers.get() + j) == *((contestants.get() + i)->answers.get() + j))
			{
			++correctAnswerCount; //increment correct answer counter by one
			}
		}
		//calculate contestant's score and store it:
		//divide number of correct answers by total number of questions to get score, multiply by 100 to get as percent
		double score{ static_cast<double>(correctAnswerCount) / static_cast<double>(numQuestions) * 100.00 };
		*(allScores.get() + i) = score; //assign calculated score to corresponding position in 'allScores' array
	}
	return allScores; //return unique_ptr managing dynamically allocated array containing all scores
}

//Function to calculate mean of all the contestant scores
double calculateMean(const unique_ptr<double[]>& allScores, size_t numContestants)
{
	double sum{ 0.0 }; //start with sum of 0.0 to accumulate all scores
	//iterate through each contestant
	for (size_t i = 0; i < numContestants; ++i)
	{
		sum += *(allScores.get() + i); //access the score of the ith contestant and add that to the sum
	}
	//calculate mean by dividing total sum by total number of contestants
	double mean{ sum / static_cast<double>(numContestants) };

	return mean; //return calculated mean
}

//Function to calculate the median of the contestants' scores
double calculateMedian(const unique_ptr<double[]>& sortedScores, size_t numContestants)
{
	//calculate median score from sorted scores array:
	double median{ 0.0 }; //create median variable with value 0.0 (to be replaced with actual median)

	if (numContestants % 2 == 1) //if we have an odd number of scores
	{

		median = *(sortedScores.get() + (numContestants / 2)); //our median is the value at the middle index (numContestants/2)
	}
	else //otherwise, if we have an even number of scores
	{
		//the median is the average of the two middle scores located at indices [numContestants/2 - 1] and [numContestants / 2]
		//add the value at the first middle index to the value at the second middle index, and divide the total by 2 to get the median
		median = (*(sortedScores.get() + ((numContestants / 2) - 1)) + *(sortedScores.get() + (numContestants / 2))) / 2.0;
	}
	return median; //return calculated median
}

//Function to find the mode(s) of the scores
vector<double> calculateModes(const unique_ptr<double[]>& sortedScores, size_t numContestants)
{
	//create a vector to store the mode(s)
	vector<double> modes;

	double score = *(sortedScores.get()); //store first score as current score to compare against.
	double previousScore{ score };		  //create variable previousScore with value of first score for comparison
	size_t count{ 0 };					  //create count variable to track amount of times current score appears	
	size_t highestFrequency{ 0 };		  //track highest frequency of any score seen so far

	//iterate through all scores to find mode(s)
	for (size_t i = 0; i <= numContestants; ++i)
	{
		score = *(sortedScores.get() + i); //get current score in sortedScores array
		if (score == previousScore) //if current score is same as previous score
		{
			++count; //increment our count by one
		}

		//if we find new score or reach end of the sortedScores array
		if (score != previousScore || i == numContestants)
		{
			//if count of previous score is higher than highest frequency seen so far
			if (count > highestFrequency)
			{
				modes.clear();					//clear modes vector
				modes.push_back(previousScore); //add this score to vector as new mode
				highestFrequency = count;		//update the highest frequency seen so far
			}
			//if count of previous score equals highest frequency (case where we have multiple modes)
			else if (count == highestFrequency)
			{
				modes.push_back(previousScore); //add score to modes vector without clearing it
			}
			//prepare for next score:	
			count = 1; //reset count for new score
			if (i != numContestants) //if we are not on the last score
			{
				previousScore = score; //update previousScore to current score
			}
		}
	}
	return modes; //return our vector containing the mode(s)
}

//Function to sort the scores array in ascending order
unique_ptr<double[]> sortScoresArray(const unique_ptr<double[]>& allScores, size_t numContestants)
{
	//create new dynamically allocated array to hold sorted scores
	unique_ptr<double[]> sortedScores(new double[numContestants]);

	//copy all array values from allScores to sortedScores: 
	for (size_t i = 0; i < numContestants; ++i)
	{
		// *(sortedScores.get() + i) = *(allScores.get() + i) copies the ith score to sortedScores array
		*(sortedScores.get() + i) = *(allScores.get() + i);
	}

	/* Sort scores in ascending order using sort function :
	   'sortedScores.get()' provides pointer to first element in array
	   'sortedScores.get() + numContestants' provides a pointer just past last element,
	    which gives sort function the correct range */
	sort(sortedScores.get(), sortedScores.get() + numContestants);

	return sortedScores; //return array of sorted scores
}

//Function to find most missed Questions(60% or higher miss rate)
unique_ptr<size_t[]> findMostMissedQuestions(const unique_ptr<Contestant[]>& contestants, size_t numContestants, const unique_ptr<char[]>& correctAnswers, size_t numQuestions)
{
	//create dynamic array to count number of times each question was missed
	//array is initialized with zeros, corresponding to each question
	unique_ptr<size_t[]> missedQuestionsCounter(new size_t[numQuestions]{});

	//iterate through each contestant to evaluate their answers
	for (size_t i = 0; i < numContestants; ++i)
	{
		//within each contestant, iterate through all questions to check their answers
		for (size_t j = 0; j < numQuestions; ++j)
		{
			//if question was answered incorrectly (correct answer doesn't match contestant's answer)
			if (*(correctAnswers.get() + j) != *((contestants.get() + i)->answers.get() + j))
			{
				//increment counter for that question in missedQuestionsCounter array
				//this counts how many times each question was missed across all contestants
				*(missedQuestionsCounter.get() + j) += 1;
			}
		}
	}
	return missedQuestionsCounter; //return our missedQuestionsCounter array
}

//Function to print out contestant reports
void printContestantReports(const unique_ptr<Contestant[]>& contestants, const unique_ptr<double[]>& allScores, size_t numContestants, const unique_ptr<char[]>& correctAnswers, size_t numQuestions)
{
	//iterate through each contestant 
	for (size_t i = 0; i < numContestants; ++i)
	{
		//display contestant ID and score to console
		cout << (contestants.get() + i)->id << " - " << fixed << setprecision(2) << *(allScores.get() + i) << '\n'; 

		/* allocate dynamic arrays to keep track of which questions were answered incorrectly
		   'incorrectQuestionIndices' will hold indices of questions that were missed
		   'incorrectAnswers' will hold what contestant answered for these missed questions */
		unique_ptr<size_t[]> incorrectQuestionIndices(new size_t[numQuestions]);
		unique_ptr<char[]> incorrectAnswers(new char[numQuestions]);

		size_t incorrectCount{ 0 }; //create variable to count number of incorrect answers

		for (size_t j = 0; j < numQuestions; ++j)
		{
			//if correct answer does not match the contestant answer
			if (*(correctAnswers.get() + j) != *((contestants.get() + i)->answers.get() + j))
			{
				//store index(position) of incorrect question in 'inocrrectQuestionsIndices' array
				*(incorrectQuestionIndices.get() + incorrectCount) = j; 
				//store incorrect contestant answer in 'incorrectAnswers' array
				*(incorrectAnswers.get() + incorrectCount) = *((contestants.get() + i)->answers.get() + j); 
				++incorrectCount; //increment count of incorrect answers by one
			}
		}

		//if contestant has any incorrect answers
		if (incorrectCount > 0)
		{
			//iterate through incorrectQuestionIndices array to list out questions contestant missed
			for (size_t k = 0; k < incorrectCount; ++k)
			{
				//print each missed questions number(add 1 since arrays start at index 0, and question numbers start at 1)
				cout << *(incorrectQuestionIndices.get() + k) + 1 << " ";
			}
			cout << '\n'; //move to new line for separation

			//iterate through incorrectAnswers array to display what contestant answered incorrectly
			for (size_t k = 0; k < incorrectCount; ++k)
			{
				cout << *(incorrectAnswers.get() + k) << " "; //print each incorrect answer given by contestant
			}
			cout << '\n'; //move to new line for separation

			//iterate through array of incorrect answers to display correct answers for corresponding questions
			for (size_t k = 0; k < incorrectCount; ++k)
			{
				/* print correct answer for each question contestant missed :
				//access correct answer using the index from the incorrectQuestionIndices array,
				//which points to corresponding question number that contestant got wrong. */
				cout << *(correctAnswers.get() + *(incorrectQuestionIndices.get() + k)) << " ";
			}
			cout << '\n'; //move to new line separation
		}
		cout << '\n'; //add blank line for separation between contestant reports
	}
}

//Function to print the mean of the scores
void printMean(const unique_ptr<double[]>& allScores, size_t numContestants)
{
	//print mean formatted to two decimal places
	cout << "Mean: " << setprecision(2) << calculateMean(allScores, numContestants) << '\n'; 
}

//Function to print median of the scores
void printMedian(const unique_ptr<double[]>& sortedScores, size_t numContestants)
{
	//print median formatted to two decimal places
	cout << "Median: " << setprecision(2) << calculateMedian(sortedScores, numContestants) << '\n';
}

//Function to print the calculated modes
void printModes(const vector<double>& modes) 
{
	cout << "Mode: ";
	//for each mode in our modes vector
	for (size_t i = 0; i < modes.size(); ++i)
	{
		cout << setprecision(2) << modes.at(i); //print mode to two decimal places
		if (i < modes.size() - 1)
		{
			cout << ", "; //add comma if we are not on the last mode
		}
	}
	cout << '\n';
}

//Function to print mossed missed questions and their miss rate
void printMostMissedQuestions(const unique_ptr<size_t[]> missedQuestionsCounter, size_t numContestants, size_t numQuestions)
{
	//find most missed question(s) by contestants
	cout << "MOST MISSED QUESTIONS" << '\n';

	//iterate through each question to calculate and display its miss rate
	for (size_t j = 0; j < numQuestions; ++j)
	{
		//calculate miss rate for each question by dividing number of times it was missed by total number of contestants
		double missRate{ static_cast<double>(*(missedQuestionsCounter.get() + j)) / static_cast<double>(numContestants) };

		//if miss rate for question is 60% or higher, report it as a most missed question
		if (missRate >= 0.60)
		{
			//print question number (adjusting from zero-based indexing by adding 1),
			//print miss rate formatted as a percentage (multiply by 100 to convert from decimal to percentage)
			cout << (j + 1) << "\t" << setprecision(2) << (missRate * 100) << "%\n";
		}
	}
}

//Function to create report summary for the contestants
void createReportSummary(const unique_ptr<double[]>& allScores, size_t numContestants, const unique_ptr<Contestant[]>& contestants, const unique_ptr<char[]>& correctAnswers, size_t numQuestions, const unique_ptr<double[]>& sortedScores, const vector<double>& modes)
{
	//print mean of all scores to console
	printMean(allScores, numContestants);

	//sort array of all scores before calculating median and mode
	sortScoresArray(allScores, numContestants);
	
	//print calculated median to console
	printMedian(sortedScores, numContestants);

	//print mode(s) to console
	printModes(modes);

	//if question has 60% or higher miss rate, print question number and corresponding miss rate
	printMostMissedQuestions(findMostMissedQuestions(contestants, numContestants, correctAnswers, numQuestions), numContestants, numQuestions);	
}
