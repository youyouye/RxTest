#pragma once
#include "rx_base.hpp"

template<typename T>
class FlowableSubscriber : public Subscriber<T>
{
public:
	void OnNext(const T& t) override
	{
		if (on_next_ != nullptr)
			on_next_(t);
	}
	void OnError() override
	{
		if (on_error_ != nullptr)
			on_error_();
	}
	void OnComplete() override
	{
		if (on_complete_ != nullptr)
			on_complete_();
	}
	void SetOnNext(std::function<void(const T&)> func) 
	{
		on_next_ = func;
	}
	void SetOnError(std::function<void()> func) 
	{
		on_error_ = func;
	}
	void SetOnComlete(std::function<void()> func) 
	{
		on_complete_ = func;
	}
public:
	std::function<void(const T&)> on_next_;
	std::function<void()> on_error_;
	std::function<void()> on_complete_;
};