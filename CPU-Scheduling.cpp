#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;

string TraceOrStats;
int last_instant;

struct Process {
    char name;
    int arrivaltime;
    int serviceTime;
    int priority;
    int RemainingTime;
    int Completiontime;
    int turnaround_time;
    int waitingfor;
    int startfrom;
};

vector<Process> Readinput(vector<int>& typeofalgorithm);
void SPN(vector<Process>& processes);
void HRRN(vector<Process>& processes);
void FB_1(vector<Process>& processes);
void FB_2i(vector<Process>& processes);
void FCFS(vector<Process>& processes);
void RR(vector<Process>& processes, int quantum);
void SRT(vector<Process>& processes);
void Aging(vector<Process>& processes, int quantum);
void print_stats(const vector<Process>& processes, const string& algorithm_name);
int main() {
    cin >> TraceOrStats;

    vector<int> typeofalgorithm;
    vector<Process> processes = Readinput(typeofalgorithm);
    vector<Process> original_processes = processes;  // copy ll original process

    for (size_t i = 0; i < typeofalgorithm.size(); i++) {
        // cleanup ll new algorithm
        processes = original_processes;
        
        switch(typeofalgorithm[i]) {
            case 3:
                SPN(processes);
                break;
            case 5:
                HRRN(processes);
                break;    
            case 6:
                FB_1(processes);
                break;
            case 7:
                FB_2i(processes);
                break;
            case 1:
                FCFS(processes);
                break;
            case 2:
                i++;  // 3alshan akhod el quantum ba3d el ,
                RR(processes, typeofalgorithm[i]);
                break;
            case 4:
                SRT(processes);
                break;
            case 8:
                i++;  
                Aging(processes, typeofalgorithm[i]);
                break;     
        }
    }

    return 0;
}


// 3alshan a2ra el input file
vector<Process> Readinput(vector<int>& typeofalgorithm) {
    vector<Process> processes;
    string algorithm_line;
    
    cin >> algorithm_line;

    // b split el ,
    stringstream ss(algorithm_line);
    string algorithm;
    while (getline(ss, algorithm, ',')) {
        if (algorithm[0] == '2' || algorithm[0] == '8') {  // RR or Aging algorithm
            size_t dash_pos = algorithm.find('-');
            if (dash_pos != string::npos) {
                typeofalgorithm.push_back(algorithm[0] - '0');  
                typeofalgorithm.push_back(stoi(algorithm.substr(dash_pos + 1)));  // ba2ra el q
            } else {
                typeofalgorithm.push_back(stoi(algorithm));  
            }
        } else {
            typeofalgorithm.push_back(stoi(algorithm));  
        }
    }
    
    cin >> last_instant;

    int num_processes;
    cin >> num_processes;

    // ba2ra lines el process(name,arrival,service)
    for (int i = 0; i < num_processes; ++i) {
        string line;
        cin >> line;
        
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        
        Process p;
        p.name = line[0];
        p.arrivaltime = stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
        
        
        if (typeofalgorithm[0] == 8) {  // btcheck law aging algorithm
            p.priority = stoi(line.substr(pos2 + 1));  // sa3tha el priority mesh service time
            p.serviceTime = 1;  // no service fel aging
        } else {
            p.serviceTime = stoi(line.substr(pos2 + 1));  // yb2a service lba2y el algo
            p.priority = 0;  // w yb2a kolo have the same intial priority b zero
        }
        
        p.RemainingTime = p.serviceTime;
        p.startfrom = -1;
        
        processes.push_back(p);
    }

    return processes;
}

