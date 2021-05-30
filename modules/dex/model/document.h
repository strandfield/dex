// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_DOCUMENT_H
#define DEX_MODEL_DOCUMENT_H

#include "dex/model/model-base.h"

#include <string>
#include <vector>

namespace dex
{

class DEX_MODEL_API DocumentNode : public model::Object
{
public:
  std::weak_ptr<DocumentNode> weak_parent;

public:
  ~DocumentNode() = default;

  bool isDocumentNode() const override;

  virtual const std::vector<std::shared_ptr<DocumentNode>>& childNodes() const;
  virtual void appendChild(const std::shared_ptr<DocumentNode>& node);
};

typedef std::shared_ptr<DocumentNode> DocumentNodePtr;
typedef std::vector<std::shared_ptr<DocumentNode>> DomNodeList;

class DEX_MODEL_API Document : public DocumentNode
{
public:
  std::string title;
  std::string doctype = "document";
  std::vector<std::shared_ptr<DocumentNode>> nodes;

public:
  ~Document() = default;

  explicit Document(std::string title = "");

  static constexpr model::Kind ClassKind = model::Kind::Document;
  model::Kind kind() const override;

  bool isDocument() const override;

  std::string className() const override;

  const DomNodeList& childNodes() const;
  void appendChild(const DocumentNodePtr& node) override;
};

class DEX_MODEL_API DocumentElement : public DocumentNode
{
public:
  std::string id;

public:
  ~DocumentElement() = default;

  bool isDocumentElement() const override;
};

class Paragraph;

class DEX_MODEL_API ParagraphRange
{
private:
  const Paragraph* m_par;
  size_t m_begin;
  size_t m_end;

public:
  ParagraphRange(const ParagraphRange&) = default;

  explicit ParagraphRange(const Paragraph& p, size_t begin = 0, size_t end = std::numeric_limits<size_t>::max());

  const Paragraph& paragraph() const;
  size_t begin() const;
  size_t end() const;

  std::string text() const;
};

inline bool operator==(const ParagraphRange& lhs, const ParagraphRange& rhs)
{
  return lhs.begin() == rhs.begin() && rhs.end() == lhs.end();
}

inline bool operator!=(const ParagraphRange& lhs, const ParagraphRange& rhs)
{
  return !(lhs == rhs);
}

class DEX_MODEL_API ParagraphMetaData : public DocumentNode
{
private:
  ParagraphRange m_range;

protected:
  ParagraphMetaData(const ParagraphRange& parrange);

public:
  ParagraphRange& range();
  const ParagraphRange& range() const;

  template<typename T>
  const T& get() const;
};

template<typename T>
class GenericParagraphMetaData : public ParagraphMetaData
{
private:
  T m_value;

public:
  GenericParagraphMetaData(const ParagraphRange& pr, T value)
    : ParagraphMetaData(pr),
    m_value(std::move(value))
  {

  }

  model::Kind kind() const override
  {
    return T::ClassKind;
  }

  T& value() { return m_value; }
  const T& value() const { return m_value; }
};

template<typename T>
inline const T& ParagraphMetaData::get() const
{
  return static_cast<const GenericParagraphMetaData<T>*>(this)->value();
}

class DEX_MODEL_API ParagraphIterator
{
private:
  const Paragraph* m_par;
  size_t m_parent_index;
  size_t m_index;
  bool m_text;

public:
  ParagraphIterator(const ParagraphIterator&) = default;

  explicit ParagraphIterator(const Paragraph& p);
  ParagraphIterator(const Paragraph& p, size_t parent_index);
  ParagraphIterator(const Paragraph& p, size_t parent_index, size_t index, bool text);

  const Paragraph& paragraph() const;
  ParagraphRange range() const;

  bool isText() const;
  bool hasChild() const;

  bool isEnd() const;

  ParagraphIterator begin() const;
  ParagraphIterator end() const;

  ParagraphIterator& operator++();
  ParagraphIterator operator++(int);

  std::shared_ptr<ParagraphMetaData> operator*() const;

  bool operator==(const ParagraphIterator& other) const;
  bool operator!=(const ParagraphIterator& other) const;

protected:
  bool isChild() const;
};

class DEX_MODEL_API Paragraph : public DocumentElement
{
public:
  Paragraph() = default;
  ~Paragraph() = default;

  explicit Paragraph(std::string text);

  static constexpr model::Kind ClassKind = model::Kind::Paragraph;
  model::Kind kind() const override;

  std::string& text();
  const std::string& text() const;
  size_t length() const;

  ParagraphRange range(size_t begin, size_t end) const;

  ParagraphIterator begin() const;
  ParagraphIterator end() const;

  void setText(std::string text);

  void addChar(char c);
  void addText(const std::string& text);

  const std::vector<std::shared_ptr<ParagraphMetaData>>& metadata() const;

  void addMetaData(const std::shared_ptr<ParagraphMetaData>& md);

  template<typename T, typename...Args>
  void add(ParagraphRange pr, Args&& ... args);

protected:
  struct TypeDerivedFromMetaData {};
  struct BuildGenericMetaData {};

