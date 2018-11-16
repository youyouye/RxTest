#pragma once
#include <memory>
#include "../rx_base.hpp"

template<typename>
class LambdaSubscriber : public Subscription
{
public:
	LambdaSubscriber(std::function<void(const T&)> on_next) 
	{
	}

	void SetOnSubscribe() {}

private:
	SubscribeCallback on_subscribe_;
	std::function<void(const T&)> on_next_;
	CompleteCallback on_complete_;
	ErrorCallabck on_error_;
};