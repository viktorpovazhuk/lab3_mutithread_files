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

void overworkFile(ThreadSafeQueue<std::string> &filesContents, int numOfThreads, std::string numResults, std::string alphResults){

    std::map<std::string, size_t> dict;
    std::vector<std::thread> threads;
    std::mutex mut;

    for (size_t i = 0; i < numOfThreads; i++){
        threads.emplace_back(indexFile, std::ref(dict), std::ref(filesContents), std::ref(mut));
    }

    for (size_t i = 0; i < numOfThreads; i++){
        threads[i].join();
    }

    writeInFiles(numResults, alphResults, dict);
}

void indexFile(std::map<std::string, size_t> &dict, ThreadSafeQueue<std::string> &filesContents, std::mutex &mut){

    thread_local std::map<std::string, size_t> localDict;
    while(true){
        std::string file = filesContents.deque();
        if (std::equal(file.begin(), file.end(), "")){
            filesContents.enque("");
            break;
        }
        std::for_each(file.begin(), file.end(), [](char & c){
            c = std::tolower(c);
        });
        std::replace( file.begin(), file.end(), "\n", " ");
        std::replace( file.begin(), file.end(), "\r", " ");

        std::vector <std::string> words;

        std::stringstream s(file);
        std::string s2;

        while (std::getline (s, s2, ' ')){
            words.push_back(s2);
        }

        for (size_t i = 0; i < words.size(); i++){
            if (localDict.find(words[i]) != localDict.end()){
                localDict.find(words[i])->second + 1;
            } else {
                localDict.insert({words[i], 1});
            }
        }
        mut.lock();
        mergeDicts(dict, localDict);
        mut.unlock();
    }


}

void mergeDicts(std::map<std::string, size_t> &dict, std::map<std::string, size_t> &localDict){
    for (size_t i = 0; i < localDict.size(); i++){
        if (dict.find(localDict.begin()->first) != dict.end()){
            dict.find(localDict.begin()->first)->second + localDict.begin()->second;
        } else {
            dict.insert({localDict.begin()->first, localDict.begin()->second});
        }
        localDict.erase(localDict.begin()->first);
    }
}

void writeInFiles(std::string numResults, std::string alphResults, std::map<std::string, size_t> dict){

    std::ofstream myfile;
    myfile.open (numResults);
    std::sort(dict.begin(), dict.end(), cmpBynum);
    for(auto& el : dict) {
        myfile << el.first << " = " << el.second << '\n';
    }
    myfile.close();

    myfile.open (alphResults);
    std::sort(dict.begin(), dict.end(), cmpByAlph);
    for(auto& el : dict) {
        myfile << el.first << " = " << el.second << '\n';
    }
    myfile.close();
}

bool cmpBynum(std::pair<std::string, int>& a,
         std::pair<std::string, int>& b)
{
    return a.second < b.second;
}

bool cmpByAlph(std::pair<std::string, int>& a,
         std::pair<std::string, int>& b)
{

    if (a.first[0] != b.first[0]){
        return a.first[0] < b.first[0];
    } else{
        int i = 0;
        while (a.first[i] == b.first[i]){
            i++;
        }
        return a.first[0] < b.first[0];
    }
}