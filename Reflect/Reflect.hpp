#pragma once
#include<string_view>
#include<tuple>
#include<array>
#include<type_traits>
#include<cstddef>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include<boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp> 
#include<boost/preprocessor/tuple/enum.hpp> 
#include"mpl.h"
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
	template<typename T> constexpr bool Is_Reflected_V = Is_Reflected<T>::value;
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
static constexpr auto mfun_names() -> std::array<std::string_view,N>{	\
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
#define DATA_MEMBER(r,data,i,t)	BOOST_PP_COMMA_IF(i) &dat::t
//no-static member
#define MDATA_PROCESS(CLASS,N,...)	\
static constexpr auto mdata_names() noexcept ->std::array<std::string_view,N>{ \
	 return { BOOST_PP_SEQ_FOR_EACH_I(DATA_MEMBER_NAME, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) };} \
static constexpr auto mdata() noexcept {\
	return std::make_tuple(BOOST_PP_SEQ_FOR_EACH_I(DATA_MEMBER, CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)));}\
static constexpr size_t mdata_count = N;
//static member
#define SDATA_PROCESS(CLASS,N,...)	\
static constexpr auto sdata_names() noexcept ->std::array<std::string_view,N>{ \
	 return { BOOST_PP_SEQ_FOR_EACH_I(DATA_MEMBER_NAME, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) };} \
static constexpr auto sdata() noexcept { \
	return std::make_tuple(BOOST_PP_SEQ_FOR_EACH_I(DATA_MEMBER, CLASS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)));}\
static constexpr size_t sdata_count = N;


//constructor 
#define TUPLE_TO_STD_TUPLE(t) std::tuple<BOOST_PP_TUPLE_ENUM(t)>
#define CTOR_LIST_ITEM(r,data,i,t) BOOST_PP_COMMA_IF(i) TUPLE_TO_STD_TUPLE(t)
#define CTOR_PROCESS(class,N,...) \
using ctor_list = std::tuple<BOOST_PP_SEQ_FOR_EACH_I(CTOR_LIST_ITEM,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))>; \
static constexpr size_t ctor_count = N;

//dispatch
#define MDATA(...) ((MDATA_PROCESS(__VA_ARGS__)))
#define SDATA(...) ((SDATA_PROCESS(__VA_ARGS__)))
#define MFUNC(...) ((MFUNC_PROCESS(__VA_ARGS__)))
#define SFUNC(...) ((SFUNC_PROCESS(__VA_ARGS__)))
#define CTOR(...) ((CTOR_PROCESS(__VA_ARGS__)))
#define APPLY_PROCESS_II(c,p,n,...) p(c,n,__VA_ARGS__)
#define APPLY_PROCESS_I(c,p,params)\
APPLY_PROCESS_II(c,p,BOOST_PP_TUPLE_SIZE(params),BOOST_PP_TUPLE_ENUM(params))
#define APPLY_PROCESS(r,data,tuple)\
APPLY_PROCESS_I(data,BOOST_PP_TUPLE_ELEM(0,tuple),BOOST_PP_TUPLE_ELEM(1,tuple))

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

#define REFLECT(...) APPLY_MACRO(BOOST_PP_OVERLOAD(REFLECT_, __VA_ARGS__)(__VA_ARGS__))



#define TMP_HAS(CAT)													\
template <typename T, typename = void>                                  \
struct BOOST_PP_CAT(has_reflect_, CAT) : std::false_type{};             \
template <typename T>                                                   \
struct BOOST_PP_CAT(has_reflect_, CAT)<T, std::void_t<                  \
    decltype(T::BOOST_PP_CAT(CAT, _names)()),                           \
    decltype(T::BOOST_PP_CAT(CAT, BOOST_PP_EMPTY())())                  \
>> : std::true_type{};                                                  \
template <typename T, typename = void>                                  \
struct BOOST_PP_CAT(has_visit_, CAT) : std::false_type{};               \
template <typename T>                                                   \
struct BOOST_PP_CAT(has_visit_, CAT)<T, std::void_t<                    \
    decltype(std::declval<T>().BOOST_PP_CAT(visit_, CAT)(               \
        std::declval<std::string_view>(),                               \
        std::declval<int>(),                                            \
        std::declval<size_t>()                                          \
    ))>> : std::true_type{};

