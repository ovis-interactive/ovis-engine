#pragma once

#include <cstdint>

#include "ovis/utils/not_null.hpp"
#include "ovis/vm/type.hpp"
#include "ovis/vm/type_id.hpp"
#include "ovis/vm/value.hpp"
#include "ovis/vm/virtual_machine.hpp"

namespace ovis {

class List {
 public:
  // 32 bit because the number type in the VM is a double and 32 bit
  // can be represented without less in contrast to 64 bit.
  using SizeType = std::uint32_t;

  List(NotNull<Type*> element_type);
  List(TypeId element_type, NotNull<VirtualMachine*> virtual_machine);

  TypeId element_type() const { return element_type_; }
  SizeType size() const { return size_; }
  SizeType capacity() const { return capacity_; }

  void Reserve(SizeType new_capacity);
  void Resize(SizeType new_size);
  void Add(const Value& value);
  void Remove(SizeType index);

 private:
  TypeId element_type_;
  TypeMemoryLayout memory_layout_;
  SizeType size_ = 0;
  SizeType capacity_ = 0;
  void* data_ = nullptr;
};

}  // namespace ovis
