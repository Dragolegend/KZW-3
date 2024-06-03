#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <limits.h>

using namespace std;

struct Task {
    int n;
    std::vector<int> times;
    int totalJobTime = 0;
};

struct Times {
    int executiontime;
    int timeFromStart;
    int timeFromEnd;
};

struct DataSet {
    int n;
    std::vector<Task> jobs;
};

class Resolver {
public:
    DataSet data;
    std::vector<std::string> files;

    std::vector<std::string> custom_split(std::string str, char separator) {
        std::vector<std::string> strings;
        int startIndex = 0, endIndex = 0;
        for (int i = 0; i <= str.size(); i++) {
            if (str[i] == separator || i == str.size()) {
                endIndex = i;
                std::string temp;
                temp.append(str, startIndex, endIndex - startIndex);
                strings.push_back(temp);
                startIndex = endIndex + 1;
            }
        }
        return strings;
    }

    void load_data(std::string path) {
        std::ifstream inputFile(path);

        DataSet d1;
        int counter = 1;
        if (inputFile.is_open()) {
            std::string line;
            bool first = true;
            while (std::getline(inputFile, line)) {
                std::vector<std::string> parts = custom_split(line, ' ');

                if (first) {
                    d1.n = std::stoi(parts.at(1));
                    first = false;
                } else {
                    Task tmp;
                    for (std::string p: parts) {
                        tmp.times.push_back(std::stoi(p));
                    }
                    tmp.n = counter;
                    counter++;
                    d1.jobs.push_back(tmp);
                }
            }
            inputFile.close();
        }

        data = d1;
    }

    void print_tasks(DataSet data) {
        std::cout << "\n\n\n";
        std::cout << data.n << '\n';
        for (Task t: data.jobs) {
            for (int i: t.times) {
                std::cout << i << " ";
            }
            std::cout << '\n';
        }
        std::cout << "\n\n\n";
    }

    int cmax(std::vector<Task> tasks, int machine) {
        int amount_tasks = tasks.size();
        int amount_machine = machine;
        std::vector<int> time_end(amount_machine, 0);
        for (int i = 0; i < amount_tasks; ++i) {
            for (int j = 0; j < amount_machine; ++j) {
                if (j == 0) {
                    time_end[j] += tasks[i].times[j];
                } else {
                    time_end[j] = std::max(time_end[j], time_end[j - 1]) + tasks[i].times[j];
                }
            }
        }
        return time_end[amount_machine - 1];
    }

    void sorting_data() {
        for (auto &task: data.jobs) {
            task.totalJobTime = std::accumulate(task.times.begin(), task.times.end(), 0);
        }
        std::sort(data.jobs.begin(), data.jobs.end(), [](const Task &task1, const Task &task2) {
            if (task1.totalJobTime != task2.totalJobTime) {
                return task1.totalJobTime > task2.totalJobTime;
            } else {
                return task1.n < task2.n;
            }
        });
    }

    void updateGraphExecutionTimes(std::vector<std::vector<Times>> &graph, const DataSet &data, int i, int m) {
        for (int j = 0; j < i + 1; ++j) {
            for (int machine = 0; machine < m; ++machine) {
                graph[j][machine].executiontime = data.jobs[j].times[machine];
            }
        }
    }

    void calculateTimeFromStart(std::vector<std::vector<Times>> &graph, const DataSet &data, int i, int m) {
        graph[0][0].timeFromStart = data.jobs[0].times[0];
        for (int j = 0; j < i + 1; ++j) {
            for (int machine = 0; machine < m; ++machine) {
                if (machine == 0 && j != 0) {
                    graph[j][machine].timeFromStart = data.jobs[j].times[machine] + graph[j - 1][machine].timeFromStart;
                } else if (j == 0 && machine != 0) {
                    graph[j][machine].timeFromStart = data.jobs[j].times[machine] + graph[j][machine - 1].timeFromStart;
                } else if (j != 0 && machine != 0) {
                    graph[j][machine].timeFromStart = data.jobs[j].times[machine] +
                                                      std::max(graph[j][machine - 1].timeFromStart,
                                                               graph[j - 1][machine].timeFromStart);
                }
            }
        }
    }

