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
  "\\def\\b{"
  "  \\p@rseword\\c@ll\\@b" // 25
  "}"
  ""
  "\\def\\image{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{" // 30
  "      \\p@rseword\\c@ll\\@image"
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseword\\c@ll\\@image" // 35
  "  }"
  "}"
  ""
  "\\def\\list{"
  "  \\@ifnextchar[{" // 40
  "    \\def\\@fteroptions{"
  "      \\c@ll\\@list"
  "    }"
  "    \\p@rseoptions"
  "  }{" // 45
  "    \\c@ll\\@list"
  "  }"
  "}"
  ""
  "\\def\\li{" // 50
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\c@ll\\@li"
  "    }"
  "    \\p@rseoptions" // 55
  "  }{"
  "    \\c@ll\\@li"
  "  }"
  "}"
  "" // 60
  "% Dex - Program %"
  ""
  "\\def\\class{"
  "  \\p@rseline\\c@ll\\cl@ss"
  "}" // 65
  ""
  "\\def\\namespace{"
  "  \\p@rseline\\c@ll\\n@mesp@ce"
  "}"
  "" // 70
  "\\def\\fn{"
  "  \\p@rseline\\c@ll\\functi@n"
  "}"
  ""
  "\\def\\variable{" // 75
  "  \\p@rseline\\c@ll\\v@ri@ble"
  "}"
  ""
  "\\def\\enum{"
  "  \\p@rseline\\c@ll\\@enum" // 80
  "}"
  ""
  "\\def\\value{"
  "  \\@ifnextchar[{"
  "    \\def\\@afteroptions{" // 85
  "      \\p@rseword\\c@ll\\enumv@lue"
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseword\\c@ll\\enumv@lue" // 90
  "  }"
  "}"
  ""
  "\\def\\brief{"
  "  \\p@rseline\\c@ll\\@brief" // 95
  "}"
  ""
  "\\def\\param{"
  "  \\p@rseline\\c@ll\\p@r@m"
  "}" // 100
  ""
  "\\def\\returns{"
  "  \\p@rseline\\c@ll\\@returns"
  "}"
  "" // 105
  "\\def\\since{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\@ifleftbrace{"
  "        \\p@rseword\\c@ll\\@since" // 110
  "      }{"
  "        \\beginsince"
  "      }"
  "    }"
  "    \\p@rseoptions" // 115
  "  }{"
  "    \\p@rseline\\c@ll\\@since"
  "  }"
  "}"
  "" // 120
  "% Dex - Manual %"
  ""
  "\\def\\manual{"
  "  \\p@rseline\\c@ll\\@manual"
  "}" // 125
  ""
  "\\def\\part{"
  "  \\p@rseline\\c@ll\\@part"
  "}"
  "" // 130
  "\\def\\chapter{"
  "  \\p@rseline\\c@ll\\@chapter"
  "}"
  ""
  "\\def\\section{" // 135
  "  \\p@rseline\\c@ll\\@section"
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
  "d1r"
  ">2c9c4c2" // 25
  "s"
  ""
  "d5r"
  ">2c;tt"
  ">4c3c<t" // 30
  ">6c9c4c6"
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c6" // 35
  ">2t"
  "s"
  ""
  "d4r"
  ">2c;tt" // 40
  ">4c3c<t"
  ">6c4c5"
  ">4t"
  ">4c<"
  ">2tt" // 45
  ">4c4c5"
  ">2t"
  "s"
  ""
  "d2r" // 50
  ">2c;tt"
  ">4c3c<t"
  ">6c4c3"
  ">4t"
  ">4c<" // 55
  ">2tt"
  ">4c4c3"
  ">2t"
  "s"
  "" // 60
  ">A"
  ""
  "d5r"
  ">2c9c4c5"
  "s" // 65
  ""
  "d9r"
  ">2c9c4c9"
  "s"
  "" // 70
  "d2r"
  ">2c9c4c8"
  "s"
  ""
  "d8r" // 75
  ">2c9c4c8"
  "s"
  ""
  "d4r"
  ">2c9c4c5" // 80
  "s"
  ""
  "d5r"
  ">2c;tt"
  ">4c3c=t" // 85
  ">6c9c4c9"
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c9" // 90
  ">2t"
  "s"
  ""
  "d5r"
  ">2c9c4c6" // 95
  "s"
  ""
  "d5r"
  ">2c9c4c5"
  "s" // 100
  ""
  "d7r"
  ">2c9c4c8"
  "s"
  "" // 105
  "d5r"
  ">2c;tt"
  ">4c3c<t"
  ">6c<t"
  ">8c9c4c6" // 110
  ">6tt"
  ">8c:"
  ">6t"
  ">4t"
  ">4c<" // 115
  ">2tt"
  ">4c9c4c6"
  ">2t"
  "s"
  "" // 120
  ">@"
  ""
  "d6r"
  ">2c9c4c7"
  "s" // 125
  ""
  "d4r"
  ">2c9c4c5"
  "s"
  "" // 130
  "d7r"
  ">2c9c4c8"
  "s"
  ""
  "d7r" // 135
  ">2c9c4c8"
  "s"
  ""
  "e";


namespace dex
{
const tex::parsing::Format DexFormat = tex::parsing::Format(fmt_src, fmt_bytecode);

} // namespace dex
