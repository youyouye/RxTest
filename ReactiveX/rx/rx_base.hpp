#pragma once
#include <memory>

class Subscription 
{
public:
	virtual ~Subscription() {}
	virtual void Request(long n) {}
	virtual void Cancel() {}
};

template<typename T>
class Subscriber 
{
public:
	virtual ~Subscriber() {}
	virtual void OnSubscribe(std::shared_ptr<Subscription> s) {}
	virtual void OnNext(const T& t) {}
	virtual void OnError() {}
	virtual void OnComplete() {}
};

template<typename T>
class Publisher 
{
public:
	virtual ~Publisher() {}
	virtual void Subscribe(std::shared_ptr<Subscriber<T>> s) {}
};

template<typename T,typename R>
class Processor : public Subscriber<T>,public Publisher<R>
{
	virtual ~Processor() {}
};