void print_stats(const vector<Process>& processes, const string& algorithm_name) {
    float mean_turnaround = 0, mean_normalized = 0;

    cout << algorithm_name << "\n";

    cout << "Process    |";
    for (const auto& p : processes)
        cout << setw(3) << p.name << "  |";
    cout << "\n";

    cout << "Arrival    |";
    for (const auto& p : processes)
        cout << setw(3) << p.arrivaltime << "  |";
    cout << "\n";

    cout << "Service    |";
    for (const auto& p : processes)
        cout << setw(3) << p.serviceTime << "  |";
    cout << " Mean|\n";

    cout << "Finish     |";
    for (const auto& p : processes)
        cout << setw(3) << p.Completiontime << "  |";
    cout << "-----|\n";

    cout << "Turnaround |";
    for (const auto& p : processes) {
        cout << setw(3) << p.turnaround_time << "  |";
        mean_turnaround += p.turnaround_time;
    }
    mean_turnaround /= processes.size();
    cout << fixed << setprecision(2) << setw(5) << mean_turnaround << "|\n";

    cout << "NormTurn   |";
    for (const auto& p : processes) {
        float norm = (float)p.turnaround_time / p.serviceTime;
        cout << fixed << setprecision(2) << setw(5) << norm << "|";
        mean_normalized += norm;
    }
    mean_normalized /= processes.size();
    cout << fixed << setprecision(2) << setw(5) << mean_normalized << "|\n";

    cout << "\n";
}

