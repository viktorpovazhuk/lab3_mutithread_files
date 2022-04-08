//
// Created by petro on 27.03.2022.
//

#include "thread_functions.h"



/*
overworkFile()

Main func in thread. Take element from the queue, index it and merge with global dict.

indexFile()

Split in words, format and count. Do it in reference of dict.

mergeDicts()

Merge to global dict.*/

void overworkFile(ThreadSafeQueue<std::string> &filesContents, std::unordered_map<std::string, int>& dict, std::mutex &mut){

    std::map<std::string, int> localDict;

    while(true){
        std::string file;
        try{
        file = filesContents.deque();
        // TODO: why equal
        if (std::equal(file.begin(), file.end(), "")){
            filesContents.enque("");
            break;
        }
        // TODO: 1. define our error
        //  2. return appropriate code of error (from errors.h)
        } catch (std::error_code e){
            std::cerr << "Error code "<< e << ". Occurred while working with queue in thread." << std::endl;
            continue;
        }
        std::vector <std::string> words;


        indexFile(words, file);

        for (auto & word : words){
            if (localDict.find(word) != localDict.end()){
                localDict.find(word)->second += 1;
            } else {
                localDict.insert({word, 1});
            }
        }

        mut.lock();
        mergeDicts(dict, localDict);
        mut.unlock();
        localDict.clear();
    }

}

void indexFile(std::vector <std::string> &words, std::string& file){

    try{
    std::for_each(file.begin(), file.end(), [](char & c){
        c = std::tolower(c);
    });
    } catch (std::error_code e){
        std::cerr << "Error code "<< e << ". Occurred while transforming word to lowercase" << std::endl;
    }
    size_t start_pos = 0;
    try{
    while((start_pos = file.find(std::string("\n"), start_pos)) != std::string::npos) {
        file.replace(start_pos, std::string("\n").length(), std::string(" "));
        start_pos += std::string(" ").length();
    }

    start_pos = 0;
    while((start_pos = file.find(std::string("\r"), start_pos)) != std::string::npos) {
        file.replace(start_pos, std::string("\r").length(), std::string(" "));
        start_pos += std::string(" ").length();
    }
    } catch (std::error_code e){
        std::cerr << "Error code "<< e << ". Occurred while deleting /n and /r from files" << std::endl;
    }


    try{
    std::stringstream s(file);
    std::string s2;

    while (std::getline (s, s2, ' ')){
        words.push_back(s2);
    }
    } catch (std::error_code e){
        std::cerr << "Error code "<< e << ". Occurred while splitting file into words" << std::endl;
    }

}

void mergeDicts(std::unordered_map<std::string, int> &dict, std::map<std::string, int> &localDict){
    std::map<std::string, int>::iterator i;
    try{
    for (i = localDict.begin(); i != localDict.end(); i++){
        if (dict.find(i->first) != dict.end()){
            dict.at(i->first) += i->second;
        } else {
            dict.insert({i->first, i->second});
        }
    }
    } catch (std::error_code e){
        std::cerr << "Error code "<< e << ". Occurred while merging dicts" << std::endl;
    }
}

