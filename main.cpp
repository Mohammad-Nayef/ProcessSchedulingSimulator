#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

struct process{
    string name;
    int arrival_time, bursts;
};

vector<process> processes;
int processes_count, context_switch_time, time_quantum;

int main() {
    ifstream input("..\\input.txt");
    string line;

    while (getline(input, line)) {
        if (line.substr(0, 20) == "Number of processes:") {
            stringstream stream(line.substr(20));
            stream >> processes_count;

            for (int i = 0; i < processes_count; i++) {
                process p;
                p.name = "P" + to_string(i + 1);
                processes.push_back(p);
            }
        }

        if (line.substr(0, 14) == "Arrival times:") {
            stringstream stream(line.substr(14));

            for (int i = 0; i < processes_count; i++) 
                stream >> processes[i].arrival_time;
        }

        if (line.substr(0, 11) == "CPU bursts:") {
            stringstream stream(line.substr(11));

            for (int i = 0; i < processes_count; i++) 
                stream >> processes[i].bursts;
        }

        if (line.substr(0, 35) == "Context switch time (milliseconds):") {
            stringstream stream(line.substr(35));
            stream >> context_switch_time;
        }

        if (line.substr(0, 29) == "Time quantum for Round Robin:") {
            stringstream stream(line.substr(29));
            stream >> time_quantum;
        }
    }
}