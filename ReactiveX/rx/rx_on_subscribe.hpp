#pragma once
#include <functional>
#include "rx_base.hpp"
#include "rx_callback.hpp"

template<typename T>
class FlowableEmitter 
{
public:
	virtual void OnNext(const T &t) {}
	virtual void OnComplete() {}
};

template<typename T>
class FlowableOnSubscribe 
{
public:
	virtual void Subscribe(std::shared_ptr<FlowableEmitter<T>> emitter) 
	{
		if (callback_ != nullptr)
		{
			callback_(emitter);
		}
	}
	void SetSubscribeCallback(const std::function<void(std::shared_ptr<FlowableEmitter<T>>)> &callback)
	{
		callback_ = callback;
	}
protected:
	std::function<void(std::shared_ptr<FlowableEmitter<T>>)> callback_;
};



























