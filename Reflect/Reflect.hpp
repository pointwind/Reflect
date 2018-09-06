#pragma once
#include<string_view>
#include<tuple>
#include<array>
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
//reflect all members
#define MFUNC_MEMBER_II(c,...)	\
	APPLY_MACRO(BOOST_PP_OVERLOAD(MFUNC_MEMBER_II_, __VA_ARGS__)(c, __VA_ARGS__))
// for each macro to create names
#define MEMBER_NAME_0	MEMBER_NAME
#define MEMBER_NAME_1(tuple)	MEMBER_NAME(BOOST_PP_TUPLE_ELEM(0, tuple))
#define MFUNC_MEMBER_NAME(r,data,i,t)	\
	BOOST_PP_COMMA_IF(i) \
	APPLY_MACRO(BOOST_PP_CAT(MEMBER_NAME_, BOOST_PP_IS_BEGIN_PARENS(t))(t))

// for each macro to create pointer to member function
#define MFUNC_MEMBER_I_0	MFUNC_MEMBER_II
#define MFUNC_MEMBER_I_1(c,t)	MFUNC_MEMBER_II(c, BOOST_PP_TUPLE_ENUM(t))
#define MFUNC_MEMBER_I(c, t) \
	BOOST_PP_CAT(MFUNC_MEMBER_I_, BOOST_PP_IS_BEGIN_PARENS(t))(c, t)
#define MFUNC_MEMBER(r, data, i, t) BOOST_PP_COMMA_IF(i) MFUNC_MEMBER_I(data, t)

//entry for function member pointer
#define MFUNC_PROCESS(CLASS,N,...)	\
static constexpr auto mfun_names() noexcept -> std::array<std::string_view,N>{	\
	return { BOOST_PP_SEQ_FOR_EACH_I(MFUNC_MEMBER_NAME, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) };}	\
static constexpr auto mfunc() noexcept{	\
	return std::make_tuple(BOOST_PP_SEQ_FOR_EACH_I(MFUNC_MEMBER, CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))); }		\
static constexpr size_t mfunc_count = N;

