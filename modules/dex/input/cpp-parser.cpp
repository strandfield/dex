// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/cpp-parser.h"

#include <cpptok/tokenizer.h>

#include <cassert>
#include <map>

namespace dex
{

Name OverloadedOperatorName(const std::string& str)
{
  return "operator" + str;
}

Name LiteralOperatorName(const std::string& str)
{
  return "operator\"\"" + str;
}

Name QualifiedName(std::vector<Name>::const_iterator begin, std::vector<Name>::const_iterator end)
{
  std::string result;

  auto it = begin;
  for (auto it = begin; it != end; ++it)
  {
    result += *it + "::";
  }

  result.pop_back();
  result.pop_back();

  return result;
}

Name TemplateName(Name base, std::vector<TemplateArgument> targs)
{
  std::string result = base;

  result += "<";

  for (const TemplateArgument& targ : targs)
  {
    result += targ.toString() + ", ";
  }

  result.pop_back();
  result.pop_back();

  result += ">";

  return result;
}

class ParserViewRAII
{
protected:
  std::pair<size_t, size_t>& m_view;
  const std::pair<size_t, size_t> m_backup;

public:
  explicit ParserViewRAII(std::pair<size_t, size_t>& view)
    : m_view(view),
      m_backup(view)
  {

  }

  ParserViewRAII(std::pair<size_t, size_t>& view, size_t begin, size_t end)
    : m_view(view),
      m_backup(view)
  {
    m_view = std::make_pair(begin, end);
  }

  ~ParserViewRAII()
  {
    m_view = m_backup;
  }
};

class ParserParenView : public ParserViewRAII
{
public:

  ParserParenView(const std::vector<cpptok::Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserViewRAII(view)
  {
    size_t par_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == cpptok::TokenType::RightPar)
      {
        if (par_depth == 0)
        {
          view = std::make_pair(pos, std::distance(toks.begin(), it));
          return;
        }
        else
        {
          --par_depth;
        }
      }
      else if (it->type() == cpptok::TokenType::LeftPar)
      {
        ++par_depth;
      }

      ++it;
    }

    throw CppParserError{ "no matching parenthesis" };
  }
};

class ParserBracketView : public ParserViewRAII
{
public:
  ParserBracketView(const std::vector<cpptok::Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserViewRAII(view)
  {
    size_t bracket_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == cpptok::TokenType::RightBracket)
      {
        if (bracket_depth == 0)
        {
          view = std::make_pair(pos, std::distance(toks.begin(), it));
          return;
        }
        else
        {
          --bracket_depth;
        }
      }
      else if (it->type() == cpptok::TokenType::LeftBracket)
      {
        ++bracket_depth;
      }

      ++it;
    }

    throw CppParserError{ "no matching bracket" };
  }
};

class TemplateAngleView : public ParserViewRAII
{
public:
  bool split_right_shift = false;
  std::vector<cpptok::Token>& tokens;

public:
  TemplateAngleView(std::vector<cpptok::Token>& toks, std::pair<size_t, size_t>& view, size_t pos)
    : ParserViewRAII(view),
      tokens(toks)
  {
    size_t bracket_depth = 0;
    size_t paren_depth = 0;
    size_t angle_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == cpptok::TokenType::GreaterThan)
      {
        if (bracket_depth == 0 && paren_depth == 0)
        {
          if (angle_depth == 0)
          {
            view = std::make_pair(pos, std::distance(toks.begin(), it));
            return;
          }

          --angle_depth;
        }
      }
      else if (it->type() == cpptok::TokenType::RightShift)
      {
        if (bracket_depth == 0 && paren_depth == 0)
        {
          if (angle_depth == 1)
          {
            cpptok::Token tok = *it;
            *it = cpptok::Token(cpptok::TokenType::RightAngle, std::string_view(tok.text().data() + 1, 1));
            it = toks.insert(it, cpptok::Token(cpptok::TokenType::RightAngle, std::string_view(tok.text().data(), 1)));
            view = std::make_pair(pos, std::distance(toks.begin(), it) + 1);
            split_right_shift = true;
            return;
          }

          angle_depth -= 2;
        }
      }
      else if (it->type() == cpptok::TokenType::Less)
      {
        if (bracket_depth == 0 && paren_depth == 0)
        {
          ++angle_depth;
        }
      }
      else if (it->type() == cpptok::TokenType::RightBracket)
      {
        if (bracket_depth == 0)
          throw CppParserError{ "no matching bracket" };

        --bracket_depth;
      }
      else if (it->type() == cpptok::TokenType::LeftBracket)
      {
        ++bracket_depth;
      }
      else if (it->type() == cpptok::TokenType::RightPar)
      {
        if (paren_depth == 0)
          throw CppParserError{ "no matching parenthesis" };

        --paren_depth;
      }
      else if (it->type() == cpptok::TokenType::LeftPar)
      {
        ++paren_depth;
      }

