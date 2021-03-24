// Dog-Cat-Classifier.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>

#define USE_THREAD_POOL
#ifdef USE_THREAD_POOL
#include "ThreadPool.h"
#endif // USE_THREAD_POOL
namespace fs = std::filesystem;

enum class animalType
{
    cat = 0,
    dog = 1,
    unknown = 2
};

animalType classify(const std::string &fileName) {
    if (fileName.find("cat") != std::string::npos) 
        return animalType::cat;
    else if(fileName.find("dog") != std::string::npos)
        return animalType::dog;
    return animalType::unknown;
}

void Upload(const std::string& fileName, const std::string& type) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
#ifndef USE_THREAD_POOL
    //Solving this with a thread pool will be better for real cases because
    //we'll be able to upload results as they come in.
    //with this solution we're collecting results from all files and only then uplading them
    //I'm adding this solution since i took the thread pool implementation 
    //from github it's not from a known library
    
    std::vector<std::string> species = { "cat", "dog", "unknown" };
    std::string path = "../Data/";
    std::vector<std::string> fileNames;
    std::vector<std::string> prediction;
    
    //assuming the "data" folder structure is such that it contains 
    //folders only and those folders contain the images
    for (const auto& dirs : fs::directory_iterator(path)) {
        for (const auto& files : fs::directory_iterator(dirs.path())) {
            std::string fileName{ fs::path(files.path()).filename().u8string() };
            animalType type = classify(fileName);
            std::string& pred(species[(int)type]);
            std::cout << fileName << " is a " << pred << std::endl;

            fileNames.push_back(fileName);
            prediction.push_back(pred);
        }
    }

#pragma omp parallel for
    for (int i = 0; i < fileNames.size(); i++) {
        Upload(fileNames[i], prediction[i]);
    }   

#else
    //code for thread pool implementation taken from here - 
    //https://github.com/bshoshany/thread-pool
    
    std::vector<std::string> species = { "cat", "dog", "unknown" };
    std::string path = "../Data/";
    std::vector<std::future<bool>> futures;
    
    {
        thread_pool pool(std::thread::hardware_concurrency());
        //assuming the "data" folder structure is such that it contains 
        //folders only and those folders contain the images
        for (const auto& dirs : fs::directory_iterator(path)) {
            for (const auto& files : fs::directory_iterator(dirs.path())) {
                std::string fileName{ fs::path(files.path()).filename().u8string() };
                animalType type = classify(fileName);
                std::string& pred(species[(int)type]);
                std::cout << fileName << " is a " << pred << std::endl;
                futures.push_back(pool.submit(Upload, fileName, pred));
            }
        }
    }//thread_pool goes out of scope, all threads are joined
#endif
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
