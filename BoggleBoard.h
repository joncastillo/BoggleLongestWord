#pragma once
#include <iostream>
#include <format>
#include <vector>
#include <thread>
#include <map>
#include <latch>
#include <string>
#include <set>
#include <mutex>
#include <latch>
#include <memory>
#include <unordered_set>
#include <fstream>

#include "PubSub.h"
#include "RandomizedChar.h"

#include "Trie.h"
/**
*
*  ---------------------
* 1. Init (maxrow, maxcol, maxthreads)
* 2.	- init boggleboard
*			- randomize chars
* 2.	- init semaphore
* 3.	- init worker threads (pubsub)
*			- register to pubsub via sub, lambda
*				lambda:
*					[](message_t) {
*						boggleboard& = message.boggleBoard
*						row = message.row;
*						col = message.col;
*						semaphore++
*						- init visited.
*						- init strbuf
*						do_dfs(boggleboard, visited, row, col, strbuf, worList)
*					}
*  ---------------------

* 3. For every element in boggleBoard,
* 4.	- create Message
* 5.		- boggleBoard&
* 6.		- i
* 7.		- j
* 8.	- send Message to Pub
*
*/

using BoggleBoard_t = std::map<int, std::map<int, char>>;
using VisitedBoard_t = std::map<int, std::map<int, bool>>;
struct Message {
	const BoggleBoard_t& boggleBoard;
	int row;
	int col;
};

class BoggleBoard {
	size_t maxRow;
	size_t maxCol;
	size_t maxThreads;

	PubSub<Message> messageQueue;
	BoggleBoard_t boggleBoard;
	Trie* dictionary;

	std::set<std::string> consolidatedWords;
	std::mutex mutexConsolidatedWords;
	std::unique_ptr<std::latch> pLatch;

	std::string longestWord = "";
	int longestWordSize = 0;

	std::string joinSet(const std::unordered_set<std::string>& strings, const std::string& delimiter) {
		std::string result;
		for (auto iter = strings.begin(); iter != strings.end(); ++iter) {
			if (iter != strings.begin()) {
				result += delimiter;
			}
			result += *iter;
		}
		return result;
	}


	void processMessage(const BoggleBoard_t& boggleBoard, size_t row, size_t col, std::set<std::string>& consolidatedWords, int threadNo) {
		std::unordered_set<std::string> consolidatedWords_internal;
		VisitedBoard_t visitedBoard;
		for (size_t k = 0; k < this->maxRow; k++) {
			for (size_t l = 0; l < this->maxCol; l++) {
				visitedBoard[k].emplace(l, false);
			}
		}

		std::string stringBuffer;
		stringBuffer.reserve(maxRow * maxCol);

		doDfs(boggleBoard, visitedBoard, row, col, stringBuffer, consolidatedWords_internal);

		if (pLatch.get() != nullptr) {
			pLatch.get()->count_down();
		}


		{
			std::unique_lock<std::mutex> lock{ mutexConsolidatedWords };
			consolidatedWords.insert(consolidatedWords_internal.begin(), consolidatedWords_internal.end());
		}

		std::string completed = joinSet(consolidatedWords_internal, ", ");
		if (!completed.empty())
			std::cout << std::format("{{{:02},{:02}}} by T{:02}. Found: {}", row, col, threadNo, completed) << std::endl;
	}

	void doDfs(const BoggleBoard_t& boggleBoard, VisitedBoard_t& visitedBoard, size_t currentRow, size_t currentCol, std::string& stringBuffer, std::unordered_set<std::string>& consolidatedWords) {
		visitedBoard[currentRow][currentCol] = true;
		stringBuffer += boggleBoard.at(currentRow).at(currentCol);

		if (dictionary->isWordExist(stringBuffer)) {
			consolidatedWords.insert(stringBuffer);
		}

		int childCount = dictionary->countWordChildren(stringBuffer);
		if (childCount != 0) {
			std::vector<std::pair<int, int>> neighbors = getValidNeighbors(boggleBoard, visitedBoard, currentRow, currentCol);
			for (auto& [childRow, childCol] : neighbors) {
				doDfs(boggleBoard, visitedBoard, childRow, childCol, stringBuffer, consolidatedWords);
			}
		}

		stringBuffer.pop_back();
		visitedBoard[currentRow][currentCol] = false;
	}

