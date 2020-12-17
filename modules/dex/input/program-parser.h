// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PROGRAM_PARSER_H
#define DEX_INPUT_PROGRAM_PARSER_H

#include "dex/dex-input.h"

#include "dex/input/document-writer.h"

#include "dex/model/program.h"

#include "dex/common/state.h"

#include <cxx/namespace.h>

namespace dex
{

class DEX_INPUT_API ProgramParser
{
public:
  explicit ProgramParser(std::shared_ptr<dex::Program> prog);

  enum class FrameType
  {
    Idle,
    Class,
    Namespace,
    Function,
    Enum,
    EnumValue,
    Variable,
    Typedef,
    Macro,
  };

  struct Frame : state::Frame<FrameType>
  {
    Frame(const Frame&) = delete;
    Frame(Frame&& f) = default;
    ~Frame() = default;

    explicit Frame(FrameType ft);
    Frame(FrameType ft, std::shared_ptr<cxx::Entity> cxxent);

    std::shared_ptr<cxx::Node> node;
    std::shared_ptr<DocumentWriter> writer;
  };

  using State = state::State<Frame>;
  
  State& state();

  std::shared_ptr<cxx::Entity> currentEntity() const;
  
  // @TODO: rework these, they may not belong here
  void beginFile();
  void endFile();
  void beginBlock();
  void endBlock();

  std::shared_ptr<DocumentWriter> contentWriter();

  void class_(std::string name);
  void endclass();

  void fn(std::string signature);
  void endfn();

  void namespace_(std::string name);
  void endnamespace();

  void enum_(std::string name);
  void endenum();

  void value(std::string name);
  void endenumvalue();

  void variable(std::string decl);
  void endvariable();

  void typedef_(std::string decl);
  void endtypedef();

  void macro(std::string decl);
  void endmacro();

  void brief(std::string brieftext);
  void since(std::string version);

  void param(std::string des);
  void returns(std::string des);

  void nonmember();
  void relates(const std::string& class_name);

protected:
  Frame& currentFrame();
  void exitFrame();
  void appendChild(std::shared_ptr<cxx::Entity> e);
  static void appendChild(std::shared_ptr<cxx::Node> parent, std::shared_ptr<cxx::Entity> child);

private:
  State m_state;
  std::shared_ptr<dex::Program> m_program;
  std::shared_ptr<cxx::Entity> m_lastblock_entity;
};

} // namespace dex

#endif // DEX_INPUT_PROGRAM_PARSER_H
