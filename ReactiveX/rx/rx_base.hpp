#pragma once
#include <memory>
#include <atomic>
#include "rx_common.hpp"

class Error
{
public:
};

class Subscription
{
public:
	virtual ~Subscription() {}
	virtual void Request(int n) = 0;
	virtual void Cancel() = 0;
};

template<typename T>
class Subscriber
{
public:
	virtual ~Subscriber() {}
	virtual void OnSubscribe(std::shared_ptr<Subscription> subscription) = 0;
	virtual void OnNext(const T &t) = 0;
	virtual void OnComplete() = 0;
	virtual void OnError(Error error) = 0;
};

template<typename T>
class Publisher
{
public:
	virtual ~Publisher() {}
	virtual void Subscribe(std::shared_ptr<Subscriber<T>> subscriber) = 0;
};

template<typename T, typename R>
class Processor : public Subscriber<T>, Publisher<R>
{
	virtual ~Processor() {}
};