      ++it;
    }

    throw CppParserError{ "no matching angle bracket" };
  }

  ~TemplateAngleView()
  {
    if (split_right_shift)
    {
      cpptok::Token tok = tokens.at(m_view.second - 1);
      tokens.erase(tokens.begin() + m_view.second);
      tokens[m_view.second - 1] = cpptok::Token(cpptok::TokenType::RightShift, std::string_view(tok.text().data(), 2));
    }
  }
};

class ListView : public ParserViewRAII
{
public:
  ListView(const std::vector<cpptok::Token>& toks, std::pair<size_t, size_t>& view, size_t pos, bool ignore_angle = true)
    : ParserViewRAII(view)
  {
    size_t bracket_depth = 0;
    size_t paren_depth = 0;
    size_t brace_depth = 0;
    size_t angle_depth = 0;

    auto begin = toks.begin() + pos;
    auto end = toks.begin() + view.second;

    auto it = begin;

    while (it != end)
    {
      if (it->type() == cpptok::TokenType::Comma)
      {
        if (brace_depth == 0 && paren_depth == 0 && bracket_depth == 0 && angle_depth == 0)
        {
          view = std::make_pair(pos, std::distance(toks.begin(), it));
          return;
        }
      }
      else if (it->type() == cpptok::TokenType::GreaterThan)
      {
        if (!ignore_angle && bracket_depth == 0 && paren_depth == 0 && brace_depth == 0)
        {
          if (angle_depth == 0)
            throw CppParserError{ "no matching angle bracket" };

          --angle_depth;
        }
      }
      else if (it->type() == cpptok::TokenType::Less)
      {
        if (!ignore_angle && bracket_depth == 0 && paren_depth == 0 && brace_depth == 0)
        {
          ++angle_depth;
        }
      }
      else if (it->type() == cpptok::TokenType::RightShift)
      {
        if (!ignore_angle && bracket_depth == 0 && paren_depth == 0 && brace_depth == 0)
        {
          angle_depth -= 2;
        }
      }
      else if (it->type() == cpptok::TokenType::RightBracket)
      {
        if (bracket_depth == 0)
          throw CppParserError{ "no matching bracket" };

        --bracket_depth;
      }
      else if (it->type() == cpptok::TokenType::LeftBracket)
      {
        ++bracket_depth;
      }
      else if (it->type() == cpptok::TokenType::RightPar)
      {
        if (paren_depth == 0)
          throw CppParserError{ "no matching parenthesis" };

        --paren_depth;
      }
      else if (it->type() == cpptok::TokenType::LeftPar)
      {
        ++paren_depth;
      }
      else if (it->type() == cpptok::TokenType::RightBrace)
      {
        if (brace_depth == 0)
          throw CppParserError{ "no matching brace" };

        --brace_depth;
      }
      else if (it->type() == cpptok::TokenType::LeftBrace)
      {
        ++brace_depth;
      }


      ++it;
    }

    if (bracket_depth != 0 || paren_depth != 0 || brace_depth != 0)
    {
      throw CppParserError{ "no matching bracket/brace/paren" };
    }
  }
};


