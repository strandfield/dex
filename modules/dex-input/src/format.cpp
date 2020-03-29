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
  "\\def\\list{"
  "  \\@ifnextchar[{" // 25
  "    \\def\\@afteroptions{"
  "      \\c@ll\\@list"
  "    }"
  "    \\p@rseoptions"
  "  }{" // 30
  "    \\c@ll\\@list"
  "  }"
  "}"
  ""
  "\\def\\li{" // 35
  "  \\@ifnextchar[{"
  "    \\def\\@afteroptions{"
  "      \\c@ll\\@li"
  "    }"
  "    \\p@rseoptions" // 40
  "  }{"
  "    \\c@ll\\@li"
  "  }"
  "}"
  "" // 45
  "% Dex"
  ""
  "\\def\\class{"
  "  \\p@rseline\\c@ll\\cl@ss"
  "}" // 50
  ""
  "\\def\\namespace{"
  "  \\p@rseline\\c@ll\\n@mesp@ce"
  "}"
  "" // 55
  "\\def\\fn{"
  "  \\p@rseline\\c@ll\\functi@n"
  "}"
  ""
  "\\def\\brief{" // 60
  "  \\p@rseline\\c@ll\\@brief"
  "}"
  ""
  "\\def\\param{"
  "  \\p@rseline\\c@ll\\p@r@m" // 65
  "}"
  ""
  "\\def\\returns{"
  "  \\p@rseline\\c@ll\\@returns"
  "}" // 70
  ""
  "\\def\\since{"
  "  \\@ifnextchar[{"
  "    \\def\\@afteroptions{"
  "      \\@ifleftbrace{" // 75
  "        \\p@rseword\\c@ll\\@since"
  "      }{"
  "        \\beginsince"
  "      }"
  "    }" // 80
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseline\\c@ll\\@since"
  "  }"
  "}" // 85
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
  "d4r"
  ">2c;tt" // 25
  ">4c3c=t"
  ">6c4c5"
  ">4t"
  ">4c<"
  ">2tt" // 30
  ">4c4c5"
  ">2t"
  "s"
  ""
  "d2r" // 35
  ">2c;tt"
  ">4c3c=t"
  ">6c4c3"
  ">4t"
  ">4c<" // 40
  ">2tt"
  ">4c4c3"
  ">2t"
  "s"
  "" // 45
  ">5"
  ""
  "d5r"
  ">2c9c4c5"
  "s" // 50
  ""
  "d9r"
  ">2c9c4c9"
  "s"
  "" // 55
  "d2r"
  ">2c9c4c8"
  "s"
  ""
  "d5r" // 60
  ">2c9c4c6"
  "s"
  ""
  "d5r"
  ">2c9c4c5" // 65
  "s"
  ""
  "d7r"
  ">2c9c4c8"
  "s" // 70
  ""
  "d5r"
  ">2c;tt"
  ">4c3c=t"
  ">6c<t" // 75
  ">8c9c4c6"
  ">6tt"
  ">8c:"
  ">6t"
  ">4t" // 80
  ">4c<"
  ">2tt"
  ">4c9c4c6"
  ">2t"
  "s" // 85
  ""
  "e";

namespace dex
{
const tex::parsing::Format DexFormat = tex::parsing::Format(fmt_src, fmt_bytecode);

} // namespace dex
