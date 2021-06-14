// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_PROGRAM_H
#define DEX_MODEL_PROGRAM_H

#include "dex/dex-model.h"

#include "dex/model/model-base.h"
#include "dex/model/since.h"
#include "dex/model/document.h"

#include <algorithm>
#include <optional>
#include <unordered_map>
#include <vector>

namespace dex
{

class Class;
class Enum;
class Function;

using Type = std::string;
using Expression = std::string;
using Name = std::string;

enum class AccessSpecifier
{
  PUBLIC,
  PROTECTED,
  PRIVATE,
};

DEX_MODEL_API std::string to_string(dex::AccessSpecifier as);

class DEX_MODEL_API Entity : public model::Object
{
public:
  std::string name;
  std::weak_ptr<Entity> weak_parent;
  std::optional<std::string> brief;
  std::optional<Since> since;
  std::shared_ptr<dex::Document> description;

public:
  explicit Entity(std::string name, std::shared_ptr<Entity> parent = nullptr);

  std::shared_ptr<Entity> shared_from_this();
  std::shared_ptr<const Entity> shared_from_this() const;

  bool isProgramEntity() const override;

  std::shared_ptr<Entity> parent() const;

  virtual AccessSpecifier getAccessSpecifier() const;
  virtual void setAccessSpecifier(AccessSpecifier aspec);

  template<typename T>
  static std::shared_ptr<T> find(const std::string& name, const std::vector<std::shared_ptr<T>>& entities);
};

inline Entity::Entity(std::string n, std::shared_ptr<Entity> parent)
  : name(std::move(n)),
  weak_parent(parent)
{

}

inline std::shared_ptr<Entity> Entity::parent() const
{
  return weak_parent.lock();
}

template<typename T>
inline std::shared_ptr<T> Entity::find(const std::string& name, const std::vector<std::shared_ptr<T>>& entities)
{
  auto it = std::find_if(entities.begin(), entities.end(), [&name](const std::shared_ptr<T>& e) -> bool {
    return e->name == name;
    });

  return it != entities.end() ? *it : nullptr;
}


class DEX_MODEL_API Macro : public Entity
{
public:
  std::vector<std::string> parameters;

public:
  Macro(std::string name, std::vector<std::string> params, std::shared_ptr<Entity> parent = nullptr);

  static constexpr model::Kind ClassKind = model::Kind::Macro;
  model::Kind kind() const override;
};

inline Macro::Macro(std::string name, std::vector<std::string> params, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) },
  parameters(std::move(params))
{

}


class DEX_MODEL_API EnumValue : public Entity
{
public:
  explicit EnumValue(std::string name, std::shared_ptr<Enum> parent = nullptr);
  EnumValue(std::string name, std::string value, std::shared_ptr<Enum> parent = nullptr);

  static constexpr model::Kind ClassKind = model::Kind::EnumValue;
  model::Kind kind() const override;

  std::shared_ptr<Enum> parent() const;

  std::string& value();
  const std::string& value() const;

private:
  std::string m_value;
};


class DEX_MODEL_API Enum : public Entity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  std::vector<std::shared_ptr<EnumValue>> values;
  bool enum_class = false;

public:
  ~Enum() = default;

  explicit Enum(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr model::Kind ClassKind = model::Kind::Enum;
  model::Kind kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

  std::shared_ptr<EnumValue> find(const std::string& name) const;
};

inline Enum::Enum(std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) }
{

}



struct TemplateNonTypeParameter
{
  Type type;
  std::string default_value;

  TemplateNonTypeParameter()
  {

  }

  TemplateNonTypeParameter(const TemplateNonTypeParameter&) = default;
  TemplateNonTypeParameter(TemplateNonTypeParameter&&) = default;

  TemplateNonTypeParameter(Type t, std::string default_val = "")
    : type(t),
    default_value(std::move(default_val))
  {

  }

  TemplateNonTypeParameter& operator=(const TemplateNonTypeParameter&) = default;

  typedef std::string default_value_t;
};