CppParser::CppParser(const std::string *src)
{
  cpptok::Tokenizer lexer;
  lexer.tokenize(*src);

  for (const cpptok::Token& tok : lexer.output)
  {
    if (!isDiscardable(tok))
      m_buffer.push_back(tok);
  }

  m_view = std::make_pair(0, m_buffer.size());
}

dex::Type CppParser::parseType(const std::string& str)
{
  CppParser p{ &str };
  return p.parseType();
}

std::shared_ptr<Function> CppParser::parseFunctionSignature(const std::string& str)
{
  CppParser p{ &str };
  return p.parseFunctionSignature();
}

std::shared_ptr<Variable> CppParser::parseVariable(const std::string& str)
{
  CppParser p{ &str };
  return p.parseVariable();
}

std::shared_ptr<Typedef> CppParser::parseTypedef(const std::string& str)
{
  CppParser p{ &str };
  return p.parseTypedef();
}

std::shared_ptr<Macro> CppParser::parseMacro(const std::string& str)
{
  CppParser p{ &str };
  return p.parseMacro();
}

bool CppParser::atEnd() const
{
  return m_index == m_view.second;
}

Type CppParser::parseType()
{
  const size_t start = m_index;

  if (peek() == cpptok::TokenType::Const)
    unsafe_read();

  std::string name = parseName();

  if (atEnd())
    return tostring(start, m_index);

  if (unsafe_peek() == cpptok::TokenType::Const)
  {
    unsafe_read();

    if (atEnd())
      return tostring(start, m_index);

    if (unsafe_peek() == cpptok::TokenType::Ref || unsafe_peek() == cpptok::TokenType::RefRef)
    {
      unsafe_read();
    }
  }
  else if (unsafe_peek() == cpptok::TokenType::Ref || unsafe_peek() == cpptok::TokenType::RefRef)
  {
    unsafe_read();

    if (atEnd())
      return tostring(start, m_index);

    if (unsafe_peek() == cpptok::TokenType::Const)
      unsafe_read();
  }

  if (atEnd())
    return tostring(start, m_index);

  if (unsafe_peek() == cpptok::TokenType::LeftPar) 
  {
    auto save_point = pos();

    try
    {
      auto fsig = tryReadFunctionSignature(start);
      return fsig;
    }
    catch (const std::runtime_error&)
    {
      seek(save_point);
    }
  }
  else if (peek() == cpptok::TokenType::Star)
  {
    unsafe_read();

    while (!atEnd() && (peek() == cpptok::TokenType::Const || unsafe_peek() == cpptok::TokenType::Star))
    {
      cpptok::Token tok = read();
    }

    return tostring(start, m_index);
  }

  return tostring(start, m_index);
}

Type CppParser::tryReadFunctionSignature(size_t start)
{
  std::vector<Type> params;

  unsafe_read(); // read '('

  read(cpptok::TokenType::Star);
  read(cpptok::TokenType::RightPar);
  read(cpptok::TokenType::LeftPar);
  
  {
    ParserParenView paren_view{ m_buffer, m_view, m_index };

    while (!atEnd())
    {
      {
        ListView param_view{ m_buffer, m_view, m_index };
        Type t = parseType();
        params.push_back(t);
      }

      if (!atEnd())
        read(cpptok::TokenType::Comma);
    }
  }

  read(cpptok::TokenType::RightPar);

  return tostring(start, m_index);
}

