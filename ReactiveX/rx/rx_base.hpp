#pragma once
#include <memory>

class Error 
{
};

class Disposable
{
public:
	virtual void Dispose() = 0;
	virtual bool IsDisposed() = 0;
};

template<typename T>
class Observer 
{
public:
	virtual void OnSubscribe(std::shared_ptr<Disposable> disposable) = 0;
	virtual void OnNext(const T& t) = 0;
	virtual void OnError(const Error& error) = 0;
	virtual void OnComplete() = 0;
};


template<typename T>
class ObservableSource 
{
public:
	virtual void Subscribe(std::shared_ptr<Observer> observer) = 0;
};