  template<typename T, typename...Args>
  void add_meta_data(TypeDerivedFromMetaData, ParagraphRange pr, Args&& ... args);

  template<typename T, typename...Args>
  void add_meta_data(BuildGenericMetaData, ParagraphRange pr, Args&& ... args);

public:
  std::string m_text;
  std::vector<std::shared_ptr<ParagraphMetaData>> m_metadata;
};

inline Paragraph::Paragraph(std::string text)
  : m_text(std::move(text))
{

}

inline std::string& Paragraph::text()
{
  return m_text;
}

inline const std::string& Paragraph::text() const
{
  return m_text;
}

inline size_t Paragraph::length() const
{
  return text().length();
}

inline const std::vector<std::shared_ptr<ParagraphMetaData>>& Paragraph::metadata() const
{
  return m_metadata;
}

template<typename T, typename...Args>
inline void Paragraph::add(ParagraphRange pr, Args&& ... args)
{
  using Selector = typename std::conditional<std::is_base_of<ParagraphMetaData, T>::value, TypeDerivedFromMetaData, BuildGenericMetaData>::type;
  add_meta_data<T>(Selector{}, pr, std::forward<Args>(args)...);
}

template<typename T, typename...Args>
inline void Paragraph::add_meta_data(TypeDerivedFromMetaData, ParagraphRange pr, Args&& ... args)
{
  auto md = std::make_shared<T>(pr, std::forward<Args>(args)...);
  addMetaData(md);
}

template<typename T, typename...Args>
inline void Paragraph::add_meta_data(BuildGenericMetaData, ParagraphRange pr, Args&& ... args)
{
  auto md = std::make_shared<GenericParagraphMetaData<T>>(pr, T(std::forward<Args>(args)...));
  addMetaData(md);
}

class DEX_MODEL_API Link : public ParagraphMetaData
{
public:
  ~Link() = default;

  Link(const ParagraphRange& pr, std::string url);

  static constexpr model::Kind ClassKind = model::Kind::Link;
  model::Kind kind() const override;

  const std::string& url() const;
  void setUrl(std::string url);

private:
  std::string m_url;
};

inline Link::Link(const ParagraphRange& pr, std::string url)
  : ParagraphMetaData(pr),
  m_url(std::move(url))
{

}

inline const std::string& Link::url() const
{
  return m_url;
}

inline void Link::setUrl(std::string url)
{
  m_url = std::move(url);
}


class DEX_MODEL_API TextStyle : public ParagraphMetaData
{
public:
  TextStyle(const ParagraphRange& pr, std::string style);

  static constexpr model::Kind ClassKind = model::Kind::TextStyle;
  model::Kind kind() const override;

  const std::string& style() const;
  void setStyle(std::string str);

private:
  std::string m_style;
};

inline TextStyle::TextStyle(const ParagraphRange& pr, std::string style)
  : ParagraphMetaData(pr),
  m_style(std::move(style))
{

}

inline const std::string& TextStyle::style() const
{
  return m_style;
}

inline void TextStyle::setStyle(std::string str)
{
  m_style = std::move(str);
}


class DEX_MODEL_API Note : public ParagraphMetaData
{
public:
  ~Note() = default;

  enum Location
  {
    FootNote,
    SideNote,
  };

  Note(const ParagraphRange& pr, std::shared_ptr<Paragraph> par, Location loc = FootNote);

  static constexpr model::Kind ClassKind = model::Kind::Note;
  model::Kind kind() const override;

private:
  std::shared_ptr<Paragraph> m_content;
  Location m_location;
};

inline Note::Note(const ParagraphRange& pr, std::shared_ptr<Paragraph> par, Location loc)
  : ParagraphMetaData(pr),
  m_content(std::move(par)),
  m_location(loc)
{

}


class DEX_MODEL_API ListItem : public DocumentElement
{
public:
  std::string marker;
  int value = -1;
  DomNodeList content;

public:
  ListItem();

  static constexpr model::Kind ClassKind = model::Kind::ListItem;
  model::Kind kind() const override;

  const DomNodeList& childNodes() const override;
  void appendChild(const DocumentNodePtr& node) override;
};

class DEX_MODEL_API List : public DocumentElement
{
public:
  std::string marker;
  bool ordered = false;
  bool reversed = false;
  DomNodeList items;

public:
  explicit List(std::string mark = {});
  ~List() = default;

  static constexpr model::Kind ClassKind = model::Kind::List;
  model::Kind kind() const override;

  const DomNodeList& childNodes() const override;
  void appendChild(const DocumentNodePtr& node) override;
};

class DEX_MODEL_API Image : public DocumentElement
{
public:
  std::string src;
  int width = -1;
  int height = -1;

public:
  ~Image() = default;

  explicit Image(std::string source);
  
  static constexpr model::Kind ClassKind = model::Kind::Image;
  model::Kind kind() const override;
};

} // namespace dex

#endif // DEX_MODEL_DOCUMENT_H
