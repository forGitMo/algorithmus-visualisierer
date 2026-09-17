#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <functional>
#include <sstream>
#include <map>

#include "AlgorithmInfo.h"
#include "algorithmen/BubbleSort.h"
#include "algorithmen/SelectionSort.h"
#include "algorithmen/HeapSort.h"
#include "algorithmen/InsertionSort.h"
#include "algorithmen/RadixSort.h"
#include "algorithmen/BucketSort.h"


struct AlgoEntry {
    std::string name;
    std::string complexity;
    std::function<SortResult(const std::vector<int>&)> sortFunc;
};

static const std::vector<AlgoEntry> ALGORITHMS = {
    { "Bubble Sort",    "O(N)  / O(N^2) / O(N^2)",   bubbleSort    },
    { "Selection Sort", "O(N^2)/ O(N^2) / O(N^2)",   selectionSort },
    { "Heap Sort",      "O(NlogN)/O(NlogN)/O(NlogN)", heapSort      },
    { "Insertion Sort", "O(N)  / O(N^2) / O(N^2)",   insertionSort },
    { "Radix Sort",     "O(N)  / O(N)   / O(N)",     radixSort     },
    { "Bucket Sort",    "O(N)  / O(N)   / O(N^2)",   bucketSort    },
};


enum class ArrayType { Random, Sorted, Reversed };

static std::vector<int> generateArray(std::size_t size, ArrayType type) {
    std::vector<int> arr(size);
    switch (type) {
        case ArrayType::Random: {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dis(1, static_cast<int>(size) * 10);
            for (auto& v : arr) v = dis(gen);
            break;
        }
        case ArrayType::Sorted:
            for (std::size_t i = 0; i < size; ++i) arr[i] = static_cast<int>(i + 1);
            break;
        case ArrayType::Reversed:
            for (std::size_t i = 0; i < size; ++i) arr[i] = static_cast<int>(size - i);
            break;
    }
    return arr;
}

static std::string arrayTypeName(ArrayType t) {
    switch (t) {
        case ArrayType::Random:   return "Zufaellig";
        case ArrayType::Sorted:   return "Sortiert";
        case ArrayType::Reversed: return "Umgekehrt";
    }
    return "?";
}


struct RunResult {
    std::string algoName;
    std::string complexity;
    std::size_t comparisons;
    std::size_t swaps;
    double      timeMs;
};

static void printTable(const std::vector<RunResult>& results,
                       std::size_t arraySize,
                       ArrayType   arrayType)
{
    const int W_NAME  = 18;
    const int W_COMP  = 14;
    const int W_SWAP  = 12;
    const int W_COMPL = 28;
    const int W_TIME  = 10;
    const int TOTAL   = W_NAME + W_COMP + W_SWAP + W_COMPL + W_TIME + 5; // 5 separators

    auto hline = [&](char c = '-') {
        std::cout << std::string(static_cast<std::size_t>(TOTAL), c) << '\n';
    };

    hline('=');
    std::cout << " SORTIER-VERGLEICH ERGEBNIS\n";
    std::cout << " Array-Groesse: " << arraySize
              << "  |  Typ: " << arrayTypeName(arrayType) << '\n';
    hline();

    std::cout << std::left
              << std::setw(W_NAME)  << " Algorithmus"
              << std::setw(W_COMP)  << "| Vergleiche"
              << std::setw(W_SWAP)  << "| Tausche"
              << std::setw(W_COMPL) << "| Komplexitaet (B/A/W)"
              << std::setw(W_TIME)  << "| Zeit\n";
    hline();

    for (const auto& r : results) {
        std::ostringstream timeStr;
        timeStr << std::fixed << std::setprecision(2) << r.timeMs << " ms";

        std::cout << std::left
                  << std::setw(W_NAME)  << (" " + r.algoName)
                  << std::setw(W_COMP)  << ("| " + std::to_string(r.comparisons))
                  << std::setw(W_SWAP)  << ("| " + std::to_string(r.swaps))
                  << std::setw(W_COMPL) << ("| " + r.complexity)
                  << std::setw(W_TIME)  << ("| " + timeStr.str())
                  << '\n';
    }
    hline('=');
}


