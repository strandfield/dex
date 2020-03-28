// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/format.h"

#include <tex/lexer.h>

#include <cassert>
#include <stdexcept>

const char* fmt_src =
  "" // 0
  ""
  "% Input conditionals"
  ""
  "\\def\\@ifnextchar#1#2#3{"
  "  \\testnextch@r #1 \\ifbr #2 \\else #3 \\fi" // 5
  "}"
  ""
  "\\def\\@ifleftbrace#1#2{"
  "  \\testleftbr@ce \\ifbr #1 \\else #2 \\fi"
  "}" // 10
  ""
  "% Environment"
  ""
  "\\def\\begin#1{"
  "  \\csname #1 \\endcsname" // 15
  "}"
  ""
  "\\def\\end#1{"
  "  \\csname end#1\\endcsname"
  "}" // 20
  ""
  "% Dex"
  ""
  "\\def\\class{"
  "  \\p@rseline\\c@ll\\cl@ss" // 25
  "}"
  ""
  "\\def\\namespace{"
  "  \\p@rseline\\c@ll\\n@mesp@ce"
  "}" // 30
  ""
  "\\def\\fn{"
  "  \\p@rseline\\c@ll\\functi@n"
  "}"
  "" // 35
  "\\def\\brief{"
  "  \\p@rseline\\c@ll\\@brief"
  "}"
  ""
  "\\def\\param{" // 40
  "  \\p@rseline\\c@ll\\p@r@m"
  "}"
  ""
  "\\def\\returns{"
  "  \\p@rseline\\c@ll\\@returns" // 45
  "}"
  ""
  "\\def\\since{"
  "  \\@ifnextchar[{"
  "    \\def\\@afteroptions{" // 50
  "      \\@ifleftbrace{"
  "        \\p@rseword\\c@ll\\@since"
  "      }{"
  "        \\beginsince"
  "      }" // 55
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseline\\c@ll\\@since"
  "  }" // 60
  "}"
  ""
;

const char* fmt_bytecode =
  "v0"
  "" // 0
  ""
  ">D"
  ""
  "d;pppr"
  ">2c<>1p>1c4>1p>1c4>1p>1c2" // 5
  "s"
  ""
  "d<ppr"
  ">2c=>1c4>1p>1c4>1p>1c2"
  "s" // 10
  ""
  ">="
  ""
  "d5pr"
  ">2c6>1p>1c9" // 15
  "s"
  ""
  "d3pr"
  ">2c6>1tttpc9"
  "s" // 20
  ""
  ">5"
  ""
  "d5r"
  ">2c9c4c5" // 25
  "s"
  ""
  "d9r"
  ">2c9c4c9"
  "s" // 30
  ""
  "d2r"
  ">2c9c4c8"
  "s"
  "" // 35
  "d5r"
  ">2c9c4c6"
  "s"
  ""
  "d5r" // 40
  ">2c9c4c5"
  "s"
  ""
  "d7r"
  ">2c9c4c8" // 45
  "s"
  ""
  "d5r"
  ">2c;tt"
  ">4c3c=t" // 50
  ">6c<t"
  ">8c9c4c6"
  ">6tt"
  ">8c:"
  ">6t" // 55
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c6"
  ">2t" // 60
  "s"
  ""
  "e";

namespace dex
{
const tex::parsing::Format DexFormat = tex::parsing::Format(fmt_src, fmt_bytecode);

} // namespace dex
