// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PARSER_FRONTEND_H
#define DEX_INPUT_PARSER_FRONTEND_H

#include "dex/dex-input.h"

#include <map>
#include <memory>
#include <string>

namespace dex
{

struct FunctionCall;

class DocumentWriter;
class ManualParser;
class ParserMachine;
class ProgramParser;

class DEX_INPUT_API ParserFrontend
{
public:
  explicit ParserFrontend(ParserMachine& machine);
  ~ParserFrontend();

  ParserMachine& machine() const;

  enum class Mode
  {
    Idle,
    Program,
    Manual,
  };

  enum class CS
  {
    /* TeX */
    PAR,
    INPUT,
    /* Entities */
    CLASS,
    ENDCLASS,
    FN,
    ENDFN,
    NAMESPACE,
    ENDNAMESPACE,
    ENUM,
    ENDENUM,
    ENUMVALUE,
    ENDENUMVALUE,
    VARIABLE,
    ENDVARIABLE,
    TYPEDEF,
    ENDTYPEDEF,
    MACRO,
    ENDMACRO,
    NONMEMBER,
    RELATES,
    /* Metadata */
    BRIEF,
    SINCE,
    PARAM,
    RETURNS,
    /* Documents */
    manual,
    page,
    part,
    chapter,
    section,
    tableofcontents,
    makeindex,
    index,
    printindex,
    /* DOM elements */
    code,
    endcode,
    /* Grouping */
    ingroup,
  };

  static const std::map<std::string, CS>& csmap();
  
  void write(char c);
  void write_space(char c);
  void write_active(char c);

  void bgroup();
  void egroup();

  void mathshift();
  void alignmenttab();
  void superscript();
  void subscript();

  void handle(const FunctionCall& call);

  void beginFile();
  void endFile();
  void beginBlock();
  void endBlock();

  void recover();
  void reset();

protected:

  DocumentWriter& currentWriter();

  CS parseCs(const std::string& str) const;

  void checkMode(Mode m);

  void par(const FunctionCall& call);
  void input(const FunctionCall& call);

  void fn_class(const FunctionCall& call);
  void cs_endclass();
  void fn_fn(const FunctionCall& call);
  void cs_endfn();
  void fn_namespace(const FunctionCall& call);
  void cs_endnamespace();
  void fn_enum(const FunctionCall& call);
  void cs_endenum();
  void fn_enumvalue(const FunctionCall& call);
  void cs_endenumvalue();
  void fn_variable(const FunctionCall& call);
  void cs_endvariable();
  void typedef_(const FunctionCall& call);
  void endtypedef();
  void macro(const FunctionCall& call);
  void endmacro();

  void fn_brief(const FunctionCall& call);
  void fn_since(const FunctionCall& call);
  void fn_param(const FunctionCall& call);
  void fn_returns(const FunctionCall& call);
  void cs_nonmember();
  void fn_relates(const FunctionCall& call);

  void fn_manual(const FunctionCall& call);
  void fn_page(const FunctionCall& call);
  void fn_part(const FunctionCall& call);
  void fn_chapter(const FunctionCall& call);
  void fn_section(const FunctionCall& call);
  void tableofcontents();
  void makeindex();
  void index(const FunctionCall& call);
  void printindex();

  void ingroup(const FunctionCall& call);

  void code(const FunctionCall& call);
  void endcode(const FunctionCall& call);

private:
  ParserMachine& m_machine;
  Mode m_mode;
  std::unique_ptr<ProgramParser> m_prog_parser;
  std::unique_ptr<ManualParser> m_manual_parser;
};

} // namespace dex

namespace dex
{

inline ParserMachine& ParserFrontend::machine() const
{
  return m_machine;
}

}

#endif // DEX_INPUT_PARSER_FRONTEND_H
