//#include "pch.h" //for Visual Studio 2017
#include <iostream> 
#include <queue> 
#include <fstream>
#include <cstring>
#include <ctime>

using namespace std;

struct Job {
	int number;
	int arrivaltime;
	int tasktime[3];
	int taskstart[3];
	int stage;
	int getCurrentTaskTime() {
		return tasktime[stage];
	}
	int getRemainingTime() {
		if (stage == 0) {
			return tasktime[0] + tasktime[1] + tasktime[2];
		}
		else if (stage == 1) {
			return tasktime[1] + tasktime[2];
		}
		else if (stage == 2) {
			return tasktime[2];
		}
		else {
			return 0;
		}
	}
	void start(const int t) {
		taskstart[stage] = t;
	}
};

struct WorkStation {
	bool free;
	int taskRemainingTime;
	int jobNumber;
	int release() {
		int releasedJob = jobNumber;
		jobNumber = -1;
		return releasedJob;
	}
};

struct JobQueue {
	int number;
	int remainingtime;
	bool operator<(const JobQueue& rhs) const
	{
		return remainingtime < rhs.remainingtime;
	}
};

void display_help() {
	cout << "Usage:" << endl
		<< "algoBOWL_group2 [Input File] [Output File]" << endl
		<< "- Generate output from input." << endl
		<< "algoBOWL_group2 -v [Input File] [OutputFIle]" << endl
		<< "- validate a pair of input and output" << endl;
	return;
}

int longestRemaingTime(const char inputFileName[],const char outputFileName[]) {

	ifstream inputFile;
	inputFile.open(inputFileName);

	if (!inputFile.is_open()) {
		cerr << "Couldn't read " << inputFileName << endl;
		return 1;
	}

	//Initialize input
	int n = 0;
	inputFile >> n;
	int m = 0;
	inputFile >> m;

	Job jobs[1000] = { 0 };
	for (int i = 0; i < n; i++) {
		jobs[i].number = i;
		inputFile >> jobs[i].arrivaltime;
		for (int j = 0; j < m; j++) {
			inputFile >> jobs[i].tasktime[j];
		}
	}
	
	inputFile.close();

	//Job jobs[1000] = { 0 };
	//for (int i = 0; i < n; i++) {
	//	jobs[i].number = i;
	//	//jobs[i].arrivaltime = rand() % 5;
	//	jobs[i].arrivaltime = 0;
	//	for (int j = 0; j < 3; j++) {
	//		jobs[i].tasktime[j] = rand() % 5 + 1;
	//	}
	//}

	//Print input

	for (int i = 0; i < n; i++) {
		cout << jobs[i].number << "\t";
		cout << jobs[i].arrivaltime << "\t";
		for (int j = 0; j < 3; j++) {
			cout << jobs[i].tasktime[j] << "\t";
		}
		cout << endl;
	}
	cout << endl;

	//Setup system

	priority_queue <JobQueue> queue;
	int t = 0;
	int jobDone = 0;
	WorkStation workStation[3];

	for (int i = 0; i < m; i++) {
		workStation[i].free = true;
	}

	//Start crunching

	while (1) {

		//Add new arrival jobs to queue
		for (int i = 0; i < n; i++) {
			if (t <= 50 && jobs[i].arrivaltime == t) {
				JobQueue newjob;
				newjob.number = jobs[i].number;
				newjob.remainingtime = jobs[i].getRemainingTime();
				queue.push(newjob);
			}
		}

		//Check for task completion, push task finished job back to queue
		for (int i = 0; i < m; i++) {
			if (!workStation[i].free && workStation[i].taskRemainingTime == 0) {
				int releasedJob = workStation[i].release();
				workStation[i].free = true;
				jobs[releasedJob].stage++;
				if (jobs[releasedJob].stage == 3) {
					jobDone++;
				}
				else {
					JobQueue backToQueue;
					backToQueue.number = jobs[releasedJob].number;
					backToQueue.remainingtime = jobs[releasedJob].getRemainingTime();
					queue.push(backToQueue);
				}
			}
		}

		//Shutting down if all jobs finsished
		if (jobDone == n) break;

		//Assign job(s) to free workstation
		for (int i = 0; i < m; i++) {
			if (workStation[i].free) {
				if (!queue.empty()) {
					workStation[i].free = false;
					int newJobNumber = queue.top().number;
					workStation[i].taskRemainingTime = jobs[newJobNumber].getCurrentTaskTime();
					workStation[i].jobNumber = newJobNumber;
					queue.pop();
					jobs[newJobNumber].start(t);
				}
			}
		}

		//Execution stage
		t++;
		for (int i = 0; i < m; i++) {
			if (!workStation[i].free) {
				workStation[i].taskRemainingTime--;
			}
		}
	}

	//Print start time and total time

	for (int i = 0; i < n; i++) {
		cout << jobs[i].number << "\t\t";
		for (int j = 0; j < 3; j++) {
			cout << jobs[i].taskstart[j] << "\t";
		}
		cout << jobs[i].taskstart[2] + jobs[i].tasktime[2];
		cout << endl;
	}
	cout << "Total time: " << t << endl;

	ofstream outputFile;
	outputFile.open(outputFileName);
	if (!outputFile.is_open()) {
		cerr << "Error writing output file!" << endl;
		return 2;
	}
	
	outputFile << t << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			outputFile << jobs[i].taskstart[j] << " ";
		}
		outputFile << endl;
	}
	outputFile.close();

	system("pause");
	return 0;
}

int validator(const char inputFileName[], const char outputFileName[]) {
	
	ifstream inputFile;
	inputFile.open(inputFileName);
	if (!inputFile.is_open()) {
		cerr << "Couldn't read " << inputFileName << endl;
		return 1;
	}
	int n = 0;
	inputFile >> n;
	int m = 0;
	inputFile >> m;

	Job jobs[1000] = { 0 };
	for (int i = 0; i < n; i++) {
		jobs[i].number = i;
		inputFile >> jobs[i].arrivaltime;
		for (int j = 0; j < m; j++) {
			inputFile >> jobs[i].tasktime[j];
		}
	}

	inputFile.close();


}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		display_help();
	}
	else if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) {
		display_help();
	}
	else if (argc == 3) {
		clock_t c_start = clock();
		longestRemaingTime(argv[1], argv[2]);
		clock_t c_end = clock();
		cout << "Finished in "
			<< 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << " ms\n"
			;
		return 0;
	}
	else if (argc == 4 && !strcmp(argv[1], "-v")) {
		validator(argv[1], argv[2]);
		return 0;
	}
	else {
		cerr << "Invalid arguments." << endl;
		return 1;
	}
}