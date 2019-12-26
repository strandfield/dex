// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-output-annotator.h"

namespace dex
{

void JsonUrlAnnotator::visit_entity(const cxx::Entity& e, json::Object& obj)
{
  std::string url = get_url(e);

  if (!url.empty())
    obj["url"] = url;
  
  JsonAnnotator::visit_entity(e, obj);
}

} // namespace dex