struct TemplateTypeParameter
{
  Type default_value;

  TemplateTypeParameter()
  {

  }

  TemplateTypeParameter(const TemplateTypeParameter&) = default;
  TemplateTypeParameter(TemplateTypeParameter&&) = default;

  TemplateTypeParameter(Type t)
    : default_value(t)
  {

  }

  TemplateTypeParameter& operator=(const TemplateTypeParameter&) = default;

  typedef Type default_value_t;
};

class DEX_MODEL_API TemplateParameter : public Entity
{
private:
  bool m_is_type_parameter;
  std::tuple<TemplateNonTypeParameter, TemplateTypeParameter> m_data;

public:
  TemplateParameter();
  TemplateParameter(const TemplateParameter&) = default;
  TemplateParameter(TemplateParameter&&) = default;
  ~TemplateParameter() = default;

  TemplateParameter(std::string name, TemplateNonTypeParameter tntp);
  TemplateParameter(std::string name, TemplateTypeParameter ttp);

  static constexpr model::Kind ClassKind = model::Kind::TemplateParameter;
  model::Kind kind() const override;

  bool isTypeParameter() const;
  bool isNonTypeParameter() const;

  bool hasDefaultValue();

  template<typename T>
  typename T::default_value_t defaultValue() const
  {
    return std::get<T>(m_data).default_value;
  }

  TemplateParameter& operator=(const TemplateParameter&) = default;
  TemplateParameter& operator=(TemplateParameter&&) = default;
};

class DEX_MODEL_API TemplateArgument
{
private:
  //std::tuple<Type, Expression> m_data;
  std::tuple<std::string> m_data;

public:
  TemplateArgument(const TemplateArgument&) = default;
  TemplateArgument(TemplateArgument&&) = default;
  ~TemplateArgument() = default;

  //explicit TemplateArgument(Expression expr);
  //explicit TemplateArgument(const Type& type);
  explicit TemplateArgument(std::string expr_or_type);

  bool isType() const;

  template<typename T>
  const T& get() const
  {
    return std::get<T>(m_data);
  }

  std::string toString() const;

  TemplateArgument& operator=(const TemplateArgument&) = default;
  TemplateArgument& operator=(TemplateArgument&&) = default;
};



class DEX_MODEL_API FunctionSpecifier
{
public:

  enum Value
  {
    None = 0,
    Inline = 1,
    Static = 2,
    Constexpr = 4,
    Virtual = 8,
    Override = 16,
    Final = 32,
    Const = 64,
    Pure = 128,
    Noexcept = 256,
    Explicit = 512,
    Default = 1024,
    Delete = 2048,
  };
};

class DEX_MODEL_API FunctionKind
{
public:
  enum Value
  {
    None = 0,
    Constructor = 1,
    Destructor = 2,
    OperatorOverload,
    ConversionFunction,
  };
};

class DEX_MODEL_API FunctionParameter : public Entity
{
public:
  Type type;
  Expression default_value;

public:
  FunctionParameter(Type type, std::string name, std::shared_ptr<Function> parent = nullptr);
  FunctionParameter(Type type, std::string name, Expression default_value, std::shared_ptr<Function> parent = nullptr);

  static constexpr model::Kind ClassKind = model::Kind::FunctionParameter;
  model::Kind kind() const override;

  std::shared_ptr<Function> parent() const;
};

class DEX_MODEL_API Function : public Entity
{
public:

  struct ReturnType
  {
    Type type;
    std::optional<std::string> brief;
  };

public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  ReturnType return_type;
  std::vector<std::shared_ptr<FunctionParameter>> parameters;
  int specifiers = FunctionSpecifier::None;
  FunctionKind::Value category = FunctionKind::None;
  std::vector<std::shared_ptr<TemplateParameter>> template_parameters;


public:
  ~Function() = default;