static RunResult runAlgo(const AlgoEntry& entry, const std::vector<int>& data, bool verbose) {
    auto start  = std::chrono::high_resolution_clock::now();
    SortResult res = entry.sortFunc(data);
    auto end    = std::chrono::high_resolution_clock::now();
    double ms   = std::chrono::duration<double, std::milli>(end - start).count();

    if (verbose) {
        std::cout << "\n--- " << entry.name << " (Ereignis-Log) ---\n";
        std::size_t step = 0;
        for (const auto& ev : res.events) {
            ++step;
            switch (ev.type) {
                case EventType::Compare:
                    std::cout << "  [" << step << "] VERGLEICH  idx " << ev.indexA << " <-> " << ev.indexB << '\n';
                    break;
                case EventType::Swap:
                    std::cout << "  [" << step << "] TAUSCH     idx " << ev.indexA << " <-> " << ev.indexB << '\n';
                    break;
                case EventType::Overwrite:
                    std::cout << "  [" << step << "] SCHREIBEN  idx " << ev.indexA << " <- " << ev.value << '\n';
                    break;
                case EventType::MarkSorted:
                    std::cout << "  [" << step << "] SORTIERT   idx " << ev.indexA << '\n';
                    break;
                default:
                    break;
            }
        }
    }

    return { entry.name, entry.complexity, res.stats.comparisons, res.stats.swaps, ms };
}


static void printHelp(const char* prog) {
    std::cout <<
        "Verwendung: " << prog << " [OPTIONEN]\n\n"
        "  --algo <name>     Algorithmus auswaehlen (kann mehrfach angegeben werden).\n"
        "                    Moegliche Werte: bubble, selection, heap, insertion, radix, bucket, all\n"
        "  --size <n>        Array-Groesse (Standard: 100)\n"
        "  --type <typ>      Array-Typ: random | sorted | reversed (Standard: random)\n"
        "  --verbose         Jeden Schritt (Vergleich/Tausch) ausgeben\n"
        "  --help            Diese Hilfe anzeigen\n\n"
        "Ohne Argumente: interaktives Menue\n\n"
        "Beispiele:\n"
        "  " << prog << " --algo bubble --algo heap --size 200 --type random\n"
        "  " << prog << " --algo all --size 50 --type sorted\n"
        "  " << prog << " --algo all --verbose --size 20\n";
}


static int promptMenu(const std::string& prompt,
                      const std::vector<std::string>& options,
                      int defaultIdx = 0)
{
    std::cout << '\n' << prompt << '\n';
    for (int i = 0; i < static_cast<int>(options.size()); ++i) {
        std::cout << "  [" << (i + 1) << "] " << options[i];
        if (i == defaultIdx) std::cout << "  (Standard)";
        std::cout << '\n';
    }
    std::cout << "Auswahl (Enter = Standard): ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return defaultIdx;
    try {
        int choice = std::stoi(line) - 1;
        if (choice >= 0 && choice < static_cast<int>(options.size())) return choice;
    } catch (...) {}
    std::cout << "Ungueltige Eingabe, verwende Standard.\n";
    return defaultIdx;
}

static bool promptMultiAlgo(std::vector<std::size_t>& selectedIndices) {
    std::cout << "\nAlgorithmen waehlen (kommagetrennte Nummern, z.B. 1,3 oder 'all'):\n";
    for (std::size_t i = 0; i < ALGORITHMS.size(); ++i) {
        std::cout << "  [" << (i + 1) << "] " << ALGORITHMS[i].name << '\n';
    }
    std::cout << "  [" << (ALGORITHMS.size() + 1) << "] Alle\n";
    std::cout << "Auswahl (Enter = Alle): ";
    std::string line;
    std::getline(std::cin, line);

    if (line.empty() || line == "all") {
        for (std::size_t i = 0; i < ALGORITHMS.size(); ++i) selectedIndices.push_back(i);
        return true;
    }
    if (line == std::to_string(ALGORITHMS.size() + 1)) {
        for (std::size_t i = 0; i < ALGORITHMS.size(); ++i) selectedIndices.push_back(i);
        return true;
    }

    std::istringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        try {
            int idx = std::stoi(token) - 1;
            if (idx >= 0 && idx < static_cast<int>(ALGORITHMS.size())) {
                selectedIndices.push_back(static_cast<std::size_t>(idx));
            }
        } catch (...) {}
    }
    if (selectedIndices.empty()) {
        for (std::size_t i = 0; i < ALGORITHMS.size(); ++i) selectedIndices.push_back(i);
    }
    return true;
}

