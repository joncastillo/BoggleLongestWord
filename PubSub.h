#pragma once

#include <queue>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <thread>

/**
*	Pub Sub Message Queue implementation
*/
template <typename T>
class PubSub {
	std::condition_variable cvPublish;
	std::condition_variable cvSubscribe;
	std::mutex mutexQueue;

	std::queue<T> queueMessage;

	size_t maxQueueSize;
	bool flagShutdown;

	std::vector<std::thread> containerThread;

	void enqueue(const T& message) {
		std::unique_lock<std::mutex> lock{ mutexQueue };
		cvPublish.wait(lock, [this]() {return queueMessage.size() < maxQueueSize || flagShutdown; });

		if (flagShutdown) {
			throw std::runtime_error("PubSub is shutdown.");
		}

		if (queueMessage.size() == maxQueueSize) {
			throw std::runtime_error("spurious wakeup.");
		}

		queueMessage.push(message);
		cvSubscribe.notify_all();
	}

	T poll() {
		std::unique_lock<std::mutex> lock{ mutexQueue };
		cvSubscribe.wait(lock, [this]() {return queueMessage.size() > 0 || flagShutdown; });

		if (flagShutdown) {
			throw std::runtime_error("PubSub is shutdown.");
		}

		if (queueMessage.empty()) {
			throw std::runtime_error("spurious wakeup.");
		}

		T message = std::move(queueMessage.front());
		queueMessage.pop();

		cvPublish.notify_all();
		return message;
	}
public:
	PubSub(size_t maxQueueSize)
		: maxQueueSize(maxQueueSize),
		flagShutdown(false) {}

	/*
	* Message publisher
	*/
	void pub(const T& message) {
		try {
			enqueue(message);
		}
		catch (std::runtime_error& e) {
		}
	}

	/*
	* Threadable subscribe method.
	* Awaits for message in Queue and process it using messageProcessor
	*/
	void sub(std::function<void(const T& message)> messageProcessor) {
		flagShutdown = false;
		containerThread.emplace_back([this, messageProcessor]() {
			try {
				while (!flagShutdown) {
					T message = poll();
					messageProcessor(message);
				}
			}
			catch (std::runtime_error& e) {
			}
			});

	}

	void shutdown() {
		flagShutdown = true;
		cvPublish.notify_all();
		cvSubscribe.notify_all();
		for (auto& thread : containerThread) {
			if (thread.joinable()) {
				thread.join();
			}
		}
		containerThread.clear();
	}

};