
#include <iostream>
#include <unordered_map>
#include <list>
#include <string>
#include <unordered_set>
#include <format>
#include "BoggleBoard.h"
#include "Trie.h"

using namespace std;

void initDictionary(Trie* dictionary) {
    std::cout << "caching the dictionary..." << std::endl;
    std::ifstream file("words_alpha.txt");
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            dictionary->addWord(line);
        }
        file.close();
    }
    else {
        std::cerr << "Unable to open file" << std::endl;
    }
}

int main()
{
    Trie dictionary;
    initDictionary(&dictionary);

    BoggleBoard_t board;
    board[0][0] = 'a';
    board[0][1] = 'b';
    board[0][2] = 'c';
    board[0][3] = 'd';
    board[1][0] = 'e';
    board[1][1] = 'f';
    board[1][2] = 'g';
    board[1][3] = 'h';
    board[2][0] = 'i';
    board[2][1] = 'j';
    board[2][2] = 'k';
    board[2][3] = 'l';
    board[3][0] = 'm';
    board[3][1] = 'n';
    board[3][2] = 'o';
    board[3][3] = 'p';

    BoggleBoard boggleBoard(board, &dictionary, 32);
    boggleBoard.dump();
    std::cout << "solving..." << std::endl;
    set<string> words = boggleBoard.solve();
    std::cout << "--------------------" << "\n";
    std::cout << format("found {} words: ", words.size()) << "\n";
    std::cout << "--------------------" << "\n";

    for (auto& word : words) {
        cout << format("{} ", word);
    }

    std::string longestWord = "";
    for (const std::string& word : words) {
        if (longestWord.size() < word.size()) {
            longestWord = word;
        }
    }
    std::cout << "--------------------" << "\n";
    std::cout << "longest word: " << longestWord << std::endl;
    std::cout << "--------------------" << "\n";
    boggleBoard.shutdown();


    BoggleBoard boggleBoardRandom(20, 20, &dictionary, 32);
    boggleBoardRandom.dump();
    std::cout << "solving..." << std::endl;
    words = boggleBoardRandom.solve();
    std::cout << "--------------------" << "\n";
    std::cout << format("found {} words: ", words.size()) << "\n";
    std::cout << "--------------------" << "\n";

    for (auto& word : words) {
        cout << format("{} ", word);
    }

    longestWord = "";
    for (const std::string& word : words) {
        if (longestWord.size() < word.size()) {
            longestWord = word;
        }
    }
    std::cout << "--------------------" << "\n";
    std::cout << "longest word: " << longestWord << std::endl;
    std::cout << "--------------------" << "\n";

    boggleBoardRandom.shutdown();
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
