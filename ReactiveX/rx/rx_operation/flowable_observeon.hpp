#pragma once
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"

template<typename T>
class Flowable;

template<typename T>
class FlowableObserveOn : public Flowable<T>
{
public:
	FlowableObserveOn(std::shared_ptr<Flowable<T>> source,ThreadType type) 
		:source_(source),type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		source_->Subscribe(std::make_shared<ObserveOnSubscriber<T>>());
	}

private:
	std::shared_ptr<Flowable<T>> source_;
	ThreadType type_;
};

template<typename T>
class ObserveOnSubscriber : public Subscriber<T>,public Subscription,
	public std::enable_shared_from_this<ObserveOnSubscriber<T>>
{
public:
	ObserveOnSubscriber(std::shared_ptr<Subscriber<T>> actual,ThreadType type) 
		:actual_(actual),type_(type)
	{
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) override 
	{
		subscription_ = subscription;
		actual_->OnSubscribe(shared_from_this());
	}

	void OnNext(const T& t) override 
	{
		if (done_)
			return;
		queue_.Put(t);
		TrySchedule();
	}

	void OnComplete() override
	{
		if (!done_)
		{
			done_ = true;
			TrySchedule();
		}
	}

	void Request(long n) override 
	{
		request_num_.fetch_add(n);
		TrySchedule();
	}

	void Cancel() override 
	{
		if (cancel_)
			return;
		cancel_ = true;
		subscription_->Cancel();
		//dispose
		if (process_num_.fetch_add(1) == 0)
		{
			queue_.Clear();
		}
	}

	void TrySchedule()
	{
		if (process_num_.fetch_add(1) != 0)
			return;
		ScheduleManager::Instance()->PostThread(type_, &ObserveOnSubscriber::RunInThread, shared_from_this());
	}

	void RunInThread() 
	{
		RunAsync();
	}

	void RunSync() 
	{
		int missed = 1;
		int e = produced_;
		for (;;)
		{
			int current = request_num_.load();
			while ( e != current)
			{
				if (cancel_)
					return;
				bool empty = queue_.Empty();
				if (empty)
				{
					actual_->OnComplete();
					//dispose
					return;
				}
				T value;
				if (!empty)
				{
					value = queue_.Take();
				}
				actual_->OnNext(value);
			}
			if (cancel_)
				return;
			if (queue_.Empty())
			{
				actual_->OnComplete();
				//dispose
				return;
			}
			int process = process_num_.load();
			if (missed == process)
			{
				produced_ = e;
				break;
			}
			else {
				missed = process;
			}
		}
	}
	
	void RunAsync() 
	{
		int missed = 1;
		int e = produced_;
		for (;;)
		{
			int current = request_num_.load();
			while (e != current)
			{
				bool empty = queue_.Empty();
				if (CheckTerminated(done_,empty))
					return;
				if (empty)
					break;
				T value = queue_.Take();
				actual_->OnNext(value);
				e++;
			}
			if (e == current && CheckTerminated(done_, queue_.Empty()))
				return;
			int process = process_num_.load();
			if (missed == process)
			{
				produced_ = e;
				break;
			}
		}
	}

	bool CheckTerminated(bool done,bool empty) 
	{
		if (cancel_)
		{
			queue_.Clear();
			return true;
		}
		if (done)
		{
			if (empty)
			{
				actual_->OnComplete();
				//dispose
				return true;
			}
		}
		return false;
	}

private:
	std::shared_ptr<Subscriber<T>> actual_;
	std::shared_ptr<Subscription> subscription_;
	ThreadType type_;
	bool done_ = false;
	bool cancel_ = false;
	SafeQueue<T> queue_;
	std::atomic_int process_num_ = 0;
	std::atomic_int request_num_ = 0;

	int produced_ = 0;
};

