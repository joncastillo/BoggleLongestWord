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

	std::string joinSet(const std::unordered_set<std::string>& strings, const std::string& delimiter);
	void processMessage(const BoggleBoard_t& boggleBoard, size_t row, size_t col, std::set<std::string>& consolidatedWords, int threadNo);
	void doDfs(const BoggleBoard_t& boggleBoard, VisitedBoard_t& visitedBoard, size_t currentRow, size_t currentCol, std::string& stringBuffer, std::unordered_set<std::string>& consolidatedWords);
	std::vector<std::pair<int, int>> getValidNeighbors(const BoggleBoard_t& boggleBoard, const VisitedBoard_t& visitedBoard, size_t currentRow, size_t currentCol);

public:
	BoggleBoard(BoggleBoard_t& origin, Trie* dictionary, size_t maxThreads);
	BoggleBoard(size_t maxRow, size_t maxCol, Trie* dictionary, size_t maxThreads);
	std::set<std::string> solve();
	void shutdown();
	void dump();
};

