#pragma once
#include "rx_base.hpp"

template<typename T>
class Observable : public ObservableSource<T> 
{
public:
	void Subscribe(std::shared_ptr<Observer> observer) 
	{
		SubscribeActual(observer);
	}

	virtual void SubscribeActual(std::shared_ptr<Observer> observer)= 0;

	static std::shared_ptr<Observable> Just(const T& item) 
	{
		
	}


};