Name CppParser::parseName()
{
  cpptok::Token t = peek();

  switch (t.type().value())
  {
  case cpptok::TokenType::Void:
  case cpptok::TokenType::Bool:
  case cpptok::TokenType::Char:
  case cpptok::TokenType::Int:
  case cpptok::TokenType::Float:
  case cpptok::TokenType::Double:
  case cpptok::TokenType::Auto:
  case cpptok::TokenType::This:
    return Name(unsafe_read().to_string());
  case cpptok::TokenType::Operator:
    return readOperatorName();
  case cpptok::TokenType::UserDefinedName:
    return readUserDefinedName();
  default:
    break;
  }

  throw std::runtime_error{ "expected identifier" };
}

Name CppParser::readOperatorName()
{
  //if (!testOption(ParseOperatorName))
  //  throw SyntaxError{ ParserError::UnexpectedToken, errors::UnexpectedToken{peek(), Token::Invalid} };

  cpptok::Token opkw = read();
  if (atEnd())
    throw std::runtime_error{ "unexpected end of input" };

  cpptok::Token op = peek();
  if (op.type().value() & cpptok::TokenCategory::OperatorToken)
  {
    return OverloadedOperatorName(unsafe_read().to_string());
  }
  else if (op.type() == cpptok::TokenType::LeftPar)
  {
    const cpptok::Token lp = read();
    const cpptok::Token rp = read(cpptok::TokenType::RightPar);

    if (lp.text().data() + 1 != rp.text().data())
      throw std::runtime_error{ "unexpected blank space between '(' and ')'" };

    return OverloadedOperatorName("()");
  }
  else if (op.type() == cpptok::TokenType::LeftBracket)
  {
    const cpptok::Token lb = read();
    const cpptok::Token rb = read(cpptok::TokenType::RightBracket);

    if (lb.text().data() + 1 != rb.text().data())
      throw std::runtime_error{ "unexpected blank space between '[' and ']'" };

    return OverloadedOperatorName("[]");
  }
  else if (op.type() == cpptok::TokenType::StringLiteral)
  {
    if (op.text().size() != 2)
      throw std::runtime_error{ "unexpected \"\"" };

    unsafe_read();
    auto suffix_name = parseName();

    return LiteralOperatorName(suffix_name);
  }
  else if (op.type() == cpptok::TokenType::UserDefinedLiteral)
  {
    op = unsafe_read();
    const std::string str = op.to_string();

    if (str.find("\"\"") != 0)
      throw std::runtime_error{ "unexpected \"\"" };

    std::string suffix_name{ str.begin() + 2, str.end() };
    return LiteralOperatorName(std::move(suffix_name));
  }

  throw std::runtime_error{ "expected operator symbol" };
}

Name CppParser::readUserDefinedName()
{
  const cpptok::Token base = read();

  if (base.type() != cpptok::TokenType::UserDefinedName)
    throw std::runtime_error{ "expected identifier" };

  Name ret = Name(base.to_string());

  if (atEnd())
    return ret;

  cpptok::Token t = peek();

  if (t.type() == cpptok::TokenType::LeftAngle)
  {
    const auto savepoint = pos();

    try
    {
      ret = readTemplateArguments(ret);
    }
    catch (const std::runtime_error&)
    {
      seek(savepoint);
      return ret;
    }
  }

  if (atEnd())
    return ret;

  t = peek();

  if (t.type() == cpptok::TokenType::ScopeResolution)
  {
    std::vector<Name> identifiers;
    identifiers.push_back(ret);

    while (t.type() == cpptok::TokenType::ScopeResolution)
    {
      read();

      identifiers.push_back(parseName());

      if (atEnd())
        break;
      else
        t = peek();
    }

    ret = Name(QualifiedName(identifiers.begin(), identifiers.end()));
  }

  return ret;
}

