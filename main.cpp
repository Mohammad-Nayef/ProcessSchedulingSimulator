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
    int arrival_time, burst;
};

int processes_count, context_switch_time, time_quantum;
const int SLEEPING_INTERVAL = 1; // Milliseconds
const string CONTEXT_SWITCH = "Context switch", IDLE = "Idle";

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
                stream >> processes[i].burst;
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

bool by_arrival_time(pcb p1, pcb p2) {
    return p1.arrival_time < p2.arrival_time;
}

vector<string> FCFS(vector<pcb> processes)
{
    vector<string> timeline;
    cout << "First-Come First-Served timeline:\n\n";
    queue<int> ready_queue;

    for (int time = 0, index = 0; ;time++)
    {
        cout << time << ": ";

        if (index < processes_count && time >= processes[index].arrival_time)
        {
            ready_queue.push(index);
            index++;
        }

        if (ready_queue.empty())
        {
            if (index >= processes_count)
                break;

            cout << "idle, ";
            timeline.push_back(IDLE);
            Sleep(SLEEPING_INTERVAL);
            continue;
        }

        processes[ready_queue.front()].burst--;
        cout << processes[ready_queue.front()].name << "-burst, ";
        timeline.push_back(processes[ready_queue.front()].name);

        if (processes[ready_queue.front()].burst == 0)
        {
            ready_queue.pop();

            if (index == processes_count && ready_queue.empty())
                break;

            for (int interval = 0; interval < context_switch_time; interval++)
            {
                time++;
                cout << time << ": ";

                cout << "switching, ";
                timeline.push_back(CONTEXT_SWITCH);
                Sleep(SLEEPING_INTERVAL);
            }

            continue;
        }

        Sleep(SLEEPING_INTERVAL);
    }

    return timeline;
}

int main()
{
    vector<pcb> processes;
    processes_input(processes);
    sort(processes.begin(), processes.end(), by_arrival_time);
    vector<string> FCFS_timeline = FCFS(processes);

    for(string s: FCFS_timeline)
        cout << s << ' ' << endl;
}