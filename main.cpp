#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <windows.h>

using namespace std;

// Process control block
struct pcb {
    string name;
    int arrival_time, burst_time, start_time, finish_time;
};

int processes_count, context_switch_time, time_quantum;
const int SLEEPING_INTERVAL = 1; // Milliseconds
const string CONTEXT_SWITCH = "CS", IDLE = "Idle";

void processes_input(vector<pcb> &processes) {
    ifstream input("..\\input.txt");
    string line;

    while (getline(input, line))
    {
        if (line.substr(0, 20) == "Number of processes:")
        {
            stringstream stream(line.substr(20));
            stream >> processes_count;

            for (int i = 0; i < processes_count; i++)
            {
                pcb p;
                p.name = "P" + to_string(i + 1);
                processes.push_back(p);
            }
        }

        if (line.substr(0, 14) == "Arrival times:")
        {
            stringstream stream(line.substr(14));

            for (int i = 0; i < processes_count; i++)
                stream >> processes[i].arrival_time;
        }

        if (line.substr(0, 10) == "CPU burst:")
        {
            stringstream stream(line.substr(10));

            for (int i = 0; i < processes_count; i++)
                stream >> processes[i].burst_time;
        }

        if (line.substr(0, 35) == "Context switch time (milliseconds):")
        {
            stringstream stream(line.substr(35));
            stream >> context_switch_time;
        }

        if (line.substr(0, 29) == "Time quantum for Round Robin:")
        {
            stringstream stream(line.substr(29));
            stream >> time_quantum;
        }
    }
}

bool by_arrival_time(pcb process1, pcb process2) {
    return process1.arrival_time < process2.arrival_time;
}

bool by_name(pcb process1, pcb process2) {
    return process1.name < process2.name;
}

vector<string> get_FCFS_timeline(vector<pcb> processes)
{
    vector<string> timeline;
    cout << "First-Come First-Served live simulation timeline:\n\n";
    queue<int> ready_queue;

    for (int time = 0, index = 0; ;time++)
    {
        if (index < processes_count && time >= processes[index].arrival_time)
        {
            ready_queue.push(index);
            index++;
        }

        if (ready_queue.empty())
        {
            if (index >= processes_count)
                break;

            Sleep(SLEEPING_INTERVAL);
            cout << time << ": idle, ";
            timeline.push_back(IDLE);
            continue;
        }

        processes[ready_queue.front()].burst_time--;
        Sleep(SLEEPING_INTERVAL);
        cout << time << ": " << processes[ready_queue.front()].name << "-burst, ";
        timeline.push_back(processes[ready_queue.front()].name);

        if (processes[ready_queue.front()].burst_time == 0)
        {
            ready_queue.pop();

            if (index == processes_count && ready_queue.empty())
                break;

            for (int interval = 0; interval < context_switch_time; interval++)
            {
                time++;
                Sleep(SLEEPING_INTERVAL);
                cout << time << ": switching, ";
                timeline.push_back(CONTEXT_SWITCH);
            }

            continue;
        }
    }

    cout << "\n\n";
    return timeline;
}

int main()
{
    vector<pcb> processes;
    processes_input(processes);
    sort(processes.begin(), processes.end(), by_arrival_time);
    vector<string> timeline = get_FCFS_timeline(processes);
    int i;
    cout << "Gantt chart for FCFS:\n\n";

    for (i = 0; i < timeline.size(); i++) {
        cout << i << '\t' << timeline[i] << '\t';

        for (int j = i; j < timeline.size(); j++) {
            if (j + 1 < timeline.size() && timeline[j] == timeline[j + 1]) 
                    i++;
            else break;
        }
     }

    cout << i << "\n\n";

    for (int i = 0; i < processes_count; i++) {
        for (int j = 0 ; j < timeline.size(); j++) {
            if (processes[i].name == timeline[j]) {
                processes[i].start_time = j;
                break;
            }
        }

        for (int j = timeline.size() - 1; j > 0; j--) {
            if (processes[i].name == timeline[j]) {
                processes[i].finish_time = j + 1;
                break;
            }
        }
    }
    
    sort(processes.begin(), processes.end(), by_name);
    cout << "Finish time for each process:\n";

    for (int i = 0; i < processes_count; i++) 
        cout << processes[i].name << ": " << processes[i].finish_time << endl;

    cout << "\nWaiting time for each process:\n";

    for (int i = 0; i < processes_count; i++) {
        cout << processes[i].name << ": " << 
            processes[i].finish_time - processes[i].burst_time - processes[i].arrival_time << endl;
    }

    cout << "\nTurnaround time for each process:\n";

    for (int i = 0; i < processes_count; i++) {
        cout << processes[i].name << ": " << 
            processes[i].finish_time - processes[i].arrival_time << endl;
    }

    cout << "\nCPU utilization: ";
    int bursts_sum = 0;

    for (int i = 0; i < processes_count; i++) {
        bursts_sum += processes[i].burst_time;
    }

    cout << (float(bursts_sum) / timeline.size()) * 100 << "%\n";
}