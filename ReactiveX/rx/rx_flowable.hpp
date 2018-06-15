#pragma once
#include <vector>
#include "rx_base.hpp"
#include "schedule_manager.h"
#include "rx_on_subscribe.hpp"
#include "rx_operation/flowable_just.hpp"
#include "rx_operation/flowable_subscribe_on.hpp"
#include "rx_operation/flowable_observe_on.hpp"
#include "rx_operation/flowable_concat.hpp"
#include "rx_operation/flowable_from_array.hpp"
#include "rx_operation/flowable_merge.hpp"
#include "rx_operation/flowable_create.hpp"
#include "rx_operation/flowable_flatmap.hpp"

template<typename T>
class Flowable : public Publisher<T>,
	public std::enable_shared_from_this<Flowable<T>>
{
public:
	Flowable() {}
	virtual ~Flowable() {}

	void Subscribe(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		SubscribeActual(subscriber);
	}

	static std::shared_ptr<Flowable<T>> Just(const T& item)
	{
		return std::make_shared<FlowableJust<T>>(item);
	}
	static std::shared_ptr<Flowable<T>> Just(const T& item1, const T& item2)
	{
		return std::make_shared<FlowableJust<T>>(item1,item2);
	}
	static std::shared_ptr<Flowable<T>> Just(const T& item1, const T& item2, const T& item3)
	{
		return std::make_shared<FlowableJust<T>>(item1, item2,item3);
	}
	static std::shared_ptr<Flowable<T>> Just(const T& item1, const T& item2, const T& item3, const T& item4)
	{
		return std::make_shared<FlowableJust<T>>(item1, item2,item3,item4);
	}
	static std::shared_ptr<Flowable<T>> Just(const T& item1, const T& item2, const T& item3, const T& item4, const T& item5)
	{
		return std::make_shared<FlowableJust<T>>(item1, item2,item3,item4,item5);
	}

	
	std::shared_ptr<Flowable<T>> SubscribeOn(const ThreadType &type)
	{
		return std::make_shared<FlowableSubscribeOn<T>>(shared_from_this(),type);
	}

	std::shared_ptr<Flowable<T>> ObserveOn(const ThreadType &type) 
	{
		return std::make_shared<FlowableObserveOn<T>>(shared_from_this(),type);
	}

	static std::shared_ptr<Flowable<T>> FromArray(const T& item1,const T& item2) 
	{
		return std::make_shared<FlowableFromArray<T>>(std::vector<T>{item1,item2});
	}
	static std::shared_ptr<Flowable<T>> FromArray(const T& item1, const T& item2,
		const T& item3)
	{
		return std::make_shared<FlowableFromArray<T>>(std::vector<T>{item1, item2,item3});
	}
	static std::shared_ptr<Flowable<T>> FromArray(const T& item1, const T& item2,
		const T& item3,const T& item4)
	{
		return std::make_shared<FlowableFromArray<T>>(std::vector<T>{item1, item2, item3,item4});
	}
	static std::shared_ptr<Flowable<T>> FromArray(const T& item1, const T& item2,
		const T& item3, const T& item4, const T& item5)
	{
		return std::make_shared<FlowableFromArray<T>>(std::vector<T>{item1, item2, item3,item4,item5});
	}
	//item
	/*
	static std::shared_ptr<Flowable<T>> FromArray(std::shared_ptr<Flowable<T>> item1, std::shared_ptr<Flowable<T>> item2)
	{
		return std::make_shared<FlowableFromArray<T>>(std::vector<std::shared_ptr<Flowable<T>>>{item1, item2});
	}
	static std::shared_ptr<Flowable<T>> FromArray(std::shared_ptr<Flowable<T>> item1, std::shared_ptr<Flowable<T>> item2,
		std::shared_ptr<Flowable<T>> item3)
	{
		return std::make_shared<FlowableFromArray<T>>(std::vector<std::shared_ptr<Flowable<T>>>{item1, item2, item3});
	}
	*/

	static std::shared_ptr<Flowable<T>> Concat(std::shared_ptr<Flowable<T>> source1, std::shared_ptr<Flowable<T>> source2)
	{
		return std::make_shared<FlowableConcat<T>>(std::vector<std::shared_ptr<Flowable<T>>>{source1, source2});
	}
	static std::shared_ptr<Flowable<T>> Concat(std::shared_ptr<Flowable<T>> source1, std::shared_ptr<Flowable<T>> source2,
		std::shared_ptr<Flowable<T>> source3)
	{
		return std::make_shared<FlowableConcat<T>>(std::vector<std::shared_ptr<Flowable<T>>>{source1, source2, source3});
	}
	static std::shared_ptr<Flowable<T>> Concat(std::shared_ptr<Flowable<T>> source1, std::shared_ptr<Flowable<T>> source2,
		std::shared_ptr<Flowable<T>> source3, std::shared_ptr<Flowable<T>> source4)
	{
		return std::make_shared<FlowableConcat<T>>(std::vector<std::shared_ptr<Flowable<T>>>{source1, source2, source3, source4});
	}
	static std::shared_ptr<Flowable<T>> Concat(std::shared_ptr<Flowable<T>> source1, std::shared_ptr<Flowable<T>> source2,
		std::shared_ptr<Flowable<T>> source3, std::shared_ptr<Flowable<T>> source4, std::shared_ptr<Flowable<T>> source5)
	{
		return std::make_shared<FlowableConcat<T>>(std::vector<std::shared_ptr<Flowable<T>>>{source1, source2, source3, source4, source5});
	}
	static std::shared_ptr<Flowable<T>> Merge(std::shared_ptr<Flowable<T>> source1, std::shared_ptr<Flowable<T>> source2)
	{
		return std::make_shared<FlowableMerge<T>>(std::vector<std::shared_ptr<Flowable<T>>>{source1,source2});
	}
	static std::shared_ptr<Flowable<T>> Merge(std::shared_ptr<Flowable<T>> source1, std::shared_ptr<Flowable<T>> source2,
		std::shared_ptr<Flowable<T>> source3)
	{
		return std::make_shared<FlowableMerge<T>>(std::vector<std::shared_ptr<Flowable<T>>>{source1, source2,source3});
	}
	static std::shared_ptr<Flowable<T>> Merge(std::shared_ptr<Flowable<T>> source1, std::shared_ptr<Flowable<T>> source2,
		std::shared_ptr<Flowable<T>> source3, std::shared_ptr<Flowable<T>> source4)
	{
		return std::make_shared<FlowableMerge<T>>(std::vector<std::shared_ptr<Flowable<T>>>{source1, source2, source3,source4});
	}
	static std::shared_ptr<Flowable<T>> Merge(std::shared_ptr<Flowable<T>> source1, std::shared_ptr<Flowable<T>> source2,
		std::shared_ptr<Flowable<T>> source3, std::shared_ptr<Flowable<T>> source4, std::shared_ptr<Flowable<T>> source5)
	{
		return std::make_shared<FlowableMerge<T>>(std::vector<std::shared_ptr<Flowable<T>>>{source1, source2, source3, source4,source5});
	}
	static std::shared_ptr<Flowable<T>> Create(std::shared_ptr<FlowableOnSubscribe<T>> source) 
	{
		return std::make_shared<FlowableCreate<T>>(source);
	}

	template<typename R>
	std::shared_ptr<Flowable<R>> FlatMap(const std::function<std::shared_ptr<Flowable<R>>(const T& item)>& mapper) 
	{
		auto self = shared_from_this();
		return std::make_shared<FlowableFlatMap<T,R>>(self,mapper);
	}

protected:
	virtual void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) {}
};