void SPN(vector<Process>& processes) {
    vector<Process*> ready_queue;
    int current_time = 0;
    size_t completed = 0;
    
    
    vector<vector<char>> timeline(processes.size(), vector<char>(last_instant, ' '));
    
    
    while (completed < processes.size()) {
        // b add el arrived processes ll ready queue
        for (auto& process : processes) {
            if (process.arrivaltime <= current_time && process.RemainingTime > 0 &&
                find(ready_queue.begin(), ready_queue.end(), &process) == ready_queue.end()) {
                ready_queue.push_back(&process);
            }
        }

        if (!ready_queue.empty()) {
            // bageeb shortest process
            auto shortest = min_element(ready_queue.begin(), ready_queue.end(),
                [](Process* a, Process* b) { 
                    return a->serviceTime < b->serviceTime; 
                });
            
            Process* current = *shortest;
            ready_queue.erase(shortest);

            // babda2 el start time
            if (current->startfrom == -1) {
                current->startfrom = current_time;
            }

            current->Completiontime = current_time + current->serviceTime;
            current->turnaround_time = current->Completiontime - current->arrivaltime;
            current->waitingfor = current->turnaround_time - current->serviceTime;
            current->RemainingTime = 0;
            completed++;
            
            current_time = current->Completiontime;
        } else {
            current_time++;
        }
    }

    for (const auto& process : processes) {
        int row = &process - &processes[0];
        
        // bahot el running process b *
        for (int t = process.startfrom; t < process.Completiontime && t < last_instant; t++) {
            timeline[row][t] = '*';
        }
        
        // bahot el ready process b .
        for (int t = process.arrivaltime; t < process.startfrom && t < last_instant; t++) {
            timeline[row][t] = '.';
        }
    }

    // bgeeb el output law kn tarce aw stats
    if (TraceOrStats == "trace") {
        cout << "SPN   ";
        for (int t = 0; t <= last_instant; t++) cout << t % 10 << " ";
        cout << "\n------------------------------------------------\n";
        
        for (size_t i = 0; i < processes.size(); i++) {
            cout << processes[i].name << "     |";
            for (int t = 0; t < last_instant; t++) {
                cout << timeline[i][t] << "|";
            }
            cout <<" ";
            cout << "\n";
        }
        cout << "------------------------------------------------\n";
        cout << "\n";
    } else if (TraceOrStats == "stats") {
        print_stats(processes, "SPN");
    }
}
void HRRN(vector<Process>& processes) {
    vector<Process*> ready_queue;
    int current_time = 0;
    size_t completed = 0;
    
    vector<vector<char>> timeline(processes.size(), vector<char>(last_instant, ' '));
    
    while (completed < processes.size()) {
        for (auto& process : processes) {
            if (process.arrivaltime <= current_time && process.RemainingTime > 0 &&
                find(ready_queue.begin(), ready_queue.end(), &process) == ready_queue.end()) {
                ready_queue.push_back(&process);
            }
        }

        if (!ready_queue.empty()) {
            // bageeb el process el 3ndha akbar ratio
            auto highest_ratio = max_element(ready_queue.begin(), ready_queue.end(),
                [current_time](Process* a, Process* b) {
                    float ratio_a = (float)(current_time - a->arrivaltime + a->serviceTime) / a->serviceTime;
                    float ratio_b = (float)(current_time - b->arrivaltime + b->serviceTime) / b->serviceTime;
                    if (ratio_a == ratio_b) {
                        return a->arrivaltime > b->arrivaltime;
                    }
                    return ratio_a < ratio_b;
                });
            
            Process* current = *highest_ratio;
            ready_queue.erase(highest_ratio);

            if (current->startfrom == -1) {
                current->startfrom = current_time;
            }

            current->Completiontime = current_time + current->serviceTime;
            current->turnaround_time = current->Completiontime - current->arrivaltime;
            current->waitingfor = current->turnaround_time - current->serviceTime;
            current->RemainingTime = 0;
            completed++;
            
            current_time = current->Completiontime;
        } else {
            current_time++;
        }
    }

    for (const auto& process : processes) {
        int row = &process - &processes[0];
        
        for (int t = process.startfrom; t < process.Completiontime && t < last_instant; t++) {
            timeline[row][t] = '*';
        }
        
        for (int t = process.arrivaltime; t < process.startfrom && t < last_instant; t++) {
            timeline[row][t] = '.';
        }
    }

    if (TraceOrStats == "trace") {
        cout << "HRRN  ";
        for (int t = 0; t <= last_instant; t++) cout << t % 10 << " ";
        cout << "\n------------------------------------------------\n";
        
        for (size_t i = 0; i < processes.size(); i++) {
            cout << processes[i].name << "     |";
            for (int t = 0; t < last_instant; t++) {
                cout << timeline[i][t] << "|";
            }
            cout <<" ";
            cout << "\n";
        }
        cout << "------------------------------------------------\n";
        cout << "\n";
    } else if (TraceOrStats == "stats") {
        print_stats(processes, "HRRN");
    }
}
void FB_1(vector<Process>& processes) {
    const int MaxQueueLevels = 10;  // bintialize el max number of queue levels b 10
    const int QUANTUM = 1;     // hna fixed q b 1
    vector<vector<Process*>> ready_queues(MaxQueueLevels);
    vector<int> current_queue(processes.size(), 0);
    int current_time = 0;
    size_t completed = 0;
    int quantum_left = QUANTUM;
    
    vector<vector<char>> timeline(processes.size(), vector<char>(last_instant, ' '));
    
    for (auto& p : processes) {
        p.RemainingTime = p.serviceTime;
    }

    Process* current_process = nullptr;

    while (completed < processes.size()) {
        // bahot el new process fel RQ0
        for (auto& process : processes) {
            if (process.arrivaltime == current_time && process.RemainingTime > 0) {
                bool already_in_queue = false;
                for (const auto& queue : ready_queues) {
                    if (find(queue.begin(), queue.end(), &process) != queue.end()) {
                        already_in_queue = true;
                        break;
                    }
                }
                if (!already_in_queue && &process != current_process) {
                    ready_queues[0].push_back(&process);
                }
            }
        }

        // law mfish active process aw law el q khelset bkhtar mn el awel
        if (current_process == nullptr || quantum_left == 0) {
            if (current_process != nullptr && current_process->RemainingTime > 0) {
                // ba3dd el other processes fy el system
                int others = 0;
                for (const auto& p : processes) {
                    if (&p != current_process && p.arrivaltime <= current_time && p.RemainingTime > 0) {
                        others++;
                    }
                }

                // law fy others bhothom fy el next queue
                if (others > 0) {
                    current_queue[current_process - &processes[0]]++;
                }
                ready_queues[current_queue[current_process - &processes[0]]].push_back(current_process);
            }

            // bfind el highest priority fy el non-empty queue
            int active_queue = 0;
            while (active_queue < MaxQueueLevels && ready_queues[active_queue].empty()) {
                active_queue++;
            }

            if (active_queue < MaxQueueLevels) {
                current_process = ready_queues[active_queue].front();
                ready_queues[active_queue].erase(ready_queues[active_queue].begin());
                quantum_left = QUANTUM;
            } else {
                current_process = nullptr;
            }
        }

        if (current_process != nullptr) {

            timeline[current_process - &processes[0]][current_time] = '*';
            current_process->RemainingTime--;
            quantum_left--;

            
            for (int q = 0; q < MaxQueueLevels; q++) {
                for (const auto* p : ready_queues[q]) {
                    timeline[p - &processes[0]][current_time] = '.';
                }
            }

            // b check law process khalast
            if (current_process->RemainingTime == 0) {
                current_process->Completiontime = current_time + 1;
                current_process->turnaround_time = current_process->Completiontime - current_process->arrivaltime;
                current_process->waitingfor = current_process->turnaround_time - current_process->serviceTime;
                completed++;
                current_process = nullptr;
            }
        }

        current_time++;
    }

    if (TraceOrStats == "trace") {
        cout << "FB-1  ";
        for (int t = 0; t <= last_instant; t++) cout << t % 10 << " ";
        cout << "\n------------------------------------------------\n";
        
        for (size_t i = 0; i < processes.size(); i++) {
            cout << processes[i].name << "     |";
            for (int t = 0; t < last_instant; t++) {
                cout << timeline[i][t] << "|";
            }
            cout <<" ";
            cout << "\n";
        }
        cout << "------------------------------------------------\n";
        cout << "\n";
    }  else if (TraceOrStats == "stats") {
        print_stats(processes, "FB-1");
    }
}
void FB_2i(vector<Process>& processes) {
    const int MaxQueueLevels = 10;
    vector<vector<Process*>> ready_queues(MaxQueueLevels);
    vector<int> current_queue(processes.size(), 0);
    int current_time = 0;
    size_t completed = 0;
    
    vector<vector<char>> timeline(processes.size(), vector<char>(last_instant, ' '));
    
    for (auto& p : processes) {
        p.RemainingTime = p.serviceTime;
    }

    Process* current_process = nullptr;
    int quantum_left = 0;

    while (completed < processes.size()) {
       
        for (auto& process : processes) {
            if (process.arrivaltime == current_time && process.RemainingTime > 0) {
                bool already_in_queue = false;
                for (const auto& queue : ready_queues) {
                    if (find(queue.begin(), queue.end(), &process) != queue.end()) {
                        already_in_queue = true;
                        break;
                    }
                }
                if (!already_in_queue && &process != current_process) {
                    ready_queues[0].push_back(&process);
                }
            }
        }

    
        if (current_process == nullptr || quantum_left == 0) {
            if (current_process != nullptr && current_process->RemainingTime > 0) {
                int others = 0;
                for (const auto& p : processes) {
                    if (&p != current_process && p.arrivaltime <= current_time && p.RemainingTime > 0) {
                        others++;
                    }
                }

                if (others > 0) {
                    current_queue[current_process - &processes[0]]++;
                }
                ready_queues[current_queue[current_process - &processes[0]]].push_back(current_process);
            }

            // bageeb process with the highest priority
            int active_queue = 0;
            while (active_queue < MaxQueueLevels && ready_queues[active_queue].empty()) {
                active_queue++;
            }

            if (active_queue < MaxQueueLevels) {
                current_process = ready_queues[active_queue].front();
                ready_queues[active_queue].erase(ready_queues[active_queue].begin());
                // hena el q mesh fixed lkol el queues bas fixed the queue nafso w byb2a 2 power i ,where i is the queue level starting from 0
                quantum_left = 1 << active_queue; 
            } else {
                current_process = nullptr;
            }
        }

        if (current_process != nullptr) {
            timeline[current_process - &processes[0]][current_time] = '*';
            current_process->RemainingTime--;
            quantum_left--;

            for (int q = 0; q < MaxQueueLevels; q++) {
                for (const auto* p : ready_queues[q]) {
                    timeline[p - &processes[0]][current_time] = '.';
                }
            }

            if (current_process->RemainingTime == 0) {
                current_process->Completiontime = current_time + 1;
                current_process->turnaround_time = current_process->Completiontime - current_process->arrivaltime;
                current_process->waitingfor = current_process->turnaround_time - current_process->serviceTime;
                completed++;
                current_process = nullptr;
            }
        }

        current_time++;
    }

    //bakhtar shakl el output
    if (TraceOrStats == "trace") {
        cout << "FB-2i ";
        for (int t = 0; t <= last_instant; t++) cout << t % 10 << " ";
        cout << "\n------------------------------------------------\n";
        
        for (size_t i = 0; i < processes.size(); i++) {
            cout << processes[i].name << "     |";
            for (int t = 0; t < last_instant; t++) {
                cout << timeline[i][t] << "|";
            }
            cout <<" ";
            cout << "\n";
        }
        cout << "------------------------------------------------\n";
        cout << "\n";
    }  else if (TraceOrStats == "stats") {
        print_stats(processes, "FB-2i");
    }
}
void FCFS(vector<Process>& processes) {
    vector<Process*> ready_queue;
    int current_time = 0;
    size_t completed = 0;
    
    vector<vector<char>> timeline(processes.size(), vector<char>(last_instant, ' '));
    
    for (auto& p : processes) {
        p.RemainingTime = p.serviceTime;
    }

    while (completed < processes.size()) {
        //bahot el new arrived process fel ready queue
        for (auto& process : processes) {
            if (process.arrivaltime == current_time && process.RemainingTime > 0) {
                ready_queue.push_back(&process);
            }
        }

        if (!ready_queue.empty()) {
            Process* current = ready_queue.front();
            
            //hna mfish preemption fa once bada2t lazm akhlas
            timeline[current - &processes[0]][current_time] = '*';
            
            for (size_t i = 1; i < ready_queue.size(); i++) {
                timeline[ready_queue[i] - &processes[0]][current_time] = '.';
            }

            current->RemainingTime--;
            
            if (current->RemainingTime == 0) {
                //hna el process khalast
                current->Completiontime = current_time + 1;
                current->turnaround_time = current->Completiontime - current->arrivaltime;
                current->waitingfor = current->turnaround_time - current->serviceTime;
                ready_queue.erase(ready_queue.begin());
                completed++;
            }
        }
        
        current_time++;
    }

    if (TraceOrStats == "trace") {
        cout << "FCFS  ";
        for (int t = 0; t <= last_instant; t++) cout << t % 10 << " ";
        cout << "\n------------------------------------------------\n";
        
        for (size_t i = 0; i < processes.size(); i++) {
            cout << processes[i].name << "     |";
            for (int t = 0; t < last_instant; t++) {
                cout << timeline[i][t] << "|";
            }
            cout <<" ";
            cout << "\n";
        }
        cout << "------------------------------------------------\n";
        cout << "\n";
    }  else if (TraceOrStats == "stats") {
        print_stats(processes, "FCFS");
    }
}
void RR(vector<Process>& processes, int quantum) {
    vector<Process*> ready_queue;
    int current_time = 0;
    size_t completed = 0;
    
    vector<vector<char>> timeline(processes.size(), vector<char>(last_instant, ' '));
    
    for (auto& p : processes) {
        p.RemainingTime = p.serviceTime;
    }

    Process* current_process = nullptr;
    int quantum_left = 0;

    while (completed < processes.size()) {
        for (auto& process : processes) {
            if (process.arrivaltime == current_time && process.RemainingTime > 0) {
                bool already_in_queue = false;
                for (const auto* p : ready_queue) {
                    if (p == &process) {
                        already_in_queue = true;
                        break;
                    }
                }
                if (!already_in_queue && &process != current_process) {
                    ready_queue.push_back(&process);
                }
            }
        }

        //hna fy preemption fa law mfish active process aw quantum expired bkhtar new process
        if (current_process == nullptr || quantum_left == 0) {
            if (current_process != nullptr && current_process->RemainingTime > 0) {
                ready_queue.push_back(current_process);
            }

            if (!ready_queue.empty()) {
                current_process = ready_queue.front();
                ready_queue.erase(ready_queue.begin());
                quantum_left = quantum;
            } else {
                current_process = nullptr;
            }
        }

        if (current_process != nullptr) {
            timeline[current_process - &processes[0]][current_time] = '*';
            current_process->RemainingTime--;
            quantum_left--;

            for (const auto* p : ready_queue) {
                timeline[p - &processes[0]][current_time] = '.';
            }

            if (current_process->RemainingTime == 0) {
                current_process->Completiontime = current_time + 1;
                current_process->turnaround_time = current_process->Completiontime - current_process->arrivaltime;
                current_process->waitingfor = current_process->turnaround_time - current_process->serviceTime;
                completed++;
                current_process = nullptr;
            }
        }

        current_time++;
    }

    if (TraceOrStats == "trace") {
        cout << "RR-" << quantum << "  ";
        for (int t = 0; t <= last_instant; t++) cout << t % 10 << " ";
        cout << "\n------------------------------------------------\n";
        
        for (size_t i = 0; i < processes.size(); i++) {
            cout << processes[i].name << "     |";
            for (int t = 0; t < last_instant; t++) {
                cout << timeline[i][t] << "|";
            }
            cout <<" ";
            cout << "\n";
        }
        cout << "------------------------------------------------\n";
        cout << "\n";
    }  else if (TraceOrStats == "stats") {
        print_stats(processes, "RR-"+ to_string(quantum));
    }
}
void SRT(vector<Process>& processes) {
    vector<Process*> ready_queue;
    int current_time = 0;
    size_t completed = 0;
    
    vector<vector<char>> timeline(processes.size(), vector<char>(last_instant, ' '));
    
    for (auto& p : processes) {
        p.RemainingTime = p.serviceTime;
    }

    Process* current_process = nullptr;

    while (completed < processes.size()) {
        for (auto& process : processes) {
            if (process.arrivaltime == current_time && process.RemainingTime > 0) {
                ready_queue.push_back(&process);
            }
        }

        //process el 3ndha shortest remaining time
        if (!ready_queue.empty()) {
            auto shortest = min_element(ready_queue.begin(), ready_queue.end(),
                [](const Process* a, const Process* b) {
                    return a->RemainingTime < b->RemainingTime;
                });
            
            current_process = *shortest;
            ready_queue.erase(shortest);

            // brun el process for one time unit
            timeline[current_process - &processes[0]][current_time] = '*';
            current_process->RemainingTime--;

            for (const auto* p : ready_queue) {
                timeline[p - &processes[0]][current_time] = '.';
            }

            if (current_process->RemainingTime == 0) {
                current_process->Completiontime = current_time + 1;
                current_process->turnaround_time = current_process->Completiontime - current_process->arrivaltime;
                current_process->waitingfor = current_process->turnaround_time - current_process->serviceTime;
                completed++;
            } else {
                ready_queue.push_back(current_process);
            }
        }
        
        current_time++;
    }

    if (TraceOrStats == "trace") {
        cout << "SRT   ";
        for (int t = 0; t <= last_instant; t++) cout << t % 10 << " ";
        cout << "\n------------------------------------------------\n";
        
        for (size_t i = 0; i < processes.size(); i++) {
            cout << processes[i].name << "     |";
            for (int t = 0; t < last_instant; t++) {
                cout << timeline[i][t] << "|";
            }
            cout <<" ";
            cout << "\n";
        }
        cout << "------------------------------------------------\n";
        cout << "\n";
    } else if (TraceOrStats == "stats") {
        print_stats(processes, "SRT");
    }
}
void Aging(vector<Process>& processes, int quantum) {
    vector<Process*> ready_queue;
    int current_time = 0;
    
    vector<vector<char>> timeline(processes.size(), vector<char>(last_instant, ' '));
    
    // hna bstore el initial priorities w akher mara et3mlhom execution
    vector<int> initial_priorities(processes.size());
    vector<int> last_execution(processes.size(), -1);
    vector<int> WaitingSince(processes.size(), -1);  // btrack emta el process bad2t teb2a waiting
    
    for (size_t i = 0; i < processes.size(); i++) {
        initial_priorities[i] = processes[i].priority;
        processes[i].priority = initial_priorities[i];
    }

    Process* running_process = nullptr;

    while (current_time < last_instant) {
        // bhot el new process fel ready queue with priority+1 3alshan they are treated as waiting processes
        for (auto& process : processes) {
            if (process.arrivaltime == current_time) {
                process.priority = initial_priorities[&process - &processes[0]] + 1;
                ready_queue.push_back(&process);
                WaitingSince[&process - &processes[0]] = current_time;
            }
        }

        // law fy running process brg3ha ll queue bel initial priority bet3tha
        if (running_process != nullptr) {
            running_process->priority = initial_priorities[running_process - &processes[0]];
            ready_queue.push_back(running_process);
            WaitingSince[running_process - &processes[0]] = current_time;
            last_execution[running_process - &processes[0]] = current_time;
            running_process = nullptr;
        }

        // wel waiting processes b increment their priorities
        for (auto* p : ready_queue) {
            p->priority++;
        }

        if (!ready_queue.empty()) {
            //bgeeb el highest priority processes
            int max_priority = -1;
            for (const auto* p : ready_queue) {
                max_priority = max(max_priority, p->priority);
            }

            //law fy proceses 3ndohm nfs el highest priorty bkhtar el 3ndha thelongest wait time
            auto selected = ready_queue.end();
            int longest_wait = -1;

            for (auto it = ready_queue.begin(); it != ready_queue.end(); ++it) {
                if ((*it)->priority == max_priority) {
                    int wait_time;
                    if (last_execution[*it - &processes[0]] == -1) {
                        //el process el 3mrhom mt3mlhom run
                        wait_time = current_time - (*it)->arrivaltime;
                    } else {
                        wait_time = current_time - last_execution[*it - &processes[0]];
                    }

                    if (selected == ready_queue.end() || wait_time > longest_wait) {
                        selected = it;
                        longest_wait = wait_time;
                    }
                }
            }

            if (selected != ready_queue.end()) {
                running_process = *selected;
                ready_queue.erase(selected);
            }
        }

        if (running_process != nullptr) {
            timeline[running_process - &processes[0]][current_time] = '*';
        }

        //waiting processes
        for (const auto* p : ready_queue) {
            timeline[p - &processes[0]][current_time] = '.';
        }

        current_time++;
    }

    if (TraceOrStats == "trace") {
        cout << "Aging ";
        for (int t = 0; t <= last_instant; t++) cout << t % 10 << " ";
        cout << "\n------------------------------------------------\n";
        
        for (size_t i = 0; i < processes.size(); i++) {
            cout << processes[i].name << "     |";
            for (int t = 0; t < last_instant; t++) {
                cout << timeline[i][t] << "|";
            }
            cout <<" ";
            cout << "\n";
        }
        cout << "------------------------------------------------\n";
        cout << "\n";
    }
}
