#include <iostream>
#include "thread_safe_queue.h"
#include <filesystem>
#include <string>
#include <fstream>
#include "files_methods.h"
#include "options_parser.h"
#include "errors.h"

namespace fs = std::filesystem;

using std::string;
using std::cout;
using std::cerr;
using std::endl;

#define PRINT_STEPS
#define PRINT_CONTENT

int main(int argc, char *argv[]) {
    string configFilename;
    if (argc < 2) {
        configFilename = "index.cfg";
    }
    else {
        std::unique_ptr<command_line_options_t> command_line_options;
        try {
            command_line_options = std::make_unique<command_line_options_t>(argc, argv);
        }
        catch (std::exception &ex) {
            cerr << ex.what() << endl;
            return Errors::OPTIONS_PARSER;
        }
        configFilename = command_line_options->config_file;
    }

    std::unique_ptr<config_file_options_t> config_file_options;
    try {
        config_file_options = std::make_unique<config_file_options_t>(configFilename);
    }
    catch (OpenConfigFileException &ex) {
        cerr << ex.what() << endl;
        return Errors::OPEN_CFG_FILE;
    } catch (std::exception &ex) {
        cerr << ex.what() << endl;
        return Errors::READ_CFG_FILE;
    }

    ThreadSafeQueue<fs::path> paths;

    findFiles(config_file_options->indir, paths);

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
