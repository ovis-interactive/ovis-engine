#pragma once

#include <cstddef>
namespace ovis {

namespace detail {

template <typename T, typename M> T GetMemberPointerClass(M T::*);
template <typename T, typename M> M GetMemberPointerMember(M T::*);

}  // namespace detail

template <auto MEMBER_POINTER>
struct MemberPointer {
  // See: https://stackoverflow.com/questions/12811330/c-compile-time-offsetof-inside-a-template
  using ClassType = decltype(detail::GetMemberPointerClass(MEMBER_POINTER));
  using MemberType = decltype(detail::GetMemberPointerMember(MEMBER_POINTER));
  static const std::size_t offset = reinterpret_cast<std::size_t>(&(((ClassType*)0)->*MEMBER_POINTER));
};

}  // namespace ovis