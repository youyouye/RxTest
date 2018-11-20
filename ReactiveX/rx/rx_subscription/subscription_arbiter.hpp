#pragma once
#include <atomic>
#include <memory>
#include "../rx_base.hpp"

class SubscriptionArbiter : public Subscription 
{
public:
	SubscriptionArbiter() 
		:cancel_on_replace_(true)
	{
	}
	SubscriptionArbiter(bool cancel_on_replace)
		:cancel_on_replace_(cancel_on_replace)
	{
	}
	
	void SetSubscription(std::shared_ptr<Subscription> s) 
	{
		if (cancelled_)
		{
			s->Cancel();
			return;
		}
		if (process_num_ == 0)
		{
			process_num_ = 1;
			if (actual_ != nullptr && cancel_on_replace_)
			{
				actual_->Cancel();
			}
			actual_ = s;
			int r = request_num_;
			if ((process_num_ = process_num_-1) != 0)
			{
				DrainLoop();
			}
			if (r != 0)
			{
				s->Request(r);
			}
			return;
		}
		missed_subscription_ = s;
		if (missed_subscription_ != nullptr && cancel_on_replace_)
		{
			missed_subscription_->Cancel();
		}
		Drain();
	}

	void Request(int n) override
	{
		if (process_num_ == 0)
		{
			process_num_ = 1;
			request_num_ += n;
			if ((process_num_ = process_num_ - 1) != 0)
				DrainLoop();
			if (actual_ != nullptr)
			{
				actual_->Request(n);
			}
			return;
		}
		missed_requested_.fetch_add(n);
		Drain();
	}
	
	void Produced(int n) 
	{
		if (process_num_ == 0)
		{
			process_num_ = 1;
			request_num_ = request_num_ - n;
			if ((process_num_ = process_num_ - 1) == 0)
				return;
			DrainLoop();
			return;
		}
		missed_producted_ = n;
		Drain();
	}

	void Cancel() override
	{
		if (!cancelled_)
		{
			cancelled_ = true;
			Drain();
		}
	}

	void Drain() 
	{
		if (process_num_.fetch_add(1) != 0)
			return;
		DrainLoop();
	}

	void DrainLoop() 
	{
		int missed = 1;
		int requestAmount = 0;
		std::shared_ptr<Subscription> requestTarget = nullptr;
		for (;;)
		{
			std::shared_ptr<Subscription> ms = missed_subscription_;
			if (ms != nullptr)
				missed_subscription_ = nullptr;
			int mr = missed_requested_;
			if (mr != 0)
				missed_requested_ = 0;
			int mp = missed_producted_;
			if (mp != 0)
				missed_producted_ = 0;
			std::shared_ptr<Subscription> a = actual_;
			if (cancelled_)
			{
				if (a != nullptr)
				{
					a->Cancel();
					actual_ = nullptr;
				}
				if (ms != nullptr)
				{
					ms->Cancel();
				}
			}
			else
			{
				int v = (mr + request_num_) - mp;
				if (v < 0)
				{
					v = 0;
				}
				request_num_ = v;
				if (ms != nullptr)
				{
					if (a != nullptr && cancel_on_replace_)
					{
						a->Cancel();
					}
					actual_ = ms;
					if (request_num_ != 0) 
					{
						requestAmount = requestAmount + request_num_;
						requestTarget = ms;
					}
				}
				else if (a != nullptr && mr != 0) 
				{
					requestAmount = requestAmount + mr;
					requestTarget = a;
				}
			}
			missed = process_num_ + (-missed);
			if (missed == 0)
			{
				if (requestAmount != 0)
				{
					requestTarget->Request(requestAmount);
				}
				return;
			}
		}
	}

protected:
	std::atomic_int missed_requested_ = 0;
	std::atomic_int missed_producted_ = 0;
	std::shared_ptr<Subscription> missed_subscription_ = nullptr;
	std::shared_ptr<Subscription> actual_ = nullptr;
	std::atomic_int request_num_ = 0;
	std::atomic_int process_num_ = 0;
	bool cancel_on_replace_ = false;
	bool cancelled_ = false;
};