static void runInteractive() {
    std::cout << "\n";
    std::cout << "================================================\n";
    std::cout << "   SORTIER-VERGLEICH  |  Shell-Modus\n";
    std::cout << "================================================\n";

    // Algorithmen waehlen
    std::vector<std::size_t> algoIndices;
    promptMultiAlgo(algoIndices);

    // Groesse
    const std::vector<std::string> sizeOpts = { "50", "100", "250", "500", "1000" };
    int sizeIdx = promptMenu("Array-Groesse:", sizeOpts, 1);
    std::size_t arraySize = static_cast<std::size_t>(std::stoi(sizeOpts[static_cast<std::size_t>(sizeIdx)]));

    // Typ
    const std::vector<std::string> typeOpts = { "random (Zufaellig)", "sorted (Sortiert)", "reversed (Umgekehrt)" };
    int typeIdx = promptMenu("Array-Typ:", typeOpts, 0);
    ArrayType arrType = static_cast<ArrayType>(typeIdx);

    std::cout << "\nEreignisse (Compare/Swap) ausgeben? [j/N]: ";
    std::string vLine;
    std::getline(std::cin, vLine);
    bool verbose = (!vLine.empty() && (vLine[0] == 'j' || vLine[0] == 'J' || vLine[0] == 'y' || vLine[0] == 'Y'));

    std::vector<int> data = generateArray(arraySize, arrType);
    std::vector<RunResult> results;
    for (std::size_t idx : algoIndices) {
        results.push_back(runAlgo(ALGORITHMS[idx], data, verbose));
    }
    std::cout << '\n';
    printTable(results, arraySize, arrType);

    std::cout << "\nNochmal? [j/N]: ";
    std::string again;
    std::getline(std::cin, again);
    if (!again.empty() && (again[0] == 'j' || again[0] == 'J' || again[0] == 'y' || again[0] == 'Y')) {
        runInteractive();
    }
}


int main(int argc, char* argv[]) {
    // Keine Argumente → interaktives Menue
    if (argc == 1) {
        runInteractive();
        return 0;
    }

    std::vector<std::string> algoArgs;
    std::size_t arraySize = 100;
    ArrayType   arrayType = ArrayType::Random;
    bool        verbose   = false;
    bool        showHelp  = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--algo" && i + 1 < argc) {
            algoArgs.push_back(argv[++i]);
        } else if (arg == "--size" && i + 1 < argc) {
            try { arraySize = static_cast<std::size_t>(std::stoi(argv[++i])); }
            catch (...) { std::cerr << "Ungueltige Groesse.\n"; return 1; }
        } else if (arg == "--type" && i + 1 < argc) {
            std::string t = argv[++i];
            if      (t == "sorted")   arrayType = ArrayType::Sorted;
            else if (t == "reversed") arrayType = ArrayType::Reversed;
            else if (t == "random")   arrayType = ArrayType::Random;
            else { std::cerr << "Unbekannter Typ: " << t << '\n'; return 1; }
        } else {
            std::cerr << "Unbekanntes Argument: " << arg << '\n';
            printHelp(argv[0]);
            return 1;
        }
    }

    if (showHelp) { printHelp(argv[0]); return 0; }

    std::vector<std::size_t> selectedIndices;
    static const std::map<std::string, std::size_t> nameMap = {
        { "bubble",        0 },
        { "bubblesort",    0 },
        { "selection",     1 },
        { "selectionsort",  1 },
        { "heap",          2 },
        { "heapsort",      2 },
        { "insertion",     3 },
        { "insertionsort",  3 },
        { "radix",         4 },
        { "radixsort",     4 },
        { "bucket",        5 },
        { "bucketsort",    5 },
    };

    if (algoArgs.empty() || std::find(algoArgs.begin(), algoArgs.end(), "all") != algoArgs.end()) {
        for (std::size_t i = 0; i < ALGORITHMS.size(); ++i) selectedIndices.push_back(i);
    } else {
        for (const auto& a : algoArgs) {
            std::string lower = a;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            auto it = nameMap.find(lower);
            if (it == nameMap.end()) {
                std::cerr << "Unbekannter Algorithmus: '" << a << "'. Erlaubt: bubble, selection, heap, insertion, radix, bucket, all\n";
                return 1;
            }
            selectedIndices.push_back(it->second);
        }
    }

    {
        std::vector<std::size_t> deduped;
        for (std::size_t idx : selectedIndices) {
            if (std::find(deduped.begin(), deduped.end(), idx) == deduped.end()) {
                deduped.push_back(idx);
            }
        }
        selectedIndices = deduped;
    }

    std::vector<int> data = generateArray(arraySize, arrayType);
    std::vector<RunResult> results;
    for (std::size_t idx : selectedIndices) {
        results.push_back(runAlgo(ALGORITHMS[idx], data, verbose));
    }
    std::cout << '\n';
    printTable(results, arraySize, arrayType);

    return 0;
}