Name CppParser::readTemplateArguments(const Name base)
{
  const cpptok::Token leftangle = read();

  std::vector<TemplateArgument> params;

  bool need_read_right_angle = true;

  {
    TemplateAngleView main_view{ m_buffer, m_view, m_index };
    // If a '>>' was splitted in two by the TemplateAngleView, its 
    // destructor will implicitely consume the second '>' so there is no 
    // need to read() it afterward.
    need_read_right_angle = !main_view.split_right_shift;

    while (!atEnd())
    {
      {
        ListView sub_view{ m_buffer, m_view, m_index, false };
        params.push_back(parseDelimitedTemplateArgument());
      }

      if (!atEnd())
        read(cpptok::TokenType::Comma);
    }
  }

  if(need_read_right_angle)
    read(cpptok::TokenType::RightAngle);

  return Name{ TemplateName(base, std::move(params)) };
}

std::shared_ptr<Function> CppParser::parseFunctionSignature()
{
  int specifiers = FunctionSpecifier::None;
  FunctionKind::Value category = FunctionKind::None;

  while (peek() == cpptok::TokenType::Virtual || peek() == cpptok::TokenType::Static 
    || peek() == cpptok::TokenType::Inline || peek() == cpptok::TokenType::Constexpr
    || peek() == cpptok::TokenType::Explicit)
  {
    if (unsafe_peek() == cpptok::TokenType::Virtual)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Virtual;
    }
    else if(unsafe_peek() == cpptok::TokenType::Static)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Static;
    }
    else if (unsafe_peek() == cpptok::TokenType::Inline)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Inline;
    }
    else if (unsafe_peek() == cpptok::TokenType::Constexpr)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Constexpr;
    }
    else if (unsafe_peek() == cpptok::TokenType::Explicit)
    {
      unsafe_read();
      specifiers |= FunctionSpecifier::Explicit;
    }
  }

  Type return_type;
  Name fun_name;

  if (peek() == cpptok::TokenType::Tilde)
  {
    category = FunctionKind::Destructor;
    read();
    fun_name = "~" + parseName();
  }
  else if (peek() == cpptok::TokenType::Operator)
  {
    category = FunctionKind::ConversionFunction;
    read();
    fun_name = "operator " + parseType();
  }
  else
  {
    return_type = parseType();

    if (peek() == cpptok::TokenType::LeftPar)
    {
      fun_name = return_type;
      return_type = "";
      category = FunctionKind::Constructor;
    }
    else
    {
      fun_name = parseName();
    }
  }

  auto ret = std::make_shared<Function>(fun_name);
  ret->specifiers = specifiers;
  ret->return_type.type = return_type;
  ret->category = category;

  read(cpptok::TokenType::LeftPar);

  {
    ParserParenView parameters_view{ m_buffer, m_view, m_index };

    while (!atEnd())
    {
      {
        constexpr bool ignore_angle = false;
        ListView param_view{ m_buffer, m_view,  m_index, ignore_angle };

        ret->parameters.push_back(parseFunctionParameter());
        ret->parameters.back()->weak_parent = ret;
      }

      if (!atEnd())
        read(cpptok::TokenType::Comma);
    }

  }

  read(cpptok::TokenType::RightPar);

  if(atEnd())
    return ret;

  cpptok::Token tok = read();

  if (tok == cpptok::TokenType::Semicolon)
    return ret;

  if (tok == cpptok::TokenType::Const)
  {
    ret->specifiers |= FunctionSpecifier::Const;

    if (atEnd())
      return ret;

    tok = read();
  }

  if (tok == cpptok::TokenType::Semicolon)
    return ret;

  if (tok == cpptok::TokenType::Noexcept)
  {
    ret->specifiers |= FunctionSpecifier::Noexcept;

    if (atEnd())
      return ret;

    tok = read();

    if (tok == cpptok::TokenType::Semicolon)
      return ret;
  }

  if (tok == cpptok::TokenType::Override)
  {
    ret->specifiers |= FunctionSpecifier::Override;
  }
  else if (tok == cpptok::TokenType::Final)
  {
    ret->specifiers |= FunctionSpecifier::Final;
  }
  else if (tok == cpptok::TokenType::Eq)
  {
    tok = read();

    if (tok == cpptok::TokenType::Zero)
    {
      ret->specifiers |= FunctionSpecifier::Pure;
    }
    else
    {
      throw std::runtime_error{ "expected = 0" };
    }
  }

  if (atEnd())
    return ret;

  read(cpptok::TokenType::Semicolon);

  return ret;
}

