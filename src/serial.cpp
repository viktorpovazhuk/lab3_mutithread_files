#include <iostream>
#include "thread_safe_queue.h"
#include <filesystem>
#include <string>
#include <fstream>
#include "files_methods.h"

namespace fs = std::filesystem;

using std::string;
using std::cout;

#define PRINT_STEPS
#define PRINT_CONTENT

int main() {
    ThreadSafeQueue<fs::path> paths;
    string filesDirectory = "../data";

    findFiles(filesDirectory, paths);

#ifdef PRINT_CONTENT
    auto path = paths.deque();
    paths.enque(path);
    while (path != fs::path("")) {
        cout << path << '\n';
        path = paths.deque();
        paths.enque(path);
    }
    cout << "--------------------------" << '\n';
#endif

    ThreadSafeQueue<string> filesContents;

    readFiles(paths, filesContents);

#ifdef PRINT_CONTENT
    auto content = filesContents.deque();
    filesContents.enque(content);
    while (!content.empty()) {
        cout << content << '\n';
        content = filesContents.deque();
        filesContents.enque(content);
    }
    cout << "--------------------------" << '\n';
#endif

    return 0;
}
