// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_IMPORT_H
#define DEX_OUTPUT_JSON_IMPORT_H

#include "dex/dex-output.h"

#include "dex/model/model.h"

#include <dom/paragraph.h>

#include <json-toolkit/json.h>

namespace dex
{

class DEX_OUTPUT_API JsonImport
{
public:

  static std::shared_ptr<dom::Paragraph> decode_paragraph(const json::Object& par);

};

} // namespace dex

#endif // DEX_OUTPUT_JSON_IMPORT_H
