// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PROGRAM_MODE_H
#define DEX_INPUT_PROGRAM_MODE_H

#include "dex/input/parser-mode.h"

#include "dex/input/document-writer.h"

#include "dex/common/state.h"

#include <cxx/class.h>

#include <map>
#include <variant>

namespace dex
{

class FunctionCaller;

class DEX_INPUT_API ProgramMode : public ParserMode
{
public:
  explicit ProgramMode(ParserMachine& machine);

  enum class FrameType
  {
    Idle,
    Class,
    Namespace,
    Function,
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
    std::variant<std::monostate> data;
  };

  using State = state::State<Frame>;
  
  State& state();

  enum class CS
  {
    /* TeX */
    PAR,
    /* Entities */
    CLASS,
    ENDCLASS,
    FN,
    ENDFN,
    NAMESPACE,
    ENDNAMESPACE,
    /* Metadata */
    BRIEF,
    SINCE,
    PARAM,
    RETURNS,
  };

  static const std::map<std::string, CS>& csmap();
  
  bool write(tex::parsing::Token&& tok) override;
  bool handle(const FunctionCall& call) override;

  void childFinished(ParserMode& mode) override;

  void beginFile() override;
  void endFile() override;
  void beginBlock() override;
  void endBlock() override;

protected:
  Frame& currentFrame();
  void exitFrame();

  CS parseCs(const std::string& str) const;

  void write_idle(tex::parsing::Token&& tok);
  void write_entity(tex::parsing::Token&& tok);

  void cs_par();

  void fn_class(const FunctionCall& call);
  void cs_endclass();
  void fn_fn(const FunctionCall& call);
  void cs_endfn();
  void fn_namespace(const FunctionCall& call);
  void cs_endnamespace();

  void fn_brief(const FunctionCall& call);
  void fn_since(const FunctionCall& call);
  void fn_param(const FunctionCall& call);
  void fn_returns(const FunctionCall& call);

private:
  State m_state;
};

} // namespace dex

#endif // DEX_INPUT_PROGRAM_MODE_H
