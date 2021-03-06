#pragma once

#include <memory>

template<typename T>
using UniquePointer = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr UniquePointer<T> CreateUniquePointer(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using SharedPointer = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr SharedPointer<T> CreateSharedPointer(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using WeakPointer = std::weak_ptr<T>;
