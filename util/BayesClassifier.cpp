//
// Created by elise on 5/3/2020.
//

#include "BayesClassifier.h"


void BayesClassifier::classify() {
    std::cout << "classifying tweets..." << std::endl;
    if(this->reClassify){
        std::cout << "Reassifying..." << std::endl;
        std::ifstream inFile;
        inFile.open(dataFile);
        if(!inFile.is_open()){throw std::invalid_argument("No such file: " + dataFile);}
        std::string line;
        std::getline(inFile,line); // getting rid of first line
        while(std::getline(inFile,line)){
            std::istringstream iss(line);                     // gathering some information from the csv
            std::getline(iss, line, ',');         // putting the stuff into the line var
            std::getline(iss, line, ',');         // passing the line var to the checkline method
            std::getline(iss, line, ',');        // which then does some more processing things
            std::getline(iss, line, ',');
            checkline(line);
        }
        calculateAccuracy();
    }
    else{
        std::cout << "using previous..." << std::endl;
        calculateAccuracy();
    }
}

void BayesClassifier::checkline(std::string &line) {
    std::ifstream iFile("classify.txt");
    std::ofstream of;
    if(iFile.is_open() && mode){
        mode = 0;
        of.open("classify.txt");
    }
    else of.open("classify.txt", std::ios_base::app);
    std::istringstream iss(line);
    std::string word;                                            // gets the bayes value for both
    double bayesPositive = (double)this->tweetData.getPositiveTweets() / (double)this->tweetData.getTotalTweets();
    double bayesNegative = (double)this->tweetData.getNegativeTweets() / (double)this->tweetData.getTotalTweets();
    while(std::getline(iss, word,  ' ')){
        if(word.size() < 3) continue;
        remove(word);
        Word w(word);
        w.stem();
                                                                                     // updates the bayes value based
        if(this->tweetData.containsWord(w) && tweetData.getWord(w).getCnt() > 4 ){   // on the positivity of curr word
            bayesPositive *= ((double)tweetData.getWord(w).getPositiveCnt() / (double)tweetData.getPositiveTweets());
            bayesNegative *= ((double)tweetData.getWord(w).getNegativeCnt() / (double)tweetData.getNegativeTweets());
        }
    }
    of << normalize(bayesPositive, bayesNegative) << " " << line << std::endl;   // normalizes both values
    of.close();
}

void BayesClassifier::calculateAccuracy() {
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

char BayesClassifier::normalize(double &postiveBayes, double &negativeBayes) {  // normalilzes the bayes values
    this->evidenceValue = postiveBayes + negativeBayes;                         // we calculated above
    double pos = postiveBayes / evidenceValue;                                  // if positive is greater then it is
    double neg = negativeBayes / evidenceValue;                                 // probably a positive tweet
    //std::cout << pos << " " << neg << std::endl;                              // if negative is greater then it is
    if(pos == neg){                                                             // probably a negative tweet
        posi = !posi;
        return posi? '+' : '-';

    }
    else
        return pos > neg? '+' : '-';
}
