// Riddle.cpp - Multiple choice riddle blocking player progress

#include "Riddle.h"
#include <cctype>

// Constructor
Riddle::Riddle(int x, int y, const std::string& q,
               const std::vector<std::string>& opts, char answer)
    : GameObject(x, y, '?', Color::LightRed),
      question(q), options(opts), correctAnswer(std::tolower(answer)) {
}

// Getters
const std::string& Riddle::getQuestion() const {
    return question;
}

const std::vector<std::string>& Riddle::getOptions() const {
    return options;
}

char Riddle::getCorrectAnswer() const {
    return correctAnswer;
}

// Check answer
bool Riddle::checkAnswer(char answer) const {
    return std::tolower(answer) == correctAnswer;
}

// Player can step on riddle to trigger it
bool Riddle::isBlocking() const {
    return false;
}
