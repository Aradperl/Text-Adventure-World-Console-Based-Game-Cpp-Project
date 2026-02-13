// Riddle.h - Riddle object with questions

#pragma once

#include "GameObject.h"
#include <string>
#include <vector>

class Riddle : public GameObject {
private:
    std::string question;
    std::vector<std::string> options;  // a, b, c, d
    char correctAnswer;

public:
    Riddle(int x, int y, const std::string& q, 
           const std::vector<std::string>& opts, char answer);
    
    // Getters
    const std::string& getQuestion() const;
    const std::vector<std::string>& getOptions() const;
    char getCorrectAnswer() const;
    
    // Check answer
    bool checkAnswer(char answer) const;
    
    // Player can step on it
    bool isBlocking() const override;
};
