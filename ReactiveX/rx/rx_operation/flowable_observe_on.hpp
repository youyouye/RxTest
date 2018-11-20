#pragma once
#include <atomic>
#include "../rx_base.hpp"
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

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) 
	{
		auto observe_on_subscriber = std::make_shared<ObserveOnSubscriber<T>>(subscriber,type_);
		source_->Subscribe(observe_on_subscriber);
	}

private:
	std::shared_ptr<Flowable<T>> source_;
	ThreadType type_;
};

template<typename T>
class ObserveOnSubscriber : public Subscriber<T> ,public Subscription,
	public std::enable_shared_from_this<ObserveOnSubscriber<T>>
{
public:
	ObserveOnSubscriber(std::shared_ptr<Subscriber<T>> actual,ThreadType type) 
		:downstream_(actual),type_(type)
	{
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		upstream_ = subscription;
		auto self = shared_from_this();
		downstream_->OnSubscribe(self);
		subscription->Request(128);
	}
	void OnNext(const T &t) override
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
	void OnError(std::shared_ptr<Error> error) override
	{
		if (done_)
			return;
		error_ = error;
		done_ = true;
		TrySchedule();
	}
	void Request(int n) override
	{
		request_num_ += n;
		TrySchedule();
	}
	void Cancel() override
	{
		if (cancelled_)
			return;
		cancelled_ = true;
		upstream_->Cancel();
		if (process_num_.fetch_add(1) != 0)
			queue_.Clear();
	}

	void TrySchedule() 
	{
		if (process_num_.fetch_add(1) != 0)
			return;
		auto self = shared_from_this();
		ScheduleManager::Instance()->PostThread(type_, [self]() {
			self->RunAsync();
		});
	}

	void RunAsync() 
	{
		int missed = 1;
		int e = producted_;
		for (;;)
		{
			int r = request_num_;
			while (e != r)
			{
				bool done = done_;
				bool empty = queue_.Empty();
				if (CheckTerminated(done, empty, downstream_))
					return;
				if (empty)
					break;
				downstream_->OnNext(queue_.Take());
				e++;
			}
			if (e == r && CheckTerminated(done_,queue_.Empty(),downstream_))
				return;
			if (missed == process_num_)
			{
				producted_ = e;
				missed = process_num_.fetch_add(-missed);
				if (missed == 0)
					break;
			}
			else 
			{
				missed = process_num_;
			}
		}
	}

	bool CheckTerminated(bool done,bool empty,std::shared_ptr<Subscriber<T>> actual) 
	{
		if (cancelled_)
		{
			queue_.Clear();
			return true;
		}
		if (done)
		{
			if (error_ != nullptr)
			{
				cancelled_ = true;
				queue_.Clear();
				actual->OnError(error_);
				return true;
			}
			else if (empty) 
			{
				cancelled_ = true;
				actual->OnComplete();
				return true;
			}
		}
		return false;
	}

private:
	ThreadType type_;
	std::shared_ptr<Subscriber<T>> downstream_ = nullptr;
	std::shared_ptr<Subscription> upstream_ = nullptr;
	std::atomic_bool done_ = false;
	std::atomic_int request_num_ = 0;
	std::atomic_int producted_ = 0;
	std::atomic_bool cancelled_ = false;
	std::shared_ptr<Error> error_ = nullptr;
	SafeQueue<T> queue_;
	std::atomic_int process_num_ = 0;
};
