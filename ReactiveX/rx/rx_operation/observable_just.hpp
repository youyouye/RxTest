#pragma once
#include "..\rx_observable.hpp"

template<typename T>
class Observable;

template<typename T>
class ObservableJust : public Observable<T>
{
public:
	ObservableJust(const T& value) 
		:value_(value)
	{
	}

	void SubscribeActual(std::shared_ptr<Observer> observer) 
	{
		
	}

private:
	T value_;
};