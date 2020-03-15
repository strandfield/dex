// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_STATE_H
#define DEX_COMMON_STATE_H

#include "dex/dex-common.h"

#include <variant>
#include <vector>

namespace dex
{

namespace state
{

enum EmptyFrameSubType
{
  FST_None = 0,
};

template<typename FT, typename FST = EmptyFrameSubType>
class Frame
{
public:
  typedef FT frame_type_t;
  typedef FST frame_subtype_t;

public:
  FT type;
  FST subtype;

public:
  Frame(FT frame_type, FST frame_subtype = static_cast<FST>(0))
    : type(frame_type), subtype(frame_subtype)
  {

  }
};

template<typename F>
class State
{
public:
  typedef F frame_t;
  typedef typename F::frame_type_t frame_type_t;
  typedef typename F::frame_subtype_t frame_subtype_t;

public:
  std::vector<F> m_frames;

public:
  State() = default;
  State(const State<F>&) = delete;
  State(State<F>&&) = default;

  const std::vector<F>& frames() const { return m_frames; }

  size_t depth() const { return m_frames.size(); }

  F& current()
  {
    return m_frames.back();
  }

  const F& current() const
  {
    return m_frames.back();
  }

  void enter(F&& f)
  {
    m_frames.push_back(std::move(f));
  }

  template<frame_type_t FT, typename...Args>
  void enter(Args&&...args)
  {
    m_frames.emplace_back(FT, std::forward<Args>(args)...);
  }

  void leave()
  {
    m_frames.pop_back();
  }

  bool test(frame_type_t ft) const
  {
    return current().type == ft;
  }

  bool test(frame_type_t ft, frame_subtype_t fst) const
  {
    return current().type == ft && current().subtype == fst;
  }
};

} // namespace state

} // namespace dex

#endif // DEX_COMMON_STATE_H
