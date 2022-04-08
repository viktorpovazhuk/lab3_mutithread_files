#include <iostream>
#include "thread_safe_queue.h"
#include <filesystem>
#include <string>
#include <fstream>
#include "files_methods.h"
#include "options_parser.h"
#include "errors.h"
#include "thread_functions.h"
#include "write_in_file.h"
#include "time_measurement.h"

namespace fs = std::filesystem;

using std::string;
using std::cout;
using std::cerr;
using std::endl;

#define PRINT_STEPS
//#define PRINT_CONTENT
#define PARALEL

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

    auto totalTimeStart = get_current_time_fenced();

    auto time_start = get_current_time_fenced();

    ThreadSafeQueue<fs::path> paths;

    std::thread filesEnumThread(findFiles, std::ref(config_file_options->indir), std::ref(paths));

    if(filesEnumThread.joinable()){
        filesEnumThread.join();
    }


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
    filesContents.setMaxElements(100);

    std::thread filesReadThread(readFiles, std::ref(paths), std::ref(filesContents));

    if(filesReadThread.joinable()){
        filesReadThread.join();
    }

    auto time_finish = get_current_time_fenced();

    auto timeReading = to_us(time_finish - time_start);

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

    std::string fn = config_file_options->out_by_n;
    std::string fa = config_file_options->out_by_a;
    int nt = config_file_options->indexing_threads;

    FILE *file;
    file = fopen(fn.c_str(), "r");
    if (file) {
        fclose(file);
    } else {
        std::ofstream MyFile(fn);
        MyFile.close();
    }

    file = fopen(fa.c_str(), "r");
    if (file) {
        fclose(file);
    } else {
        std::ofstream MyFile(fa);
        MyFile.close();
    }

    time_start = get_current_time_fenced();

    std::unordered_map<std::string, int> dict;
    std::mutex mut;
#ifdef PARALEL
    std::vector<std::thread> threads;

    threads.reserve(nt);
    try{
    for (int i = 0; i < nt; i++){
        threads.emplace_back(overworkFile, std::ref(filesContents), std::ref(dict), std::ref(mut));
    }
    } catch (std::error_code e){
        std::cerr << "Error code "<< e << ". Occurred while splitting in threads." << std::endl;
    }

    try{
    for (int i = 0; i < nt; i++){
        threads[i].join();
    }
    } catch (std::error_code e){
        std::cerr << "Error code "<< e << ". Occurred while joining in threads." << std::endl;
    }
    threads.clear();
#else
    overworkFile(filesContents, dict, mut);
#endif

    time_finish = get_current_time_fenced();

    auto timeFinding = to_us(time_finish - time_start);

    time_start = get_current_time_fenced();

    writeInFiles(fn, fa, dict);

    time_finish = get_current_time_fenced();

    auto timeWriting = to_us(time_finish - time_start);

    auto totalTimeFinish = get_current_time_fenced();

    auto timeTotal = to_us(totalTimeFinish - totalTimeStart);

    cout << "Total=" << timeTotal << "\n"
    << "Reading=" << timeReading << "\n"
    << "Finding=" << timeFinding << "\n"
    << "Writing=" << timeWriting;

    return 0;
}
