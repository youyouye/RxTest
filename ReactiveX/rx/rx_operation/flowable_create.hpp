#pragma once
#include <atomic>
#include <mutex>
#include "../safe_queue.hpp"
#include "../rx_flowable.hpp"

enum class BackPressureStrategy;
template<typename T>
class Flowable;

template<typename T>
class FlowableCreate : public Flowable<T>
{
public:
	FlowableCreate(std::shared_ptr<FlowableOnSubscribe<T>> source,BackPressureStrategy backpressure) 
		:source_(source),backpressure_(backpressure)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		std::shared_ptr<BaseEmitter<T>> emitter;
		subscriber->OnSubscribe(emitter);
		source_->Subscribe(emitter);
	}

private:
	std::shared_ptr<FlowableOnSubscribe<T>> source_;
	BackPressureStrategy backpressure_;
};

template<typename T>
class BaseEmitter : public FlowableEmitter<T>,public Subscription,
	public std::enable_shared_from_this<BaseEmitter<T>>
{
public:
	BaseEmitter(std::shared_ptr<Subscriber<T>> actual) 
		:actual_(actual)
	{
	}

	void Request(long n) override 
	{
		for (;;)
		{
			int r = request_num_.load();
			int u = r + n;
			if (request_num_.compare_exchange_strong(r,u))
			{
				break;
			}
		}
		OnRequested();
	}

	void OnRequested(){}

	void Cancel() override
	{
		if (cancel_flag_.compare_exchange_strong(false,true))
		{
			OnUnsubscribed()
		}
	}
	
	void OnUnsubscribed() {}

	void SetCancellable(CancelCallback callback) override
	{
	}
	
	int Requested() override
	{
		return request_num_.load();
	}
	
	bool IsCancelled() override
	{
		return cancel_flag_.load();
	}
	
	std::shared_ptr<FlowableEmitter<T>> Serialize() override
	{
		return std::make_shared<SerializedEmitter<T>>(shared_from_this());
	}
	
	void OnNext(const T& value) override
	{
	}
	
	void OnError() override
	{
	} 
	
	void OnComplete() override
	{
		Complete();
	}
	void Complete() 
	{
		if (IsCancelled())
			return;
		actual_->OnComplete();
	}

protected:
	std::shared_ptr<Subscriber<T>> actual_;
	std::atomic_int request_num_ = 0;
	std::atomic_bool cancel_flag_ = false;
};

template<typename T>
class SerializedEmitter : public FlowableEmitter<T>
{
	SerializedEmitter(std::shared_ptr<BaseEmitter<T>> emitter)
		:emitter_(emitter)
	{
	}
	
	void OnNext(const T& value) override
	{
		if (emitter_->IsCancelled() || done_)
			return;
		if (access_num_.load() == 0 && access_num_.compare_exchange_strong(0,1))
		{
			emitter_->OnNext(value);
			if ((access_num_-1) == 0)
				return;
		}
		else 
		{
			queue_.Put(value);
			if ((access_num_.fetch_add(1)) != 0)
				return;
		}
		DrainLoop();
	}
	
	void OnComplete() override
	{
		if (emitter_->IsCancelled() || done_)
			return;
		done_ = true;
		Drain();
	}

	void Drain() 
	{
		if (access_num_.fetch_add(1) == 0)
		{
			DrainLoop();
		}
	}

	void DrainLoop() 
	{
		for (;;)
		{
			if (emitter_->IsCancelled())
			{
				queue_.Clear();
				return;
			}
			T value;
			bool empty = queue_.Empty();
			if (!empty)
			{
				value = queue_.Take();
			}
			if (done_ && empty)
			{
				emitter_->OnComplete();
				return;
			}
			if (empty)
			{
				break;
			}
			emitter_->OnNext(value);
		}
	}
	
	int Requested() override 
	{
		return emitter_->Requested();
	}

	bool IsCancelled() 
	{
		return emitter_->IsCancelled();
	}

private:
	std::shared_ptr<BaseEmitter<T>> emitter_;
	bool done_ = false;
	std::atomic_int access_num_ = 0;
	SafeQueue<T> queue_;
};

template<typename T>
class BufferAsyncEmitter : public BaseEmitter<T> 
{
public:
	BufferAsyncEmitter(std::shared_ptr<Subscriber<T>> actual,int capacity_hint) 
		:BaseEmitter<T>(actual)
	{
	}

	void OnNext(const T& t) override
	{
		if (done_ || IsCancelled())
			return;
		queue_.Put(t);
		Drain();
	}

	void OnComplete() override
	{
		done_ = true;
		Drain();
	}

	void OnRequested() override
	{
		Drain();
	}

	void OnUnsubscribed() override
	{
		if (process_num_.fetch_add(1) == 0)
		{
			queue_.Clear();
		}
	}

	void Drain() 
	{
		if (process_num_.fetch_add(1) != 0)
			return;
		int missed = 1;
		for (;;)
		{
			int request_num = request_num_.load();
			int e = 0;
			while (request_num != e)
			{
				if (IsCancelled())
				{
					queue_.Clear();
					return;
				}
				bool empty = queue_.Empty();
				T value;
				if (!empty)
				{
					value = queue_.Take();
				}
				if (done_ && empty)
				{
					Complete();
					return;
				}
				if (empty)
					break;
				actual_->OnNext(value);
				e++;
			}
			if (e == request_num)
			{
				if (IsCancelled())
				{
					queue_.Clear();
					return;
				}
				if (done_ && queue_.empty())
				{
					Complete();
					return;
				}
			}
			if (e != 0)
			{
				for (;;)
				{
					int current = request_num_.load();
					int update = current - e;
					if (request_num_.compare_exchange_strong(current, update))
						break;
				}
			}
			missed = process_num_ + (-missed);
			if (missed == 0)
				break;
		}
	}


private:
	SafeQueue<T> queue_;
	std::atomic_int process_num_ = 0;
	bool done_;
};