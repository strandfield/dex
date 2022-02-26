// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_MESSAGE_HANDLER_H
#define DEX_APP_MESSAGE_HANDLER_H

#include "dex/dex-app.h"

#include "dex/common/logging.h"

namespace dex
{

DEX_APP_API void app_message_handler(log::Severity, Logger&, const json::Json&);

} // namespace dex

#endif // DEX_APP_MESSAGE_HANDLER_H