    void calculateTimeFromEnd(std::vector<std::vector<Times>> &graph, const DataSet &data, int i, int m) {
        graph[i][m - 1].timeFromEnd = data.jobs[i].times[m - 1];
        for (int j = i; j >= 0; --j) {
            for (int machine = m - 1; machine >= 0; --machine) {
                if (machine == m - 1 && j != i) {
                    graph[j][machine].timeFromEnd = data.jobs[j].times[machine] + graph[j + 1][machine].timeFromEnd;
                } else if (j == i && machine != m - 1) {
                    graph[j][machine].timeFromEnd = data.jobs[j].times[machine] + graph[j][machine + 1].timeFromEnd;
                } else if (j != i && machine != m - 1) {
                    graph[j][machine].timeFromEnd = data.jobs[j].times[machine] +
                                                    std::max(graph[j][machine + 1].timeFromEnd,
                                                             graph[j + 1][machine].timeFromEnd);
                }
            }
        }
    }

    int calculateTime(const DataSet &data, const std::vector<std::vector<Times>> &graph,
                      int place, int node, int i, int m, int &memory) {
        int time = 0;
        if (place == 0) {
            for (int u = 0; u <= node; ++u) {
                time += data.jobs[i + 1].times[u];
            }
            time += graph[place][node].timeFromEnd;
        } else if (place == i + 1) {
            for (int u = 0; u <= node; ++u) {
                time += data.jobs[i + 1].times[m - 1 - u];
            }
            time += graph[place - 1][m - 1 - node].timeFromStart;
        } else {
            time = std::max(memory, graph[place - 1][node].timeFromStart) +
                   data.jobs[i + 1].times[node] +
                   graph[place][node].timeFromEnd;
            memory = std::max(memory, graph[place - 1][node].timeFromStart) +
                     data.jobs[i + 1].times[node];
        }
        return time;
    }

    std::pair<int, int> findBestPlace(const std::vector<std::vector<Times>> &graph,
                                      const DataSet &data, int i, int m) {
        int the_best_node = -1;
        int the_best_place = 0;
        int the_best_time = INT_MAX;
        int time;
        int memory = 0;
        for (int place = 0; place < i + 2; ++place) {
            for (int node = 0; node < m; ++node) {
                time = calculateTime(data, graph, place, node, i, m, memory);

                if (time > the_best_node) {
                    the_best_node = time;
                }
            }
            memory = 0;
            if (the_best_node < the_best_time) {
                the_best_time = the_best_node;
                the_best_place = place;
            }

            the_best_node = -1;
        }

        return {the_best_place, the_best_time};
    }


    std::vector<Task> quickneh() {
        sorting_data();
        int n = data.jobs.size();
        int m = data.jobs[0].times.size();
        std::vector<std::vector<Times>> graph(n, std::vector<Times>(m));
        for (int i = 0; i < n - 1; ++i) {
            updateGraphExecutionTimes(graph, data, i, m);
            calculateTimeFromStart(graph, data, i, m);
            calculateTimeFromEnd(graph, data, i, m);
            auto[the_best_place, the_best_time] = findBestPlace(graph, data, i, m);
            std::rotate(data.jobs.begin() + the_best_place, data.jobs.begin() + i + 1, data.jobs.begin() + i + 2);
        }
        return data.jobs;
    }


    std::vector<Task> resolve() {
        std::vector<Task> tmp;
        tmp.push_back(data.jobs.at(0));
        std::vector<Task> best_order = tmp;

        for (int i = 1; i < data.jobs.size(); ++i) {
            tmp = best_order;
            best_order.push_back(data.jobs.at(i));
            int best_cmax = cmax(best_order, data.n);

            for (int j = 0; j < tmp.size() + 1; ++j) {
                std::vector<Task> tmp2 = tmp;
                tmp2.insert(tmp2.begin() + j, data.jobs.at(i));
                if (cmax(tmp2, data.n) < best_cmax) {
                    best_order = tmp2;
                    best_cmax = cmax(tmp2, data.n);
                }
            }
        }
        return best_order;
    }
};

void print_order(std::vector<Task> tasks) {
    for (const auto &item: tasks) {
        std::cout << item.n << " ";
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    Resolver r1;
    for (int i = 0; i <= 120; ++i) {
        std::string path = "data" + std::to_string(i) + ".txt";
        std::cout << path;
        r1.load_data(path);
//        r1.print_tasks(r1.data);
        std::cout << "Order in " + path << " ";
        std::vector<Task> order = r1.quickneh();
        print_order(order);
        std::cout << std::endl << "Cmax = " << r1.cmax(order, r1.data.n) << std::endl;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Execution time(microseconds): " << duration.count() << "\n";

    return 0;
}
