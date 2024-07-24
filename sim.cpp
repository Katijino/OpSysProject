#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

// Process class to hold process information
class Process {
public:
    string id;
    int arrival_time;
    vector<int> cpu_bursts;
    vector<int> io_bursts;
    bool is_cpu_bound;

    Process(string id, int arrival_time, bool is_cpu_bound) : id(id), arrival_time(arrival_time), is_cpu_bound(is_cpu_bound) {}
};

// Function to generate exponential random numbers
double next_exp(double lambda) {
    return -log(drand48()) / lambda;
}

// Function to generate process IDs
string generate_process_id(int index) {
    char letter = 'A' + (index / 10);
    int number = index % 10;
    return string(1, letter) + to_string(number);
}

// Function to generate processes
void generate_processes(int n, int ncpu, double lambda, int bound, vector<Process>& processes) {
    for (int i = 0; i < n; ++i) {
        string id = generate_process_id(i);
        int arrival_time = static_cast<int>(floor(next_exp(lambda)));
        bool is_cpu_bound = (i < ncpu);

        Process process(id, arrival_time, is_cpu_bound);

        int num_bursts = static_cast<int>(ceil(drand48() * 32));
        for (int j = 0; j < num_bursts; ++j) {
            int cpu_burst = static_cast<int>(ceil(next_exp(lambda)));
            while (cpu_burst >bound){
                cpu_burst = static_cast<int>(ceil(next_exp(lambda)));
            }
            int io_burst = static_cast<int>(ceil(next_exp(lambda)));
            while (io_burst > bound){
                io_burst = static_cast<int>(ceil(next_exp(lambda)));


            }
            if (is_cpu_bound) {
                cpu_burst = cpu_burst*4;
            } else {
                io_burst = io_burst*8;
            }

            process.cpu_bursts.push_back(cpu_burst);
            if (j < num_bursts - 1) {
                process.io_bursts.push_back(io_burst);
            }
        }

        processes.push_back(process);
    }
}

// Function to print processes to stdout
void print_processes(const vector<Process>& processes, int seed, double lambda, int bound) {
    cout << "<<< PROJECT PART I\n";
    cout << "<<< -- process set (n=" << processes.size() << ") with " << count_if(processes.begin(), processes.end(), [](const Process& p) { return p.is_cpu_bound; }) << " CPU-bound process\n";
    cout << "<<< -- seed=" << seed << "; lambda=" << fixed << setprecision(6) << lambda << "; bound=" << bound << endl;
    for (const auto& process : processes) {
        cout << (process.is_cpu_bound ? "CPU-bound" : "I/O-bound") << " process " << process.id << ": arrival time " << process.arrival_time << "ms; " << process.cpu_bursts.size() << " CPU bursts:\n";
        for (size_t i = 0; i < process.cpu_bursts.size(); ++i) {
            cout << "==> CPU burst " << process.cpu_bursts[i] << "ms";
            if (i < process.io_bursts.size()) {
                cout << " ==> I/O burst " << process.io_bursts[i] << "ms";
            }
            cout << endl;
        }
    }
}

// Function to calculate and write statistics to simout.txt
void write_statistics(const vector<Process>& processes) {
    ofstream outfile("simout.txt");

    int num_processes = processes.size();
    int num_cpu_bound = count_if(processes.begin(), processes.end(), [](const Process& p) { return p.is_cpu_bound; });
    int num_io_bound = num_processes - num_cpu_bound;

    double total_cpu_burst_cpu_bound = 0;
    double total_cpu_burst_io_bound = 0;
    double total_io_burst_cpu_bound = 0;
    double total_io_burst_io_bound = 0;

    int cpu_burst_count_cpu_bound = 0;
    int cpu_burst_count_io_bound = 0;
    int io_burst_count_cpu_bound = 0;
    int io_burst_count_io_bound = 0;

    for (const auto& process : processes) {
        if (process.is_cpu_bound) {
            for (const auto& burst : process.cpu_bursts) {
                total_cpu_burst_cpu_bound += burst;
                cpu_burst_count_cpu_bound++;
            }
            for (const auto& burst : process.io_bursts) {
                total_io_burst_cpu_bound += burst;
                io_burst_count_cpu_bound++;
            }
        } else {
            for (const auto& burst : process.cpu_bursts) {
                total_cpu_burst_io_bound += burst;
                cpu_burst_count_io_bound++;
            }
            for (const auto& burst : process.io_bursts) {
                total_io_burst_io_bound += burst;
                io_burst_count_io_bound++;
            }
        }
    }

    double avg_cpu_burst_cpu_bound = cpu_burst_count_cpu_bound ? total_cpu_burst_cpu_bound / cpu_burst_count_cpu_bound : 0;
    double avg_cpu_burst_io_bound = cpu_burst_count_io_bound ? total_cpu_burst_io_bound / cpu_burst_count_io_bound : 0;
    double avg_io_burst_cpu_bound = io_burst_count_cpu_bound ? total_io_burst_cpu_bound / io_burst_count_cpu_bound : 0;
    double avg_io_burst_io_bound = io_burst_count_io_bound ? total_io_burst_io_bound / io_burst_count_io_bound : 0;

    double overall_avg_cpu_burst = (total_cpu_burst_cpu_bound + total_cpu_burst_io_bound) / (cpu_burst_count_cpu_bound + cpu_burst_count_io_bound);
    double overall_avg_io_burst = (total_io_burst_cpu_bound + total_io_burst_io_bound) / (io_burst_count_cpu_bound + io_burst_count_io_bound);

    outfile << "-- number of processes: " << num_processes << endl;
    outfile << "-- number of CPU-bound processes: " << num_cpu_bound << endl;
    outfile << "-- number of I/O-bound processes: " << num_io_bound << endl;
    outfile << fixed << setprecision(3);
    outfile << "-- CPU-bound average CPU burst time: " << avg_cpu_burst_cpu_bound << " ms" << endl;
    outfile << "-- I/O-bound average CPU burst time: " << avg_cpu_burst_io_bound << " ms" << endl;
    outfile << "-- overall average CPU burst time: " << overall_avg_cpu_burst << " ms" << endl;
    outfile << "-- CPU-bound average I/O burst time: " << avg_io_burst_cpu_bound << " ms" << endl;
    outfile << "-- I/O-bound average I/O burst time: " << avg_io_burst_io_bound << " ms" << endl;
    outfile << "-- overall average I/O burst time: " << overall_avg_io_burst << " ms" << endl;

    outfile.close();
}

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    if (argc != 6) {
        cerr << "ERROR: Invalid number of arguments.\n";
        return 1;
    }

    // Extract simulation parameters from argv
    int n = atoi(argv[1]);      // Number of processes
    int ncpu = atoi(argv[2]);   // Number of CPU-bound processes
    int seed = atoi(argv[3]);   // Seed for random number generator
    double lambda = atof(argv[4]);  // Lambda for exponential distribution
    int bound = atoi(argv[5]);  // Upper bound for random numbers

    if (n <= 0 || ncpu < 0 || ncpu > n || lambda <= 0 || bound <= 0) {
        cerr << "ERROR: Invalid argument values.\n";
        return 1;
    }

    // Initialize random number generator
    srand48(seed);

    // Generate processes based on specified parameters
    vector<Process> processes;
    generate_processes(n, ncpu, lambda, bound, processes);

    // Print process details to stdout
    print_processes(processes, seed, lambda, bound);

    // Generate statistics and write to simout.txt
    write_statistics(processes);

    return 0;
}
