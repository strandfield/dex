// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/enum-parser.h"

#include "dex/common/logging.h"
#include "dex/common/string-utils.h"

#include <cpptok/tokenizer.h>

#include <algorithm>

namespace dex
{

EnumParser::EnumParser(std::shared_ptr<Enum> e)
  : m_enum(e)
{

}

void EnumParser::parse(const std::string& source)
{
  std::vector<std::string_view> lines = StdStringCRef(source).split('\n');

  std::vector<cpptok::Token> tokens = tokenize(lines);
  removeComments(tokens);

  std::vector<std::string> names = parseValues(tokens);

  if (names.size() <= m_enum->values.size())
    return;

  std::vector<std::shared_ptr<EnumValue>> values{ std::move(m_enum->values) };

  for (std::string& n : names)
  {
    std::shared_ptr<EnumValue> eval = Entity::find(n, values);

    if (!eval)
    {
      eval = std::make_shared<EnumValue>(std::move(n), m_enum);
    }

    m_enum->values.push_back(eval);
  }
}

std::vector<cpptok::Token> EnumParser::tokenize(const std::vector<std::string_view>& lines)
{
  cpptok::Tokenizer lexer;

  for (const std::string_view& l : lines)
  {
    lexer.tokenize(l.data(), l.size());
  }

  return lexer.output;
}

void EnumParser::removeComments(std::vector<cpptok::Token>& toks)
{
  auto it = std::remove_if(toks.begin(), toks.end(), [](const cpptok::Token& tok) -> bool {
    return tok.isComment();
    });

  toks.erase(it, toks.end());
}

static bool parse_value(std::vector<cpptok::Token>::const_iterator& it, std::vector<cpptok::Token>::const_iterator end, std::vector<std::string>& out)
{
  if (!it->isIdentifier())
    return false;

  out.push_back((it++)->to_string());

  while (it != end)
  {
    if (it->type() == cpptok::TokenType::Comma)
    {
      ++it;
      break;
    }

    ++it;
  }

  return true;
}

static std::vector<std::string> parse_values(std::vector<cpptok::Token>::const_iterator begin, std::vector<cpptok::Token>::const_iterator end)
{
  std::vector<std::string> result;

  while (begin != end && parse_value(begin, end, result));

  return result;
}

std::vector<std::string> EnumParser::parseValues(const std::vector<cpptok::Token>& tokens)
{
  auto it = std::find_if(tokens.begin(), tokens.end(), [](const cpptok::Token& tok) -> bool {
    return tok.type() == cpptok::TokenType::Enum;
    });

  if (it == tokens.end())
    return {};

  ++it;

  if (it == tokens.end())
    return {};

  if (it->isIdentifier())
  {
    ++it;

    if (it == tokens.end())
      return {};
  }

  if (it->type() != cpptok::TokenType::LeftBrace)
    return {};

  auto end = std::find_if(it, tokens.end(), [](const cpptok::Token& tok) -> bool {
    return tok.type() == cpptok::TokenType::RightBrace;
    });

  if (end == tokens.end())
    return {};

  return parse_values(it + 1, end);
}

} // namespace dex
