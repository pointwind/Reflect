#pragma once
#include<string_view>
#include<tuple>
#include<array>
#include<cstddef>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>


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

//STATIC FUNCTION
//for each macro to creat name
#define SFUNC_MEMBER_NAME MFUNC_MEMBER_NAME

//for each macro to creat function pointer
#define SFUNC_MEMBER_II_1 ACCESS_MEMBER
#define SFUNC_MEMBER_II_2(c,m,t)	\
	static_cast<std::add_pointer_t<(t)>>(ACCESS_MEMBER(c,m))
//reflect all members
#define SFUNC_MEMBER_II(c,...)	\
	APPLY_MACRO(BOOST_PP_OVERLOAD(SFUNC_MEMBER_II_, __VA_ARGS__)(c, __VA_ARGS__))


#define SFUNC_MEMBER_I_0 SFUNC_MEMBER_II
#define SFUNC_MEMBER_I_1 SFUNC_MEMBER_II(c, BOOST_PP_TUPLE_ENUM(t))
#define SFUNC_MEMBER_I(c,t)	\
	BOOST_PP_CAT(SFUNC_MEMBER_I_, BOOST_PP_IS_BEGIN_PARENS(t))(c, t)
#define SFUNC_MEMBER(r, data, i, t) BOOST_PP_COMMA_IF(i) SFUNC_MEMBER_I(data, t)

//entry for static function member
#define SFUNC_PROCESS(CLASS,N,...)	\
static constexpr auto sfun_names() noexcept ->std::array<std::string_view,N>{ \
	 return { BOOST_PP_SEQ_FOR_EACH_I(SFUNC_MEMBER_NAME, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) };} \
static constexpr auto sfunc() noexcept {\
	return std::make_tuple(BOOST_PP_SEQ_FOR_EACH_I(SFUNC_MEMBER, CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)));}\
static constexpr size_t sfunc_count = N;

//static and no-static data
#define DATA_MEMBER_NAME(r,data,i,t) BOOST_PP_COMMA_IF(i) MEMBER_NAME(t)


//constructor 
#define TUPLE_TO_STD_TUPLE(t) std::tuple<BOOST_PP_TUPLE_ENUM(t)>
#define CTOR_LIST_ITEM(r,data,i,t) BOOST_PP_COMMA_IF(i) TUPLE_TO_STD_TUPLE(t)
#define CTOR_PROCESS(class,N,...) \
using ctor_list = std::tuple<BOOST_PP_SEQ_FOR_EACH_I(CTOR_LIST_ITEM,_,BOOST_PP_VARIADICS_TO_SEQ(__VA_ARGS__))>; \
static constexpr size_t ctor_count = N;

/* MAIN entry */
//nomember
#define REFLECT_1(c)												\
template<> struct Reflect_Info<c> {									\
	static constexpr std::string_view name() noexcept { return #c; }\
};

#define REFLECT_2(c, seq)											\
template<> struct Reflect_Info<c> {									\
	static constexpr std::string_view name() noexcept { return #c; }\
BOOST_PP_SEQ_FOR_EACH(APPLY_PROCESS, c, seq)						\
};

#define KATH_REFLECT(...) APPLY_MACRO(BOOST_PP_OVERLOAD(REFLECT_, __VA_ARGS__)(__VA_ARGS__))