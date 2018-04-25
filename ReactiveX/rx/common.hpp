#pragma once
#include <memory>

template<typename T>
struct enable_shared_from_this_virtual;

class enable_shared_from_this_virtual_base : public std::enable_shared_from_this<enable_shared_from_this_virtual_base>
{
	typedef std::enable_shared_from_this<enable_shared_from_this_virtual_base> base_type;
	template<typename T>
	friend struct enable_shared_from_this_virtual;

	std::shared_ptr<enable_shared_from_this_virtual_base> shared_from_this()
	{
		return base_type::shared_from_this();
	}
	std::shared_ptr<enable_shared_from_this_virtual_base const> shared_from_this() const
	{
		return base_type::shared_from_this();
	}
};

template<typename T>
struct enable_shared_from_this_virtual : virtual enable_shared_from_this_virtual_base
{
	typedef enable_shared_from_this_virtual_base base_type;

public:
	std::shared_ptr<T> shared_from_this()
	{
		std::shared_ptr<T> result(base_type::shared_from_this(), static_cast<T*>(this));
		return result;
	}

	std::shared_ptr<T const> shared_from_this() const
	{
		std::shared_ptr<T const> result(base_type::shared_from_this(), static_cast<T const*>(this));
		return result;
	}
};

template<std::size_t I = 0, typename FuncT, typename... Tp>
static inline typename std::enable_if<I == sizeof...(Tp), void>::type
for_each(std::tuple<Tp...> &, FuncT) // Unused arguments are given no names.
{ }

template<std::size_t I = 0, typename FuncT, typename... Tp>
static inline typename std::enable_if < I < sizeof...(Tp), void>::type
	for_each(std::tuple<Tp...>& t, FuncT f)
{
	f(std::get<I>(t));
	for_each<I + 1, FuncT, Tp...>(t, f);
}