	std::vector<std::pair<int, int>> getValidNeighbors(const BoggleBoard_t& boggleBoard, const VisitedBoard_t& visitedBoard, size_t currentRow, size_t currentCol) {
		std::vector<std::pair<int, int>> neighbors;
		int neighborOffsets[][2] = {
			{-1,-1},
			{-1,0},
			{-1,1},
			{0,-1},
			/* no 0,0 */
			{0,1},
			{1,-1},
			{1,0},
			{1,1}
		};
		for (auto& [offsetRow, offsetCol] : neighborOffsets) {
			int neighborRow = currentRow + offsetRow;
			int neighborCol = currentCol + offsetCol;

			if (neighborRow < 0 || neighborCol < 0 || neighborRow >= maxRow || neighborCol >= maxCol) {
				// out of bounds
				continue;
			}

			if (visitedBoard.at(neighborRow).at(neighborCol)) {
				continue;
			}

			neighbors.emplace_back(neighborRow, neighborCol);
		}
		return neighbors;
	}

public:
	BoggleBoard(BoggleBoard_t& origin, Trie* dictionary, size_t maxThreads)
		:maxRow(origin.size()),
		maxCol(origin[0].size()),
		dictionary(dictionary),
		messageQueue(maxRow* maxCol),
		pLatch(nullptr) {

		//init board
		for (size_t i = 0; i < maxRow; i++) {
			for (size_t j = 0; j < maxCol; j++) {
				boggleBoard[i].emplace(j, origin[i][j]);
			}
		}

		// init worker threads;
		for (size_t i = 0; i < maxThreads; i++) {
			messageQueue.sub(std::move([this, i](const Message& message) {
				const BoggleBoard_t& boggleBoard = message.boggleBoard;
				size_t row = message.row;
				size_t col = message.col;
				this->processMessage(boggleBoard, row, col, this->consolidatedWords, i);
				}));
		}
	}

	BoggleBoard(size_t maxRow, size_t maxCol, Trie* dictionary, size_t maxThreads)
		:maxRow(maxRow),
		maxCol(maxCol),
		dictionary(dictionary),
		messageQueue(maxRow* maxCol),
		pLatch(nullptr) {
		RandomizedChar randomizedChar;

		// init board
		for (size_t i = 0; i < maxRow; i++) {
			for (size_t j = 0; j < maxCol; j++) {
				boggleBoard[i].emplace(j, randomizedChar.randomChar());
			}
		}

		// init worker threads;
		for (size_t i = 0; i < maxThreads; i++) {
			messageQueue.sub(std::move([this, i](const Message& message) {
				const BoggleBoard_t& boggleBoard = message.boggleBoard;
				size_t row = message.row;
				size_t col = message.col;
				this->processMessage(boggleBoard, row, col, this->consolidatedWords, i);
				}));
		}
	}

	std::set<std::string> solve() {

		pLatch = std::make_unique<std::latch>(maxRow * maxCol);

		for (int i = 0; i < maxRow; i++) {
			for (int j = 0; j < maxCol; j++) {
				Message aMessage{ boggleBoard , i, j };
				messageQueue.pub(aMessage);
			}
		}

		pLatch.get()->wait();
		// todo: wait until threads are done.

		return consolidatedWords;
	}

	void shutdown() {
		messageQueue.shutdown();
	}

	void dump() {
		for (int i = 0; i < maxRow; i++) {
			for (int j = 0; j < maxCol; j++) {
				std::cout << ' ' << boggleBoard[i][j];
			}
			std::cout << '\n';
		}
	}
};

