//
// Created by elise on 5/2/2020.
//

#include "TrivialDS.h"

void TrivialDS::classify(){                              // does some io stuff and passes information off to checkline
    std::cout << "classifying tweets..." << std::endl;   // to do futher processing
    if(this->reClassify){
        std::cout << "Reclassifying..." << std::endl;
        std::ifstream inFile;
        inFile.open(dataFile);
        if(!inFile.is_open()){throw std::invalid_argument("No such file: " + dataFile);}
        std::string line;
        std::getline(inFile,line); // getting rid of first line
        while(std::getline(inFile,line)){
            std::istringstream iss(line);
            while(std::getline(iss, line, ','));
            checkline(line);
        }
        calculateAccuracy();   // calculates the accuracy of the ratings
    }
    else{
        std::cout << "using previous..." << std::endl;   // this uses previous classification data and simply calculates
        calculateAccuracy();                             // the accuracy.
    }

}

void TrivialDS::checkline(std::string& line){
    std::ifstream iFile("classify.txt");
    std::ofstream of;
    if(iFile.is_open() && mode){
        mode = 0;
        of.open("classify.txt");
    }
    else of.open("classify.txt", std::ios_base::app);
    std::istringstream iss(line);
    std::string word;
    int sum = 0;
    while(std::getline(iss, word,  ' ')){   // makes sure to remove any stop words and stems the worc
        if(word.size() < 3) continue;                  // if tweet has a net of negatvie values, then it gets a negaive
        remove(word);                               //score. if positive it gets a postive. The tweets overall score
        Word w(word);                                // depends on the sum of the scores of trhe individual word.
        w.stem();

        if(this->tweetData.containsWord(w)){
            sum += tweetData.getWord(w).getPositiveCnt();
            sum -= tweetData.getWord(w).getNegativeCnt();
        }
    }
    if(sum > 0){
        of << '+' << " " << line  << std::endl;
    }
    else if(sum < 0){
        of << '-' << " "<<  line <<std::endl;
    }
    else if(pos == 1){  // assume undetermined score to be positive
        pos = 0;
        of << '+' << " " << line << std::endl;
    }
    else{
        pos = 1;
        of << '-' << " " << line << std::endl;
    }
    of.close();

}

void TrivialDS::calculateAccuracy(){
    std::ofstream of;

    std::ifstream inFile(this->targetFile);
    std::ifstream inFile2("classify.txt");
    if(!inFile.is_open() || !inFile2.is_open()){throw std::invalid_argument("Unable to locate files: " );}
    std::string line;
    std::string line2;
    std::getline(inFile,line);
    double accuracy = 0;
    while(std::getline(inFile,line) && std::getline(inFile2, line2)){
        char rating;
        std::istringstream iss2(line2);
        iss2 >> rating;
        total++;
        std::string rate;
        std::istringstream iss(line);
        std::getline(iss,rate,',');
        std::getline(iss,rate,',');
        if( (rating == '-' && rate == "0") || (rating == '+' && rate =="4")){
            correct ++;
        }
    }
    inFile. close();
    inFile2.close();
    of.open("accuracy.txt");
    if(!of.is_open()){throw std::exception_ptr();}
    accuracy = (double)correct / (double)total;
    std::cout << correct << "/" << total << std::endl;
    of << accuracy;
    of.close();
}