// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_SINCE_H
#define DEX_MODEL_SINCE_H

#include "dex/dex-model.h"

#include "dex/model/document.h"

namespace dex
{

class DEX_MODEL_API Since
{
private:
  std::string m_version;

public:
  Since(std::string version);

  static constexpr model::Kind ClassKind = model::Kind::Since;

  const std::string& version() const;
};

class DEX_MODEL_API BeginSince : public DocumentElement
{
public:
  std::string version;

public:
  explicit BeginSince(std::string v);

  static constexpr model::Kind ClassKind = model::Kind::BeginSince;
  model::Kind kind() const override;
};

class DEX_MODEL_API EndSince : public DocumentElement
{
public:
  std::weak_ptr<BeginSince> beginsince;

public:
  explicit EndSince(std::shared_ptr<BeginSince> bsince);

  static constexpr model::Kind ClassKind = model::Kind::EndSince;
  model::Kind kind() const override;
};

} // namespace dex

#endif // DEX_MODEL_SINCE_H