  explicit Function(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr model::Kind ClassKind = model::Kind::Function;
  model::Kind kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

  typedef FunctionParameter Parameter;

  bool isTemplate() const;
  const std::vector<std::shared_ptr<TemplateParameter>>& templateParameters() const;

  bool isInline() const;
  bool isStatic() const;
  bool isConstexpr() const;
  bool isVirtual() const;
  bool isVirtualPure() const;
  bool isOverride() const;
  bool isFinal() const;
  bool isConst() const;
  bool isNoexcept() const;
  bool isExplicit() const;

  bool isConstructor() const;
  bool isDestructor() const;

  std::string signature() const;

  std::string specifiersList() const;
};

inline Function::Function(std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) }
{

}

inline bool Function::isInline() const
{
  return specifiers & FunctionSpecifier::Inline;
}

inline bool Function::isStatic() const
{
  return specifiers & FunctionSpecifier::Static;
}

inline bool Function::isConstexpr() const
{
  return specifiers & FunctionSpecifier::Constexpr;
}

inline bool Function::isVirtual() const
{
  return specifiers & FunctionSpecifier::Virtual;
}

inline bool Function::isVirtualPure() const
{
  return specifiers & FunctionSpecifier::Pure;
}

inline bool Function::isOverride() const
{
  return specifiers & FunctionSpecifier::Override;
}

inline bool Function::isFinal() const
{
  return specifiers & FunctionSpecifier::Final;
}

inline bool Function::isConst() const
{
  return specifiers & FunctionSpecifier::Const;
}

inline bool Function::isNoexcept() const
{
  return specifiers & FunctionSpecifier::Noexcept;
}

inline bool Function::isExplicit() const
{
  return specifiers & FunctionSpecifier::Explicit;
}

inline bool Function::isConstructor() const
{
  return category == FunctionKind::Constructor;
}

inline bool Function::isDestructor() const
{
  return category == FunctionKind::Destructor;
}



struct BaseClass
{
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  std::shared_ptr<Class> base;

  bool isPublicBase() const
  {
    return access_specifier == AccessSpecifier::PUBLIC;
  }

  bool isProtectedBase() const
  {
    return access_specifier == AccessSpecifier::PROTECTED;
  }

  bool isPrivateBase() const
  {
    return access_specifier == AccessSpecifier::PRIVATE;
  }
};

class DEX_MODEL_API Class : public Entity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  std::vector<BaseClass> bases;
  std::vector<std::shared_ptr<Entity>> members;
  bool is_struct = false;
  bool is_final = false;
  std::vector<std::shared_ptr<TemplateParameter>> template_parameters;

public:
  ~Class() = default;

  explicit Class(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr model::Kind ClassKind = model::Kind::Class;
  model::Kind kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;

  bool isTemplate() const;
  const std::vector<std::shared_ptr<TemplateParameter>>& templateParameters() const;
};

inline Class::Class(std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) }
{

}



class DEX_MODEL_API VariableSpecifier
{
public:

  enum Value
  {
    None = 0,
    Inline = 1,
    Static = 2,
    Constexpr = 4,
  };
};

class DEX_MODEL_API Variable : public Entity
{
public:
  Variable(Type type, std::string name, std::shared_ptr<Entity> parent = nullptr);
  Variable(Type type, std::string name, Expression default_value, std::shared_ptr<Entity> parent = nullptr);

  static constexpr model::Kind ClassKind = model::Kind::Variable;
  model::Kind kind() const override;

  Type& type();
  const Type& type() const;

  Expression& defaultValue();
  const Expression& defaultValue() const;

  int& specifiers();
  int specifiers() const;

public:
  Type m_type;
  int m_flags = VariableSpecifier::None;
  Expression m_default_value;
};

inline Variable::Variable(Type type, std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) },
  m_type{ type }
{

}

inline Variable::Variable(Type type, std::string name, Expression default_value, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) },
  m_type{ type },
  m_default_value{ std::move(default_value) }
{

}

inline Type& Variable::type()
{
  return m_type;
}

inline const Type& Variable::type() const
{
  return m_type;
}



