// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/model.h"

namespace dex
{

namespace examples
{

std::shared_ptr<cxx::Program> prog_with_class();
std::shared_ptr<dex::Model> prog_with_class_image_description();
std::shared_ptr<dex::Model> prog_with_class_list_description();
std::shared_ptr<cxx::Program> prog_with_fun();
std::shared_ptr<cxx::Program> prog_with_var();
std::shared_ptr<cxx::Program> prog_with_class_and_fun();
std::shared_ptr<dex::Model> manual();

} // namespace examples

} // namespace dex
