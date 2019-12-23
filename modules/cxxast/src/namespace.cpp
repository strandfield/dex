
#include "cxx/namespace.h"

namespace cxx
{

const std::string Namespace::TypeId = "namespace";

const std::string& Namespace::type() const
{
  return TypeId;
}

} // namespace cxx
