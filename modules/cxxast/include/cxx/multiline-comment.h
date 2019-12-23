// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_MULTILINE_COMMENT_H
#define CXXAST_MULTILINE_COMMENT_H

#include "cxx/documentation.h"

#include <string>

namespace cxx
{

class CXXAST_API MultilineComment : public Documentation
{
private:
  std::string m_text;

public:
  explicit MultilineComment(std::string text, const SourceLocation& loc = {});

  static const std::string TypeId;
  const std::string& type() const override;

  const std::string& text() const;
  std::string& text();
};

} // namespace cxx

namespace cxx
{

inline MultilineComment::MultilineComment(std::string text, const SourceLocation& loc)
  : Documentation(loc),
  m_text(std::move(text))
{

}

inline const std::string& MultilineComment::text() const
{
  return m_text;
}

inline std::string& MultilineComment::text()
{
  return m_text;
}

} // namespace cxx

#endif // CXXAST_MULTILINE_COMMENT_H
