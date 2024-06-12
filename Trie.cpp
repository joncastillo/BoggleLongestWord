#include "Trie.h"

void TrieNode::addChild(char character) {
	if (children.find(character) == children.end()) {
		children.insert({ character, std::make_unique<TrieNode>() });
	}
}
void TrieNode::removeChild(char character) {
	if (children.find(character) != children.end()) {
		children.erase(character);
	}
}

void TrieNode::setWordEnd(bool isWordEnd) {
	this->_isWordEnd = isWordEnd;
}

bool TrieNode::isWordEnd() {
	return _isWordEnd;
}

TrieNode* TrieNode::getChild(char character) const {
	auto it = children.find(character);
	return it != children.end() ? it->second.get() : nullptr;
}

bool TrieNode::isWordExist(const std::string& word) {
	TrieNode* currentNode = this;
	for (int i = 0; i < word.size(); i++) {
		char currentChar = word.at(i);
		if (currentNode->getChild(currentChar) == nullptr) {
			return false;
		}
		currentNode = currentNode->getChild(currentChar);
	}

	return currentNode->isWordEnd();
}

size_t TrieNode::countWordChildren(const std::string& word) {
	TrieNode* currentNode = this;
	for (int i = 0; i < word.size(); i++) {
		char currentChar = word.at(i);
		if (currentNode->getChild(currentChar) == nullptr) {
			return false;
		}
		currentNode = currentNode->getChild(currentChar);
	}
	return currentNode->children.size();
}

void TrieNode::addWord(const std::string& word) {
	TrieNode* currentNode = this;
	for (int i = 0; i < word.size(); i++) {
		char currentChar = word.at(i);
		if (currentNode->getChild(currentChar) == nullptr) {
			currentNode->addChild(currentChar);
		}
		currentNode = currentNode->getChild(currentChar);
	}

	currentNode->setWordEnd(true);
}

void TrieNode::removeWord(const std::string& word) {
	TrieNode* currentNode = this;

	if (word.size() == 0) {
		return;
	}
	if (word.size() == 1) {
		char character = word.at(0);
		auto it = children.find(character);

		if (it != children.end()) {
			auto child = it->second.get();
			if (child->children.size() == 0) {
				// can be deleted.
				currentNode->children.erase(character);
			}
			else {
				child->setWordEnd(false);
			}
		}
	}
	else {
		removeWord_i(currentNode, word, 0);
	}
	// dfs into end.
}

bool TrieNode::removeWord_i(TrieNode* currentNode, const std::string& word, int i) {
	// stop at last. wordSize = 5, i == 4
	size_t wordSize = word.size();
	if (i == wordSize) {
		return true;
	}

	// get next Node
	char currentChar = word.at(i);
	auto it = currentNode->children.find(currentChar);
	if (it == currentNode->children.end()) {
		return false;
	}

	TrieNode* nextNode = currentNode->children.at(currentChar).get();

	// recurse
	if (!removeWord_i(nextNode, word, i + 1)) {
		return false;
	}

	if (i == wordSize - 1) {
		nextNode->setWordEnd(false);
	}

	// remove next node if nothing else uses it.
	if (nextNode->children.empty()) {
		currentNode->children.erase(currentChar);
	}

	return true;
}

void Trie::addWord(const std::string& word) {
	root.addWord(word);
}

void Trie::removeWord(const std::string& word) {
	root.removeWord(word);
}

bool Trie::isWordExist(const std::string& word) {
	return root.isWordExist(word);
}

size_t Trie::countWordChildren(const std::string& word) {
	return root.countWordChildren(word);
}