std::shared_ptr<Variable> CppParser::parseVariable()
{
  int specifiers = VariableSpecifier::None;

  while (peek() == cpptok::TokenType::Inline || peek() == cpptok::TokenType::Static || peek() == cpptok::TokenType::Constexpr)
  {
    if (unsafe_peek() == cpptok::TokenType::Inline)
    {
      unsafe_read();
      specifiers |= VariableSpecifier::Inline;
    }
    else if (unsafe_peek() == cpptok::TokenType::Static)
    {
      unsafe_read();
      specifiers |= VariableSpecifier::Static;
    }
    else
    {
      unsafe_read();
      specifiers |= VariableSpecifier::Constexpr;
    }
  }

  Type type = parseType();
  Name name = parseName();

  auto ret = std::make_shared<Variable>(type, name);
  ret->specifiers() = specifiers;

  if (atEnd() || peek() == cpptok::TokenType::Semicolon)
    return ret;

  read(cpptok::TokenType::Eq);

  std::string default_val = stringtoend();

  if (default_val.back() == ';')
    default_val.pop_back();

  ret->defaultValue() = Expression{ std::move(default_val) };

  return ret;
}

std::shared_ptr<Typedef> CppParser::parseTypedef()
{
  read(cpptok::TokenType::Typedef);

  Type t = parseType();

  cpptok::Token name = read();

  if (!name.isIdentifier())
    throw std::runtime_error{ "Unexpected identifier while parsing typedef" };

  auto result = std::make_shared<Typedef>(t, name.to_string());

  if (atEnd())
    return result;

  read(cpptok::TokenType::Semicolon);

  return result;
}

std::shared_ptr<Macro> CppParser::parseMacro()
{
  std::string name = read(cpptok::TokenType::UserDefinedName).to_string();
  std::vector<std::string> params;

  if (atEnd())
    return std::make_shared<Macro>(name, std::move(params));

  read(cpptok::TokenType::LeftPar);

  cpptok::Token tok = read();

  for (;;)
  {
    if (tok.isIdentifier())
    {
      params.push_back(tok.to_string());

      tok = read();

      if (tok.type() == cpptok::TokenType::Comma)
        tok = read();
      else if (tok.type() == cpptok::TokenType::RightPar)
        break;
    }
    else if (tok.type() == cpptok::TokenType::Dot)
    {
      read(cpptok::TokenType::Dot);
      read(cpptok::TokenType::Dot);

      params.push_back("...");

      read(cpptok::TokenType::RightPar);
      break;
    }
    else
    {
      throw std::runtime_error{ "bad input to parseMacro" };
    }
  }

  return std::make_shared<Macro>(name, std::move(params));
}

cpptok::Token CppParser::read()
{
  if (m_index == m_buffer.size())
    throw std::runtime_error{ "Unexpected end of input" };

  return unsafe_read();
}

cpptok::Token CppParser::unsafe_read()
{
  assert(m_index < m_buffer.size());
  return m_buffer[m_index++];
}

cpptok::Token CppParser::peek()
{
  if (atEnd())
    throw std::runtime_error{ "Unexpected end of input" };

  return unsafe_peek();
}

cpptok::Token CppParser::unsafe_peek() const
{
  assert(m_index < m_buffer.size()); 
  return m_buffer[m_index];
}

bool CppParser::isDiscardable(const cpptok::Token& t) const
{
  return t == cpptok::TokenType::MultiLineComment || t == cpptok::TokenType::SingleLineComment;
}

