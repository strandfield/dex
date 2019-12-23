// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_FORMAT_H
#define DEX_INPUT_FORMAT_H

#include "dex/dex-input.h"

#include <tex/parsing/preprocessor.h>

namespace dex
{

DEX_INPUT_API extern const tex::parsing::Token _1;
DEX_INPUT_API extern const tex::parsing::Token _2;
DEX_INPUT_API extern const tex::parsing::Token _3;
DEX_INPUT_API extern const tex::parsing::Token _4;
DEX_INPUT_API extern const tex::parsing::Token _5;

class DEX_INPUT_API TokenBuilder
{
public:
  TokenBuilder(const char* csname);
  TokenBuilder(std::string csname);
  TokenBuilder(char c, tex::parsing::CharCategory cc = tex::parsing::CharCategory::Invalid);
  TokenBuilder(tex::parsing::Token tok);
  
  operator const tex::parsing::Token& () const;

private:
  tex::parsing::Token m_tok;
};

class DEX_INPUT_API MacroBuilder
{
public:
  MacroBuilder(std::string csname, std::vector<TokenBuilder> toks);
  MacroBuilder(std::string csname, std::vector<TokenBuilder> param_text, std::vector<TokenBuilder> repl_text);

  operator const tex::parsing::Macro& () const;

private:
  tex::parsing::Macro m_macro;
};

class DEX_INPUT_API Format
{
public:
  Format(std::initializer_list<MacroBuilder>&& mbuilders);

  const std::vector<tex::parsing::Macro>& macros() const;

private:
  std::vector<tex::parsing::Macro> m_macros;
};

DEX_INPUT_API extern const Format DexFormat;

} // namespace dex

namespace dex
{

} // namespace dex

#endif // DEX_INPUT_FORMAT_H
