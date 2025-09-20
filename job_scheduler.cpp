#include <iostream>
#include <queue>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>

using namespace std;

// Job class
class Job {
public:
    int id;
    string name;
    int priority;

    Job(int i, const string& n, int p) : id(i), name(n), priority(p) {}

    void display() const {
        cout << "ID: " << id << ", Name: " << name << ", Priority: " << priority << endl;
    }

    void saveToFile(ofstream& file) const {
        file << id << "|" << name << "|" << priority << endl;
    }

    static Job loadFromFile(const string& line) {
        int id, priority;
        string name, token;
        stringstream ss(line);

        getline(ss, token, '|');
        id = stoi(token);

        getline(ss, name, '|');

        getline(ss, token, '|');
        priority = stoi(token);

        return Job(id, name, priority);
    }
};

// Custom Comparator for priority queue
class ComparePriority {
public:
    bool operator()(const Job& j1, const Job& j2) {
        if (j1.priority == j2.priority)
            return j1.id > j2.id;

        return j1.priority > j2.priority;
    }
};

// JobScheduler class
class JobScheduler {
private:
    queue<Job> fcfsQueue;
    priority_queue<Job, vector<Job>, ComparePriority> priorityQueue;
    int jobCounter;
    

public:
    JobScheduler() : jobCounter(1) {
        loadJobsFromFile();
    }

    void addJob() {
        string name;
        int priority;
        cin.ignore();
        cout << "Enter Job Name: ";
        getline(cin, name);
        cout << "Enter Job Priority (lower number = higher priority): ";
        cin >> priority;

        Job newJob(jobCounter++, name, priority);
        fcfsQueue.push(newJob);
        priorityQueue.push(newJob);
        saveJobToFile(newJob);

        cout << "Job added successfully!\n";
    }

    void processFCFSJob() {
        if (fcfsQueue.empty()) {
            cout << "FCFS Queue is empty.\n";
            return;
        }

        Job job = fcfsQueue.front();
        fcfsQueue.pop();

        priority_queue<Job, vector<Job>, ComparePriority> tempQueue;
        while (!priorityQueue.empty()) {
            Job temp = priorityQueue.top();
            priorityQueue.pop();
            if (temp.id != job.id)
                tempQueue.push(temp);
        }
        priorityQueue = tempQueue;

        cout << "Processing FCFS Job: ";
        job.display();

        logCompletedJob(job);
        removeJobFromFile(job); // Remove processed job from jobs.txt
    }

    void processPriorityJob() {
        if (priorityQueue.empty()) {
            cout << "Priority Queue is empty.\n";
            return;
        }

        Job job = priorityQueue.top();
        priorityQueue.pop();

        queue<Job> tempQueue;
        while (!fcfsQueue.empty()) {
            Job temp = fcfsQueue.front();
            fcfsQueue.pop();
            if (temp.id != job.id)
                tempQueue.push(temp);
        }
        fcfsQueue = tempQueue;

        cout << "Processing Priority Job: ";
        job.display();

        logCompletedJob(job);
        removeJobFromFile(job); // Remove processed job from jobs.txt
    }

    void displayFCFSQueue() const {
        if (fcfsQueue.empty()) {
            cout << "FCFS Queue is empty.\n";
            return;
        }

        cout << "\nJobs in FCFS Queue:\n";
        queue<Job> temp = fcfsQueue;
        while (!temp.empty()) {
            temp.front().display();
            temp.pop();
        }
    }

    void displayPriorityQueue() const {
        if (priorityQueue.empty()) {
            cout << "Priority Queue is empty.\n";
            return;
        }

        cout << "\nJobs in Priority Queue:\n";
        priority_queue<Job, vector<Job>, ComparePriority> temp = priorityQueue;
        while (!temp.empty()) {
            temp.top().display();
            temp.pop();
        }
    }

    void displayMenu() const {

        cout << "\n========== Job Scheduling System ==========\n";
        cout << "1. Add Job\n";
        cout << "2. Process Job (FCFS)\n";
        cout << "3. Process Job (Priority-Based)\n";
        cout << "4. Display FCFS Queue\n";
        cout << "5. Display Priority Queue\n";
        cout << "6. Exit\n";
        cout << "==========================================\n";
        
    }

    void saveJobToFile(const Job& job) {
        ofstream file("jobs.txt", ios::app);
        if (file.is_open()) {
            job.saveToFile(file);
            file.close();
        } else {
            cout << "Error saving job to file.\n";
        }
    }

    void loadJobsFromFile() {
        ifstream file("jobs.txt");
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            Job job = Job::loadFromFile(line);
            // Check if job is already completed (i.e., in completed_jobs.txt)
            ifstream completedFile("completed_jobs.txt");
            string completedLine;
            bool isCompleted = false;

            while (getline(completedFile, completedLine)) {
                if (completedLine == line) {
                    isCompleted = true;
                    break;
                }
            }

            completedFile.close();

            // If job isn't completed, add it to queues
            if (!isCompleted) {
                fcfsQueue.push(job);
                priorityQueue.push(job);
                jobCounter = max(jobCounter, job.id + 1);
            }
        }
        file.close();
    }

    void logCompletedJob(const Job& job) {
        ofstream file("completed_jobs.txt", ios::app);
        if (file.is_open()) {
            job.saveToFile(file);
            file.close();
        } else {
            cout << "Error logging completed job.\n";
        }
    }

    // Method to remove completed jobs from jobs.txt
    void removeJobFromFile(const Job& job) {
        ifstream inputFile("jobs.txt");
        ofstream tempFile("temp_jobs.txt");

        string line;
        while (getline(inputFile, line)) {
            Job currentJob = Job::loadFromFile(line);
            if (currentJob.id != job.id) {
                tempFile << line << endl;
            }
        }

        inputFile.close();
        tempFile.close();

        // Replace jobs.txt with the new file
        remove("jobs.txt");
        rename("temp_jobs.txt", "jobs.txt");
    }
};

// Main function
int main() {
    JobScheduler scheduler;
    int choice;

    do {
        scheduler.displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: scheduler.addJob(); break;
            case 2: scheduler.processFCFSJob(); break;
            case 3: scheduler.processPriorityJob(); break;
            case 4: scheduler.displayFCFSQueue(); break;
            case 5: scheduler.displayPriorityQueue(); break;
            case 6: cout << "Exiting Job Scheduling System.\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 6);

    return 0;
}


