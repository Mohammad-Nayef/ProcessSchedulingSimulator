#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <windows.h>
#include <map>
#include <climits>

using namespace std;

// Process control block
struct pcb {
    string name;
    int arrival_time, burst_time, start_time, finish_time;
};

int processes_count, context_switch_time, time_quantum;
const int SLEEPING_INTERVAL = 0; // Milliseconds
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

        if (line.substr(0, 11) == "CPU bursts:")
        {
            stringstream stream(line.substr(11));

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

void context_switch(int &time, vector<string> &timeline)
{
    for (int interval = 0; interval < context_switch_time; interval++)
    {
        time++;
        Sleep(SLEEPING_INTERVAL);
        cout << time << ": switching, ";
        timeline.push_back(CONTEXT_SWITCH);
    }
}

vector<string> get_first_come_first_served_timeline(vector<pcb> processes) {
    cout << "First-Come First-Served live simulation timeline:\n\n";
    vector<string> timeline;
    queue<int> ready_queue;

    for (int time = 0, index = 0; ; time++) {
        if (index < processes_count && time >= processes[index].arrival_time) {
            ready_queue.push(index);
            index++;
        }

        if (ready_queue.empty()) {
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

        if (processes[ready_queue.front()].burst_time == 0) {
            ready_queue.pop();

            if (index == processes_count && ready_queue.empty())
                break;

            context_switch(time, timeline);
        }
    }

    cout << "\n\n";
    return timeline;
}

vector<string> get_round_robin_timeline(vector<pcb> processes) {
    cout << "Round Robin live simulation timeline:\n\n";
    vector<string> timeline;
    queue<int> ready_queue;
    map<string, int> quantum;

    for (int i = 0; i < processes_count; i++)
        quantum[processes[i].name] = time_quantum;

    for (int time = 0, index = 0; ; time++) {
        if (index < processes_count && time >= processes[index].arrival_time) {
            ready_queue.push(index);
            index++;
        }

        if (ready_queue.empty()) {
            if (index >= processes_count)
                break;

            Sleep(SLEEPING_INTERVAL);
            cout << time << ": idle, ";
            timeline.push_back(IDLE);
            continue;
        }

        processes[ready_queue.front()].burst_time--;
        quantum[processes[ready_queue.front()].name]--;
        Sleep(SLEEPING_INTERVAL);
        cout << time << ": " << processes[ready_queue.front()].name << "-burst, ";
        timeline.push_back(processes[ready_queue.front()].name);

        if (processes[ready_queue.front()].burst_time == 0) {
            ready_queue.pop();

            if (index == processes_count && ready_queue.empty())
                break;

            context_switch(time, timeline);
        }
        else if (quantum[processes[ready_queue.front()].name] == 0) {
            quantum[processes[ready_queue.front()].name] = time_quantum;
            ready_queue.push(ready_queue.front());
            ready_queue.pop();

            context_switch(time, timeline);
        }
    }

    cout << "\n\n";
    return timeline;
}

vector<string> get_shortest_remaining_time_timeline(vector<pcb> processes) {
    cout << "Shortest Remaining Time live simulation timeline:\n\n";
    vector<string> timeline;
    vector<pcb> ready_queue;

    for (int time = 0, index = 0; ; time++) {
        if (index < processes_count && time >= processes[index].arrival_time) {
            ready_queue.push_back(processes[index]);
            index++;
        }

        if (ready_queue.empty()) {
            if (index >= processes_count)
                break;

            Sleep(SLEEPING_INTERVAL);
            cout << time << ": idle, ";
            timeline.push_back(IDLE);
            continue;
        }
        
        int min_remaining_time = INT_MAX, min_process = 0;
        
        for (int i = 0; i < ready_queue.size(); i++) {
            if (ready_queue[i].burst_time < min_remaining_time) {
                min_remaining_time = ready_queue[i].burst_time;
                min_process = i;
            }
        }

        ready_queue[min_process].burst_time--;
        Sleep(SLEEPING_INTERVAL);
        cout << time << ": " << ready_queue[min_process].name << "-burst, ";
        timeline.push_back(ready_queue[min_process].name);

        if (ready_queue[min_process].burst_time == 0) {
            ready_queue.erase(ready_queue.begin() + min_process);

            if (index == processes_count && ready_queue.empty())
                break;

            context_switch(time, timeline);
        }
    }

    cout << "\n\n";
    return timeline;
}

void print_gantt_chart(vector<string> timeline) {
    int i;
    cout << "Gantt chart:\n";

    for (i = 0; i < timeline.size(); i++) {
        cout << i << "   " << timeline[i] << "   ";

        for (int j = i; j < timeline.size(); j++) {
            if (j + 1 < timeline.size() && timeline[j] == timeline[j + 1])
                i++;
            else break;
        }
    }

    cout << i << "\n\n";
}

void set_start_and_finish_time(vector<pcb> &processes, vector<string> timeline) {
    for (int i = 0; i < processes_count; i++) {
        for (int j = 0; j < timeline.size(); j++) {
            if (processes[i].name == timeline[j]) {
                processes[i].start_time = j;
                break;
            }
        }

        for (int j = timeline.size() - 1; j >= 0; j--) {
            if (processes[i].name == timeline[j]) {
                processes[i].finish_time = j + 1;
                break;
            }
        }
    }
}

void print_finish_time_for_each_process(vector<pcb> processes) {
    cout << "Finish time for each process:\n";

    for (int i = 0; i < processes_count; i++)
        cout << processes[i].name << ": " << processes[i].finish_time << endl;

    cout << endl;
}

void print_waiting_time_for_each_process(vector<pcb> processes) {
    cout << "Waiting time for each process:\n";

    for (int i = 0; i < processes_count; i++) {
        cout << processes[i].name << ": " << 
            processes[i].finish_time - processes[i].burst_time - processes[i].arrival_time << endl;
    }

    cout << endl;
}

void print_turnaround_time_for_each_process(vector<pcb> processes) {
    cout << "Turnaround time for each process:\n";

    for (int i = 0; i < processes_count; i++) {
        cout << processes[i].name << ": " << 
            processes[i].finish_time - processes[i].arrival_time << endl;
    }

    cout << endl;
}

void print_cpu_utilization(vector<pcb> processes, vector<string> timeline) {
    cout << "CPU utilization: ";
    int bursts_sum = 0;

    for (int i = 0; i < processes_count; i++) {
        bursts_sum += processes[i].burst_time;
    }

    cout << (float(bursts_sum) / timeline.size()) * 100 << "%\n\n";
}

void calculate_and_print_results(vector<pcb> processes, vector<string> timeline) {
    set_start_and_finish_time(processes, timeline);
    sort(processes.begin(), processes.end(), by_name);

    print_gantt_chart(timeline);
    print_finish_time_for_each_process(processes);
    print_waiting_time_for_each_process(processes);
    print_turnaround_time_for_each_process(processes);
    print_cpu_utilization(processes, timeline);
    cout << "*****************************************************\n";
}

int main() {
    vector<pcb> processes;
    processes_input(processes);
    sort(processes.begin(), processes.end(), by_arrival_time);

    vector<string> fcfs_timeline = get_first_come_first_served_timeline(processes);
    calculate_and_print_results(processes, fcfs_timeline);
    vector<string> rr_timeline = get_round_robin_timeline(processes);
    calculate_and_print_results(processes, rr_timeline);
    vector<string> srt_timeline = get_shortest_remaining_time_timeline(processes);
    calculate_and_print_results(processes, srt_timeline);
}