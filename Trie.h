#pragma once

#include <string>;
#include <unordered_map>;
#include <memory>;

class TrieNode {
	bool _isWordEnd;
	std::unordered_map<char, std::unique_ptr<TrieNode>> children;

public:
	TrieNode() : _isWordEnd(false) {}
	void addChild(char character);
	void removeChild(char character);
	void setWordEnd(bool isWordEnd);
	bool isWordEnd();
	TrieNode* getChild(char character) const;
	bool isWordExist(const std::string& word);
	size_t countWordChildren(const std::string& word);
	void addWord(const std::string& word);
	void removeWord(const std::string& word);
	bool removeWord_i(TrieNode* currentNode, const std::string& word, int i);
};

class Trie {
	TrieNode root;
public:
	void addWord(const std::string& word);
	void removeWord(const std::string& word);
	bool isWordExist(const std::string& word);
	size_t countWordChildren(const std::string& word);
};