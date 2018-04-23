#pragma once
#include <memory>
#include <functional>
#include <vector>
#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>

enum ThreadType
{
	k_MainThread = 1,
	k_IoThread,
	k_Pool,
};

class ScheduleManager
{
public:
	ScheduleManager();
	~ScheduleManager();

	static std::shared_ptr<ScheduleManager> Instance();

	static void Init();

	void SetThreadInitCallback(const std::function<void()>& cb)
	{
		init_callback_ = cb;
	}
	void RunInThread(int type);
	void Start(int numThreads);
	void Run(std::function<void()> task);
	void PostThread(const ThreadType& type, std::function<void()> func);
	void Stop();
	void IsFull();
	std::function<void()> Take(int type);
private:
	mutable std::mutex mutex_;
	std::condition_variable  not_empty_;
	std::vector<std::unique_ptr<std::thread>> threads_;
	std::deque<std::function<void()>> queue_;
	
	mutable std::mutex io_mutex_;
	std::condition_variable  io_not_empty_;
	mutable std::mutex main_mutex_;
	std::condition_variable  main_not_empty_;
	std::unique_ptr<std::thread> io_thread_;
	std::unique_ptr<std::thread> main_thread_;
	std::deque<std::function<void()>> io_queue_;
	std::deque<std::function<void()>> main_queue_;

	std::function<void()> init_callback_;
	bool running_;

	static std::once_flag once_;
	static std::shared_ptr<ScheduleManager> value_;
};