cpptok::Token CppParser::read(cpptok::TokenType::Value tokt)
{
  const cpptok::Token tok = read();

  if (tok.type().value() != tokt)
    throw std::runtime_error{ "unexpected token" };

  return tok;
}

size_t CppParser::pos() const
{
  return m_index;
}

void CppParser::seek(size_t pos)
{
  m_index = pos;
}

void CppParser::seekBegin()
{
  m_index = m_view.first;
}

void CppParser::seekEnd()
{
  m_index = m_view.second;
}

std::string CppParser::tostring(size_t begin, size_t end) const
{
  cpptok::Token first = *(m_buffer.begin() + begin);
  cpptok::Token last = *(m_buffer.begin() + end - 1);

  std::string result{ first.text().data(), last.text().data() + last.text().size() };
  return result;
}

std::string CppParser::viewstring() const
{
  cpptok::Token first = *(m_buffer.begin() + m_view.first);
  cpptok::Token last = *(m_buffer.begin() + m_view.second - 1);

  return std::string(first.text().data(), last.text().data() + last.text().size());
}

std::string CppParser::stringtoend() const
{
  cpptok::Token first = m_buffer[m_index];
  cpptok::Token last = *(m_buffer.begin() + m_view.second - 1);

  return std::string(first.text().data(), last.text().data() + last.text().size());
}

TemplateArgument CppParser::parseDelimitedTemplateArgument()
{
  try
  {
    Type type = parseType();

    if (!atEnd())
      return seekEnd(), TemplateArgument{ viewstring() };

    return TemplateArgument{ type };
  }
  catch (const std::runtime_error & )
  {
    return seekEnd(), TemplateArgument{ viewstring() };
  }
}

std::shared_ptr<TemplateParameter> CppParser::parseDelimitedTemplateParameter()
{
  cpptok::Token tok = peek();

  if (tok == cpptok::TokenType::Typename || tok == cpptok::TokenType::Class)
  {
    unsafe_read();

    if (atEnd())
      return std::make_shared<TemplateParameter>("", TemplateTypeParameter());

    std::string name = peek().isIdentifier() ? read().to_string() : "";

    if (atEnd())
      return std::make_shared<TemplateParameter>(std::move(name), TemplateTypeParameter());

    if (peek() != cpptok::TokenType::Eq)
      throw std::runtime_error{ "expected '='" };

    Type default_value = parseType();

    if(!atEnd())
      throw std::runtime_error{ "expected end of input" };

    return std::make_shared<TemplateParameter>(std::move(name), TemplateTypeParameter(default_value));
  }
  else
  {
    Type type = parseType();

    if (atEnd())
      return std::make_shared<TemplateParameter>("", TemplateNonTypeParameter(type));

    std::string name = peek().isIdentifier() ? read().to_string() : "";

    if (atEnd())
      return std::make_shared<TemplateParameter>(std::move(name), TemplateNonTypeParameter(type));

    if (peek() != cpptok::TokenType::Eq)
      throw std::runtime_error{ "expected '='" };

    unsafe_read();

    std::string default_val = "";

    while (!atEnd())
    {
      default_val += unsafe_read().to_string() + " ";
    }

    default_val.pop_back();

    return std::make_shared<TemplateParameter>(std::move(name), TemplateNonTypeParameter(type, std::move(default_val)));
  }
}

std::shared_ptr<Function::Parameter> CppParser::parseFunctionParameter()
{
  const Type param_type = parseType();

  if (atEnd())
    return std::make_shared<Function::Parameter>(param_type, "");

  std::string name;

  if (peek().isIdentifier())
    name = read().to_string();

  if (atEnd())
    return std::make_shared<Function::Parameter>(param_type, std::move(name));

  read(cpptok::TokenType::Eq);

  std::string default_value = stringtoend();
  seekEnd();

  return std::make_shared<Function::Parameter>(param_type, std::move(name), Expression{ std::move(default_value) });
}

} // namespace dex

