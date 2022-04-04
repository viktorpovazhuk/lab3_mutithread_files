//
// Created by petro on 27.03.2022.
//

#ifndef SERIAL_THREAD_FUNCTIONS_H
#define SERIAL_THREAD_FUNCTIONS_H

#include "thread_functions.h"
#include <iostream>
#include "thread_safe_queue.h"
#include <filesystem>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <locale>

void overworkFile(ThreadSafeQueue<std::string> &filesContents, int numOfThreads);

void indexFile(std::map<std::string, size_t> &dict, ThreadSafeQueue<std::string> &filesContents, std::mutex &mut);

void mergeDicts(std::map<std::string, size_t> &dict, std::map<std::string, size_t> &localDict);

void writeInFiles(std::string numResults, std::string alphResults, std::map<std::string, size_t> dict);

bool cmpByAlph(std::pair<std::string, int>& a, std::pair<std::string, int>& b);

bool cmpBynum(std::pair<std::string, int>& a, std::pair<std::string, int>& b);

#endif //SERIAL_THREAD_FUNCTIONS_H
