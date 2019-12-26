// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-import.h"

#include "dex/model/since.h"

#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

namespace dex
{

std::shared_ptr<dom::Paragraph> JsonImport::decode_paragraph(const json::Object& par)
{
  // TODO
  return std::make_shared<dom::Paragraph>(par["text"].toString());
}

} // namespace dex
