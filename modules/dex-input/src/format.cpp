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
  "% Document"
  ""
  "\\def\\image{"
  "  \\@ifnextchar[{" // 25
  "    \\def\\@fteroptions{"
  "      \\p@rseword\\c@ll\\@image"
  "    }"
  "    \\p@rseoptions"
  "  }{" // 30
  "    \\p@rseword\\c@ll\\@image"
  "  }"
  "}"
  ""
  "\\def\\list{" // 35
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\c@ll\\@list"
  "    }"
  "    \\p@rseoptions" // 40
  "  }{"
  "    \\c@ll\\@list"
  "  }"
  "}"
  "" // 45
  "\\def\\li{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\c@ll\\@li"
  "    }" // 50
  "    \\p@rseoptions"
  "  }{"
  "    \\c@ll\\@li"
  "  }"
  "}" // 55
  ""
  "% Dex"
  ""
  "\\def\\class{"
  "  \\p@rseline\\c@ll\\cl@ss" // 60
  "}"
  ""
  "\\def\\namespace{"
  "  \\p@rseline\\c@ll\\n@mesp@ce"
  "}" // 65
  ""
  "\\def\\fn{"
  "  \\p@rseline\\c@ll\\functi@n"
  "}"
  "" // 70
  "\\def\\brief{"
  "  \\p@rseline\\c@ll\\@brief"
  "}"
  ""
  "\\def\\param{" // 75
  "  \\p@rseline\\c@ll\\p@r@m"
  "}"
  ""
  "\\def\\returns{"
  "  \\p@rseline\\c@ll\\@returns" // 80
  "}"
  ""
  "\\def\\since{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{" // 85
  "      \\@ifleftbrace{"
  "        \\p@rseword\\c@ll\\@since"
  "      }{"
  "        \\beginsince"
  "      }" // 90
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseline\\c@ll\\@since"
  "  }" // 95
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
  ">:"
  ""
  "d5r"
  ">2c;tt" // 25
  ">4c3c<t"
  ">6c9c4c6"
  ">4t"
  ">4c<"
  ">2tt" // 30
  ">4c9c4c6"
  ">2t"
  "s"
  ""
  "d4r" // 35
  ">2c;tt"
  ">4c3c<t"
  ">6c4c5"
  ">4t"
  ">4c<" // 40
  ">2tt"
  ">4c4c5"
  ">2t"
  "s"
  "" // 45
  "d2r"
  ">2c;tt"
  ">4c3c<t"
  ">6c4c3"
  ">4t" // 50
  ">4c<"
  ">2tt"
  ">4c4c3"
  ">2t"
  "s" // 55
  ""
  ">5"
  ""
  "d5r"
  ">2c9c4c5" // 60
  "s"
  ""
  "d9r"
  ">2c9c4c9"
  "s" // 65
  ""
  "d2r"
  ">2c9c4c8"
  "s"
  "" // 70
  "d5r"
  ">2c9c4c6"
  "s"
  ""
  "d5r" // 75
  ">2c9c4c5"
  "s"
  ""
  "d7r"
  ">2c9c4c8" // 80
  "s"
  ""
  "d5r"
  ">2c;tt"
  ">4c3c<t" // 85
  ">6c<t"
  ">8c9c4c6"
  ">6tt"
  ">8c:"
  ">6t" // 90
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c6"
  ">2t" // 95
  "s"
  ""
  "e";

namespace dex
{
const tex::parsing::Format DexFormat = tex::parsing::Format(fmt_src, fmt_bytecode);

} // namespace dex
