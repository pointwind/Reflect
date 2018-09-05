#pragma once
#include<string_view>
template<typename T>
struct Reflect_Info
{
	static char const* name() noexcept
	{
		return typeid(T).name();
	}
};

#define APPLY_MACRO(macro) macro
#define ACCESS_MEMBER(c,member) &c::member
#define MEMBER_NAME_I(member) std::string_view{#member}
#define MEMBER_NAME(member) MEMBER_NAME_I{member}
namespace PWL {
	template<typename T, typename F>
	struct To_Member_Function_Pointer {};

	template<typename T, typename Ret, typename ...Args>
	struct To_Member_Function_Pointer<T, Ret(Args...)>
	{
		using type = Ret(T::*)(Args...);
	};

	template<typename T, typename Ret, typename ...Args>
	struct To_Member_Function_Pointer<T, Ret(Args...) const>
	{
		using type = Ret(T::*)(Args...) const;
	};

	template<typename T, typename = void>
	struct Is_Reflected : std::false_type {};

	template<typename T>
	struct Is_Reflected <T, std::void_t<decltype(Reflect_Info<T>::name())>> : std::true_type {};
	template<typename T> inline constexpr bool Is_Reflected_V = Is_Reflected<T>::value;
}

//function member withour overload
#define MFUNC_MEMBER_II_1 ACCESS_MEMBER

//with overload
#define MFUNC_MEMBER_II_2(c,m,t)	\
	static_cast<PWL::To_Member_Function_Pointer<c,t>::type>(ACCESS_MEMBER(c,m))

