// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/format.h"

#include "dex/common/file-utils.h"

#include <tex/lexer.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

const char* dex_fmt_content = R"(
% Input conditionals

\def\@ifnextchar#1#2#3{%
  \testnextch@r #1\ifbr #2\else #3\fi
}

\def\@ifleftbrace#1#2{%
  \testleftbr@ce \ifbr #1\else #2\fi
}

% Input

\def\input{%
  \p@rseword\c@ll\@input
}

% Environment

\def\begin#1{%
  \csname #1\endcsname
}

\def\end#1{%
  \csname end#1\endcsname
}

% Document

\def\b{%
  \p@rseword\c@ll\@b
}

\def\textbf#1{%
  \@begintextbf #1\@endtextbf
}

\def\e{%
  \p@rseword\c@ll\@e
}

\def\textit#1{%
  \@begintextit #1\@endtextit
}

\def\c{%
  \p@rseword\c@ll\@c
}

\def\texttt#1{%
  \@begintexttt #1\@endtexttt
}

\def\href{%
  \p@rseword\p@rseword\c@ll\@href
}

\def\image{%
  \@ifnextchar[{%
    \def\@fteroptions{%
      \p@rseword\c@ll\@image
    }%
    \p@rseoptions
  }{%
    \p@rseword\c@ll\@image
  }%
}

\def\list{%
  \@ifnextchar[{%
    \def\@fteroptions{%
      \c@ll\@list
    }%
    \p@rseoptions
  }{%
    \c@ll\@list
  }%
}

\def\li{%
  \@ifnextchar[{%
    \def\@fteroptions{%
      \c@ll\@li
    }%
    \p@rseoptions
  }{%
    \c@ll\@li
  }%
}

\def\makegrouptable{%
  \@ifnextchar[{%
    \def\@fteroptions{%
      \p@rseword\c@ll\@makegrouptable
    }%
    \p@rseoptions
  }{%
    \p@rseword\c@ll\@makegrouptable
  }%
}

\def\code{%
  \@ifnextchar[{%
    \def\@fteroptions{%
      \c@ll\@code
    }%
    \p@rseoptions
  }{%
    \c@ll\@code
  }%
}

% Dex - Program %

\def\class{%
  \p@rseline\c@ll\cl@ss
}

\def\namespace{%
  \p@rseline\c@ll\n@mesp@ce
}

\def\fn{%
  \p@rseline\c@ll\functi@n
}

\def\variable{%
  \p@rseline\c@ll\v@ri@ble
}

\def\enum{%
  \p@rseline\c@ll\@enum
}

\def\value{%
  \@ifnextchar[{%
    \def\@afteroptions{%
      \p@rseword\c@ll\enumv@lue
    }%
    \p@rseoptions
  }{%
    \p@rseword\c@ll\enumv@lue
  }%
}

\def\typedef{%
  \p@rseline\c@ll\@typedef
}

\def\macro{%
  \p@rseline\c@ll\@macro
}

\def\brief{%
  \p@rseline\c@ll\@brief
}

\def\param{%
  \p@rseline\c@ll\p@r@m
}

\def\returns{%
  \p@rseline\c@ll\@returns
}

\def\relates{%
  \p@rseline\c@ll\@relates
}

\def\since{%
  \@ifnextchar[{%
    \def\@fteroptions{%
      \@ifleftbrace{%
        \p@rseword\c@ll\@since
      }{%
        \c@ll\beginsince
      }%
    }%
    \p@rseoptions
  }{%
    \p@rseline\c@ll\@since
  }%
}

% Dex - Documents %

\def\manual{%
  \p@rseline\c@ll\@manual
}

\def\page{%
  \p@rseline\c@ll\@page
}

\def\frontmatter{%
  \@frontmatter
}

\def\mainmatter{%
  \@mainmatter
}

\def\backmatter{%
  \@backmatter
}

\def\part{%
  \p@rseline\c@ll\@part
}

\def\chapter{%
  \p@rseline\c@ll\@chapter
}

\def\section{%
  \p@rseline\c@ll\@section
}

\def\tableofcontents{%
  \@tableofcontents
}

\def\makeindex{%
  \@makeindex
}

\def\index{%
  \p@rseword\c@ll\@index
}

\def\printindex{%
  \@printindex
}

% Dex - Grouping %

\def\ingroup{%
  \p@rseline\c@ll\@ingroup
}
)";

std::vector<tex::parsing::Macro> DexFormat::load()
{
  std::string source = dex_fmt_content;
  file_utils::crlf2lf(source);
  return tex::parsing::Format::parse(source);
}

} // namespace dex
