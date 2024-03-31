#include <iostream>
#include <vector>
#include <functional>
#include <list>

using namespace std;
//using fcnOfTwoArguments = function<bool(const vector<bool>&, const vector<bool>&)>;

class StrategyAnalysis;
class Person;

enum GameOutcome {
    Draw,
    WinnerPlayer1,
    WinnerPlayer2
};

namespace Strategies {
    
    bool titForTat(const vector<bool>&, const vector<bool>&);
    bool oppositeToTitForTat(const vector<bool>&, const vector<bool>&);
    bool modifiedTitForTatForOdd(const vector<bool>&, const vector<bool>&); //funct1
    bool modifiedTitForTatForEven(const vector<bool>&, const vector<bool>&); // funct2
    bool titForTwoTat(const vector<bool>&, const vector<bool>&);
    
    list <function<bool(const vector<bool>&, const vector<bool>&)>> ListOfStrategies {
        titForTat, 
        oppositeToTitForTat,
        modifiedTitForTatForOdd,
        modifiedTitForTatForEven,
        titForTwoTat
    };

    bool titForTat(const vector<bool>& thisPlayerMoves, const vector<bool>& playerMoves) {
        return playerMoves.empty() ? true : playerMoves.back();
    }

    bool oppositeToTitForTat(const vector<bool>& thisPlayerMoves, const vector<bool>& playerMoves) {
        return !titForTat(thisPlayerMoves, playerMoves);
    }

    // funct1
    bool modifiedTitForTatForOdd(const vector<bool>& thisPlayerMoves, const vector<bool>& playerMoves) {
        if (playerMoves.empty())
            return true;
        if (!playerMoves.back())
            return false;
        if (playerMoves.size() % 2)
            return false;
        return true;
    }

    // funct2
    bool modifiedTitForTatForEven(const vector<bool>& thisPlayerMoves, const vector<bool>& playerMoves) {
        return !modifiedTitForTatForOdd(thisPlayerMoves, playerMoves);
    }

    bool titForTwoTat(const vector<bool>& thisPlayerMoves, const vector<bool>& playerMoves) {
        if (playerMoves.size() < 2) {
            return modifiedTitForTatForOdd(thisPlayerMoves, playerMoves);
        }
        if (!playerMoves[playerMoves.size() - 2] && !playerMoves[playerMoves.size() - 3])
            return false;
        return true;

    }
};

class Player {
public:
    function<bool(const vector<bool>&, const vector<bool>&)> currentStrategy;
    vector<bool> moves;
    bool currentMove;
    int score = 0;

    Player(function<bool(const vector<bool>&, const vector<bool>&)> strategy) : currentStrategy(strategy) {}

    void move(vector<bool>& enemyMoves) {
        currentMove = currentStrategy(moves, enemyMoves);
    }

    void safeCurrentMove() {
        moves.push_back(currentMove);
    }

    bool back() const {
        return moves.back();
    }

    void divide() {
        ++score;
    }

    void steal() {
        score += 2;
    }

    bool noMoves() {
        return moves.empty();
    }

    bool getLast(int index) {
        return moves[moves.size() - 1 - index];
    }

    void printCurrentMove() {
        cout << currentMove << '\t';
    }
};


namespace GameManager {
    void calculationResult(Player& player1, Player& player2) {
        if (player1.noMoves() || player2.noMoves()) {
            return;
        }
        if (player1.back() && player2.back()) {
            player1.divide();
            player2.divide();
        }
        else if (!player1.back() && player2.back()) { 
            player1.steal();
        }
        else if (!player2.back() && player1.back()) { 
            player2.steal();
        }
    }

    int winnerIdentification(const Player& player1, const Player& player2) {
        if (player1.score > player2.score)
            return GameOutcome::WinnerPlayer1;
        if (player1.score < player2.score)
            return GameOutcome::WinnerPlayer2;
        return GameOutcome::Draw;
    }

    void printResult(const Player& player1, const Player& player2) {
        cout << "\nPlayer 1 Score: " << player1.score << '\n';
        cout << "Player 2 Score: " << player2.score << '\n';

        int result = winnerIdentification(player1, player2);
        switch (result) {
        case GameOutcome::Draw:
            cout << "It's a draw!\n";
            break;
        case GameOutcome::WinnerPlayer1:
            cout << "Player 1 wins!\n";
            break;
        case GameOutcome::WinnerPlayer2:
            cout << "Player 2 wins!\n";
        }
    }
    
    int differenceCalc(const Player& player1, const Player player2) {
    	return player1.score - player2.score;
    }
};

class Strategy {
    function<bool(const vector<bool>&, const vector<bool>&)> name;
    list <function<bool(const vector<bool>&, const vector<bool>&)>> ComparisonList;

public:
    Strategy(function<bool(const vector<bool>&, const vector<bool>&)> fcn) : name(fcn) {}

    void addToWins(function<bool(const vector<bool>&, const vector<bool>&)>& strategy) {
        ComparisonList.push_back(strategy);
    }
};

class StrategyAnalysis {
private:
    list <Strategy> ListOfStrategies;
public:
    int evaluateStrategy(function<bool(const vector<bool>&, const vector<bool>&)>& strategy1, function<bool(const vector<bool>&, const vector<bool>&)>& strategy2) {
        Player player1{ strategy1 };
        Player player2{ strategy2 };
        for (size_t numberOfRounds = 10u; numberOfRounds > 0u; --numberOfRounds) {
            player1.move(player1.moves);
            player2.move(player2.moves);

            GameManager::calculationResult(player1, player2);

            player1.safeCurrentMove();
            player2.safeCurrentMove();
        }
        return GameManager::differenceCalc(player1, player2);
    }

    StrategyAnalysis(function<bool(const vector<bool>&, const vector<bool>&)>& strategy) {
        for (auto& strategyForTesting : Strategies::ListOfStrategies) {
            for (auto& strategyOponent : Strategies::ListOfStrategies) {
                int result = evaluateStrategy(strategyForTesting, strategyOponent);
                pair<int, function<bool(const vector<bool>&, const vector<bool>&)>> temp(result, strategy);
                //ListOfStrategies.push_back(temp);
            }
        }
    }
};

int main() {
    Player player1{ Strategies::modifiedTitForTatForOdd };
    Player player2{ Strategies::titForTwoTat };
    size_t numberOfRounds = 5u;

    cout << "Player1\t" << "Player2\t" << "Score1\t" << "Score2\n";
    for (size_t round = 1u; round <= numberOfRounds; ++round) {
        player1.move(player1.moves);
        player1.printCurrentMove();

        player2.move(player2.moves);
        player2.printCurrentMove();

        cout << player1.score << '\t' << player2.score << "\t\n";
        GameManager::calculationResult(player1, player2);

        player1.safeCurrentMove();
        player2.safeCurrentMove();
    }

    GameManager::printResult(player1, player2);
}
