#pragma once
#include<type_traits>
namespace PWL
{
	template < typename T>
	struct negation : std::bool_constant<!T::value> {};

	template < typename T>
	inline constexpr bool negation_v = negation<T>::value;

	struct swallow_t
	{
		template < typename ... T>
		constexpr swallow_t(T&& ... t) noexcept {}

	};

	template <bool Test, typename T = void>
	using disable_if = std::enable_if<negation_v<std::bool_constant<Test>>, T>;

	template <bool Test, typename T = void>
	using disable_if_t = typename disable_if<Test, T>::type;
}