#include "schedule_manager.h"

std::once_flag ScheduleManager::once_;
std::shared_ptr<ScheduleManager> ScheduleManager::value_ = nullptr;

std::shared_ptr<ScheduleManager> ScheduleManager::Instance()
{
	std::call_once(once_, &ScheduleManager::Init);
	return value_;
}

ScheduleManager::ScheduleManager()
	:running_(false)
{
}

ScheduleManager::~ScheduleManager()
{
	if (running_)
	{
		Stop();
	}
}

void ScheduleManager::Init()
{
	value_ = std::make_shared<ScheduleManager>();
}

void ScheduleManager::RunInThread(int type)
{
	if (init_callback_)
	{
		init_callback_();
	}
	while (running_)
	{
		std::function<void()> task(Take(type));
		if (task)
		{
			task();
		}
	}
}

void ScheduleManager::Start(int numThreads)
{
	running_ = true;
	threads_.reserve(numThreads);
	for (int i = 0;i<numThreads;++i)
	{
		threads_.emplace_back(new std::thread(std::bind(&ScheduleManager::RunInThread,this,0)));
	}
	main_thread_ = std::make_unique<std::thread>(std::bind(&ScheduleManager::RunInThread, this,1));
	io_thread_ = std::make_unique<std::thread>(std::bind(&ScheduleManager::RunInThread,this,2));
}

void ScheduleManager::Stop()
{
	{
		std::unique_lock<std::mutex> lock;
		running_ = false;
		not_empty_.notify_all();
	}
	for (auto& ptr : threads_)
	{
		ptr->join();
	}
	io_thread_->join();
	main_thread_->join();
}

void ScheduleManager::PostThread(const ThreadType& type, std::function<void()> func)
{
	switch (type)
	{
	case ThreadType::k_MainThread:
		{
			std::unique_lock<std::mutex> lock(main_mutex_);
			main_queue_.push_back(std::move(func));
			main_not_empty_.notify_one();
		}
		break;
	case ThreadType::k_IoThread:
		{
			std::unique_lock<std::mutex> lock(io_mutex_);
			io_queue_.push_back(std::move(func));
			io_not_empty_.notify_one();
		}
		break;
	case ThreadType::k_Pool:
		{
			Run(func);
		}
	break;
	}
}

void ScheduleManager::Run(std::function<void()> task)
{
	if (threads_.empty())
	{
		task();
	}
	else 
	{
		std::unique_lock<std::mutex> lock(mutex_);
		queue_.push_back(std::move(task));
		not_empty_.notify_one();
	}
}

void ScheduleManager::IsFull()
{
}

std::function<void()> ScheduleManager::Take(int type)
{
	std::function<void()> task;
	if (type == 0)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		while (queue_.empty() && running_)
		{
			not_empty_.wait(lock);
		}
		if (!queue_.empty())
		{
			task = queue_.front();
			queue_.pop_front();
		}
	}
	else if (type == 1)
	{
		std::unique_lock<std::mutex> lock(main_mutex_);
		while (main_queue_.empty() && running_)
		{
			main_not_empty_.wait(lock);
		}
		if (!main_queue_.empty())
		{
			task = main_queue_.front();
			main_queue_.pop_front();
		}
	}
	else if (type == 2) 
	{
		std::unique_lock<std::mutex> lock(io_mutex_);
		while (io_queue_.empty() && running_)
		{
			io_not_empty_.wait(lock);
		}
		if (!io_queue_.empty())
		{
			task = io_queue_.front();
			io_queue_.pop_front();
		}
	}
	return task;
}
