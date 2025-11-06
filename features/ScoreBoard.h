#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "../utils/FileUtils.h"
#define FILE_SCORES "scores.txt"

struct PlayerScore
{
    std::string player_name;
    int score;
};

class ScoreBoard{
private:
    std::vector<PlayerScore> scores;
    void sort_scores_desc();
public:
    ScoreBoard();

    void init_score_board();
    void display_score_board();
    bool add_score(const std::string& name, int score);

    void fake_data(){
        add_score("Alice", 150);
        add_score("Bob", 200);
        add_score("Charlie", 120);
        add_score("Dave", 180);
    }
};



ScoreBoard::ScoreBoard() {
     init_score_board();
}

void ScoreBoard::init_score_board() {
    scores.clear();
    std::vector<std::string> lines = readFileLines(FILE_SCORES);

    for(std::string& line : lines){
        std::stringstream ss(line); // use steam to split by comma

        std::string player_name;
        getline(ss, player_name, ',');
        std::string score_str;
        getline(ss, score_str, ',');

        int score = std::stoi(score_str);
        scores.push_back({player_name, score});
    }
    sort_scores_desc();
}

void ScoreBoard::sort_scores_desc() {
    std::sort(scores.begin(), scores.end(), [](const PlayerScore& a, const PlayerScore& b) {
        return a.score > b.score;
    });
}

bool ScoreBoard::add_score(const std::string& name, int score) {
    if (score < 0) {
        std::cout << "Invalid score. Score must be non-negative." << std::endl;
        return false;
    }
    PlayerScore new_score{name, score};
    scores.push_back(new_score);
    // appendFile(FILE_SCORES, name + "," + std::to_string(score));
    sort_scores_desc();
    return true;
}

void ScoreBoard::display_score_board() {
    std::cout << "========= Score Board =========" << std::endl << std::endl << std::endl;
	std::cout << "Player Name \t\tScore" << std::endl;
	std::cout << "------------------------------" << std::endl;
    for (const auto& entry : scores) 
        std::cout << entry.player_name << "\t\t\t" << entry.score << std::endl;

	std::cout << std::endl << std::endl << "===============================" << std::endl;
}