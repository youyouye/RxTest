#pragma once
#include <queue>
#include <mutex>

template<typename T>
class SafeQueue 
{
public:
	SafeQueue() {}

	void Put(const T& x) 
	{
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.push(std::move(x));
	}

	T Take() 
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (!queue_.empty())
		{
			T value(std::move(queue_.front()));
			queue_.pop();
			return value;
		}
		return T();
	}

	bool Empty() const 
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.empty();
	}
	
	void Clear() const 
	{
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.swap(std::queue<T>());
	}

private:
	std::mutex mutex_;
	std::queue<T> queue_;
};
