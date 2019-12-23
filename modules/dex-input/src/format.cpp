// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/format.h"

#include <tex/lexer.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

const tex::parsing::Token _1 = tex::parsing::Token{ 1 };
const tex::parsing::Token _2 = tex::parsing::Token{ 2 };
const tex::parsing::Token _3 = tex::parsing::Token{ 3 };
const tex::parsing::Token _4 = tex::parsing::Token{ 4 };
const tex::parsing::Token _5 = tex::parsing::Token{ 5 };

std::vector<tex::parsing::Token> to_tokens(const std::vector<TokenBuilder>& toks)
{
  std::vector<tex::parsing::Token> result;

  for (const auto& t : toks)
    result.push_back(t);

  return result;
}

TokenBuilder::TokenBuilder(const char* csname)
  : m_tok{ std::string(csname) }
{

}

TokenBuilder::TokenBuilder(std::string csname)
  : m_tok{ std::move(csname) }
{

}

TokenBuilder::TokenBuilder(char c, tex::parsing::CharCategory cc)
{
  if (cc == tex::parsing::CharCategory::Invalid)
    cc = tex::parsing::Lexer::DefaultCatCodes[static_cast<size_t>(c)];

  m_tok = tex::parsing::CharacterToken{ c, cc };
}

TokenBuilder::TokenBuilder(tex::parsing::Token tok)
  : m_tok{ std::move(tok) }
{

}

TokenBuilder::operator const tex::parsing::Token& () const
{
  return m_tok;
}

MacroBuilder::MacroBuilder(std::string csname, std::vector<TokenBuilder> toks)
  : m_macro{ std::move(csname), to_tokens(toks) }
{

}

MacroBuilder::MacroBuilder(std::string csname, std::vector<TokenBuilder> param_text, std::vector<TokenBuilder> repl_text)
  : m_macro{ std::move(csname), to_tokens(param_text), to_tokens(repl_text) }
{

}

MacroBuilder::operator const tex::parsing::Macro& () const
{
  return m_macro;
}

Format::Format(std::initializer_list<MacroBuilder>&& mbuilders)
{
  for (const auto& mb : mbuilders)
  {
    m_macros.push_back(tex::parsing::Macro{ mb });
  }
}

const std::vector<tex::parsing::Macro>& Format::macros() const
{
  return m_macros;
}

const MacroBuilder Since = { "since",
  {"@ifnextchar", '[', '{',
    "def", "@afteroptions", '{',
      "@ifleftbrace", '{',
        "p@rseword", "c@ll", "@since",
       '}', '{',
         "beginsince",
       '}',
     '}',
    "p@rseoptions",
  '}', '{',
    "p@rseline", "c@ll", "@since",
  '}'}
};

const Format DexFormat = {
  /*Input conditionals */
  {"@ifnextchar", {_1, _2, _3}, {"testnextch@r", _1, "ifbr", _2, "else", _3, "fi"}},
  {"@ifleftbrace", {_1, _2}, {"testleftbr@ce", "ifbr", _1, "else", _2, "fi"}},
  /* Environment */
  {"begin", {_1}, {"csname", _1, "endcsname"}},
  {"end", {_1}, {"csname", 'e', 'n', 'd', _1, "endcsname"}},
  /* Dex */
  {"class", {"p@rseline", "c@ll", "cl@ss"}},
  {"namespace", {"p@rseline", "c@ll", "n@mesp@ce"}},
  {"fn", {"p@rseline", "c@ll", "functi@n"}},
  Since,
  {"brief", {"p@rseline", "c@ll", "@brief"}},
  {"param", {"p@rseline", "c@ll", "p@r@m"}},
  {"returns", {"p@rseline", "c@ll", "@returns"}},
};

} // namespace dex
