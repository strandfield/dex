// Copyright (C) 2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_PARSING_H
#define DEX_APP_PARSING_H

#include "dex/dex-app.h"

#include "dex/model/model.h"

#include <memory>
#include <set>
#include <string>

namespace dex
{

DEX_APP_API std::shared_ptr<Model> parse_inputs(const std::set<std::string>& inputs, const std::set<std::string>& suffixes);

} // namespace dex

#endif // DEX_APP_PARSING_H
