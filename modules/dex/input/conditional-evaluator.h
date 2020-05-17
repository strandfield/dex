// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_CONDITIONAL_EVALUATOR_H
#define DEX_INPUT_CONDITIONAL_EVALUATOR_H

#include "dex/dex-input.h"

#include <tex/token.h>
#include <tex/parsing/preprocessor.h>

#include <vector>

namespace tex
{
namespace parsing
{
class Lexer;
} // namespace parsing
} // namespace tex

namespace dex
{

class InputStream;
class ParserMachine;

class DEX_INPUT_API ConditionalEvaluator
{
public:
  explicit ConditionalEvaluator(ParserMachine& machine);
  ConditionalEvaluator(InputStream& is, tex::parsing::Lexer& lex, tex::parsing::Preprocessor& preproc);

  InputStream& inputStream();
  tex::parsing::Lexer& lexer();

  enum class State
  {
    Idle,
    WaitingTestNextChar,
  };

  State& state();

  void write(tex::parsing::Token&& tok);

  std::vector<tex::parsing::Token>& output();

private:
  InputStream& m_inputstream;
  tex::parsing::Lexer& m_lexer;
  tex::parsing::Preprocessor& m_preprocessor;
  State m_state;
  std::vector<tex::parsing::Token> m_output;
};

} // namespace dex

namespace dex
{

inline std::vector<tex::parsing::Token>& ConditionalEvaluator::output()
{
  return m_output;
}

} // namespace dex

#endif // DEX_INPUT_CONDITIONAL_EVALUATOR_H