class DEX_MODEL_API Typedef : public Entity
{
public:
  AccessSpecifier access_specifier = AccessSpecifier::PUBLIC;
  Type type;

public:
  Typedef(Type t, std::string name, std::shared_ptr<Entity> parent = nullptr);

  static constexpr model::Kind ClassKind = model::Kind::Typedef;
  model::Kind kind() const override;

  AccessSpecifier getAccessSpecifier() const override;
  void setAccessSpecifier(AccessSpecifier aspec) override;
};

inline Typedef::Typedef(Type t, std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) },
  type(t)
{

}


class DEX_MODEL_API Namespace : public Entity
{
public:
  std::vector<std::shared_ptr<Entity>> entities;

public:
  ~Namespace() = default;

  static constexpr model::Kind ClassKind = model::Kind::Namespace;
  model::Kind kind() const override;

  explicit Namespace(std::string name, std::shared_ptr<Entity> parent = nullptr);

  std::shared_ptr<Namespace> getOrCreateNamespace(const std::string& name);
  std::shared_ptr<Class> createClass(std::string name);
  std::shared_ptr<Class> getOrCreateClass(const std::string& name);
  std::shared_ptr<Enum> createEnum(std::string name);
  std::shared_ptr<Function> createFunction(std::string name);

  template<typename T, typename...Args>
  std::shared_ptr<T> getOrCreate(const std::string& name, Args&&... args)
  {
    auto it = std::find_if(entities.begin(), entities.end(), [&name](const std::shared_ptr<Entity>& e) {
      return e->is<T>() && e->name == name;
      });

    if (it != entities.end())
      return std::static_pointer_cast<T>(*it);

    auto result = std::make_shared<T>(std::forward<Args>(args)..., shared_from_this());
    entities.push_back(result);
    return result;
  }
};

inline Namespace::Namespace(std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) }
{

}


class DEX_MODEL_API File
{
private:
  std::string m_path;

public:
  std::vector<std::shared_ptr<Entity>> nodes;

public:
  explicit File(std::string path);

  const std::string& path() const;

  static void normalizePath(std::string& path);
};

inline File::File(std::string path)
  : m_path(std::move(path))
{

}

inline const std::string& File::path() const
{
  return m_path;
}

inline void File::normalizePath(std::string& path)
{
  for (char& c : path)
  {
    if (c == '\\')
      c = '/';
  }
}


// @TODO: could we add these informations to ClassDocumentation and FunctionDocumentation instead ?
class DEX_MODEL_API RelatedNonMembers
{
public:

  struct Entry
  {
    std::shared_ptr<Class> the_class;
    std::vector<std::shared_ptr<Function>> non_members;

    Entry(const std::shared_ptr<Class>& c)
      : the_class(c)
    {

    }
  };

  std::unordered_map<std::shared_ptr<Class>, std::shared_ptr<Entry>> class_map;
  std::unordered_map<std::shared_ptr<Function>, std::shared_ptr<Entry>> functions_map;

  bool empty() const;

  void relates(const std::shared_ptr<Function>& f, const std::shared_ptr<Class>& c);

  std::shared_ptr<Entry> getRelated(const std::shared_ptr<Class>& c) const;
  std::shared_ptr<Entry> getRelated(const std::shared_ptr<Function>& f) const;
};

class DEX_MODEL_API Program : public model::Object
{
public:
  std::vector<std::shared_ptr<Macro>> macros;
  std::vector<std::shared_ptr<File>> files;
  std::shared_ptr<Namespace> global_namespace;
  RelatedNonMembers related;

public:
  Program();
  ~Program();

  static constexpr model::Kind ClassKind = model::Kind::Program;
  model::Kind kind() const override;

  const std::shared_ptr<Namespace>& globalNamespace() const;

  std::shared_ptr<Entity> resolve(const Name& n);
  std::shared_ptr<Entity> resolve(const Name& n, const std::shared_ptr<Entity>& context);
};

inline const std::shared_ptr<Namespace>& Program::globalNamespace() const
{
  return global_namespace;
}

} // namespace dex

#endif // DEX_MODEL_PROGRAM_H
