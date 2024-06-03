#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
 #include <numeric>

using namespace std;

struct Task {
    int n;
    std::vector<int> times;
};

struct Data {
    int n;
    std::vector<Task> tasks;
};


class Resolver {
public:
    Data data;

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

        Data d1;
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
                    d1.tasks.push_back(tmp);
                }
            }

            inputFile.close();
        }

        data = d1;
    }

    void print_tasks(Data data) {
        std::cout << "\n\n\n";

        std::cout << data.n << '\n';
        for (Task t: data.tasks) {
//            std::cout << "N task: "<< t.n << " ";
            for (int i: t.times) {
                std::cout << i << " ";
            }

            std::cout << '\n';
        }

        std::cout << "\n\n\n";
    }

    void sort(){
        std::sort( data.tasks.begin( ), data.tasks.end( ), [ ]( const auto& lhs, const auto& rhs )
        {
            return  std::accumulate(lhs.times.begin(), lhs.times.end(), 0) > std::accumulate(rhs.times.begin(), rhs.times.end(), 0);
        });
    }

    
    std::vector <Task> resolve(){
        sort();
        std::vector <Task> tmp;

        tmp.push_back(data.tasks.at(0));

        std::vector <Task> bets_order = tmp;

        for(int i = 1; i < data.tasks.size(); ++i){
            tmp = bets_order;

            bets_order.push_back(data.tasks.at(i));
            int best_cmax = cmax(bets_order, data.n);

            for(int j = 0; j < tmp.size() + 1; ++j){
                std::vector <Task> tmp2 = tmp;
                tmp2.insert(tmp2.begin() + j, data.tasks.at(i));
                if(cmax(tmp2, data.n) < best_cmax){
                    bets_order = tmp2;
                    best_cmax = cmax(tmp2, data.n);
                }
            }
        }
        return bets_order;
    }

    int cmax(std::vector<Task> tasks, int machine) {
        int amount_tasks = tasks.size();
        int amount_machine = machine;
        std::vector<int> time_end(amount_machine, 0);

        for (int i = 0; i < amount_tasks; ++i) {
            for (int j = 0; j < amount_machine; ++j) {
                if (j == 0) {
                    // Jeśli zadanie jest pierwszym na maszynie, czas zakończenia na tej maszynie to czas zakończenia poprzedniego zadania na tej maszynie plus czas przetwarzania tego zadania
                    time_end[j] += tasks[i].times[j];
                } else {
                    // Jeśli zadanie nie jest pierwszym na maszynie, czas zakończenia na tej maszynie to maksimum pomiędzy czasem zakończenia poprzedniego zadania na tej maszynie i czasem zakończenia poprzedniego zadania na poprzedniej maszynie plus czasem przetwarzania tego zadania
                    time_end[j] = std::max(time_end[j], time_end[j - 1]) + tasks[i].times[j];
                }
            }
        }
        return time_end[amount_machine - 1];
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
        std::string path = "data" + std::to_string(i);
        path += ".txt";
        std::cout << path;
        r1.load_data(path);
        r1.print_tasks(r1.data);
        std::cout << "Order in " + path << " ";
        std::vector<Task> order = r1.resolve();
        print_order(order);
        

        //int wynik = r1.cmax(data.tas);
        std::cout << std::endl << "Cmax = " << r1.cmax(order, r1.data.n) << std::endl;
    }


    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Execution time(microseconds): " << duration.count() << "\n";


    return 0;
}