#define TMP_VISIT(CAT)													\
template <typename T, typename V>                                       \
inline static auto BOOST_PP_CAT(visit_, CAT)(Reflect_Info<T>, V const& visitor) noexcept \
-> std::enable_if_t<std::conjunction<                                   \
    BOOST_PP_CAT(has_reflect_, CAT)<Reflect_Info<T>>,                   \
    BOOST_PP_CAT(has_visit_, CAT)<V>>::value> {                         \
    using reflect_into_t = Reflect_Info<T>;                             \
    constexpr size_t size = reflect_into_t::BOOST_PP_CAT(CAT, _count);  \
    PWL::reflect_detail::visit_loop(                                    \
        reflect_into_t::BOOST_PP_CAT(CAT, _names)(),                    \
        reflect_into_t::BOOST_PP_CAT(CAT, BOOST_PP_EMPTY())(),          \
        [&visitor](auto ... args) {                                     \
            visitor.BOOST_PP_CAT(visit_, CAT)(std::forward<decltype(args)>(args)...); }, \
        std::make_index_sequence<size>{});                              \
}                                                                       \
template <typename T, typename V>                                       \
inline static auto BOOST_PP_CAT(visit_, CAT)(Reflect_Info<T>, V const& visitor) noexcept \
-> PWL::disable_if_t<std::conjunction<                                       \
    BOOST_PP_CAT(has_reflect_, CAT)<Reflect_Info<T>>,                   \
    BOOST_PP_CAT(has_visit_, CAT)<V>>::value>{}


#define VISIT_SEQ (mdata)(sdata)(mfunc)(sfunc)
#define VISIT_PROC(r,data,elem) BOOST_PP_CAT(visit_,elem)(ri,visitor);
#define TMP_HAS_PROC(r,data,elem) TMP_HAS(elem)
#define TMP_VISIT_PROC(r,data,elem) TMP_VISIT(elem)


namespace PWL
{

	BOOST_PP_SEQ_FOR_EACH(TMP_HAS_PROC, _, VISIT_SEQ)

	template<typename T,typename = void>
	struct has_ctor_list : std::false_type{};
	template<typename T>
	struct has_ctor_list< T, std::void_t<
		typename T::ctor_list>> : std::true_type{};

	template<typename T, typename = void>
	struct has_visit_ctor : std::false_type {};
	template<typename T>
	struct has_visit_ctor< T, std::void_t<
		decltype(&T::visit_ctor)>> : std::true_type{};
	namespace reflect_detail
	{
		template <typename Arr, typename Tuple, typename F, size_t ... Is>
		inline static void visit_loop(Arr const& names, Tuple const& members, F const& func, std::index_sequence<Is...>)
		{
			PWL::swallow_t{
				(func(names[Is], std::get<Is>(members), Is, names), true) ...
			};
		}

		BOOST_PP_SEQ_FOR_EACH(TMP_VISIT_PROC, _,VISIT_SEQ)

		template <typename T, typename V>
		inline static void visit(Reflect_Info<T> ri, V const& visitor) noexcept
		{

			BOOST_PP_SEQ_FOR_EACH(VISIT_PROC, _, VISIT_SEQ)

		}

	}
	template<typename T,typename V>
	inline static void reflect_visit(V const& visitor) noexcept
	{
		reflect_detail::visit(Reflect_Info<std::remove_const_t<T>>{}, visitor);
	}
	template<typename T, typename V>
	inline static void reflect_visit(Reflect_Info<T> ri,V const& visitor) noexcept
	{
		reflect_detail::visit(ri, visitor);
	}
}

namespace PWL
{
	template<typename T>
	inline static constexpr auto reflect()
	{
		static_assert(PWL::Is_Reflected_V<T>);
		return Reflect_Info<T>{};
	}
}

#define reflexpr(x) Reflect_Info<std::remove_const_t<x>>