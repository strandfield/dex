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
  "\\def\\textbf#1{"
  "  \\@begintextbf #1\\@endtextbf"
  "}" // 30
  ""
  "\\def\\image{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\p@rseword\\c@ll\\@image" // 35
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseword\\c@ll\\@image"
  "  }" // 40
  "}"
  ""
  "\\def\\list{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{" // 45
  "      \\c@ll\\@list"
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\c@ll\\@list" // 50
  "  }"
  "}"
  ""
  "\\def\\li{"
  "  \\@ifnextchar[{" // 55
  "    \\def\\@fteroptions{"
  "      \\c@ll\\@li"
  "    }"
  "    \\p@rseoptions"
  "  }{" // 60
  "    \\c@ll\\@li"
  "  }"
  "}"
  ""
  "% Dex - Program %" // 65
  ""
  "\\def\\class{"
  "  \\p@rseline\\c@ll\\cl@ss"
  "}"
  "" // 70
  "\\def\\namespace{"
  "  \\p@rseline\\c@ll\\n@mesp@ce"
  "}"
  ""
  "\\def\\fn{" // 75
  "  \\p@rseline\\c@ll\\functi@n"
  "}"
  ""
  "\\def\\variable{"
  "  \\p@rseline\\c@ll\\v@ri@ble" // 80
  "}"
  ""
  "\\def\\enum{"
  "  \\p@rseline\\c@ll\\@enum"
  "}" // 85
  ""
  "\\def\\value{"
  "  \\@ifnextchar[{"
  "    \\def\\@afteroptions{"
  "      \\p@rseword\\c@ll\\enumv@lue" // 90
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseword\\c@ll\\enumv@lue"
  "  }" // 95
  "}"
  ""
  "\\def\\brief{"
  "  \\p@rseline\\c@ll\\@brief"
  "}" // 100
  ""
  "\\def\\param{"
  "  \\p@rseline\\c@ll\\p@r@m"
  "}"
  "" // 105
  "\\def\\returns{"
  "  \\p@rseline\\c@ll\\@returns"
  "}"
  ""
  "\\def\\since{" // 110
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\@ifleftbrace{"
  "        \\p@rseword\\c@ll\\@since"
  "      }{" // 115
  "        \\beginsince"
  "      }"
  "    }"
  "    \\p@rseoptions"
  "  }{" // 120
  "    \\p@rseline\\c@ll\\@since"
  "  }"
  "}"
  ""
  "% Dex - Manual %" // 125
  ""
  "\\def\\manual{"
  "  \\p@rseline\\c@ll\\@manual"
  "}"
  "" // 130
  "\\def\\part{"
  "  \\p@rseline\\c@ll\\@part"
  "}"
  ""
  "\\def\\chapter{" // 135
  "  \\p@rseline\\c@ll\\@chapter"
  "}"
  ""
  "\\def\\section{"
  "  \\p@rseline\\c@ll\\@section" // 140
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
  "d6pr"
  ">2c<>1pc:"
  "s" // 30
  ""
  "d5r"
  ">2c;tt"
  ">4c3c<t"
  ">6c9c4c6" // 35
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c6"
  ">2t" // 40
  "s"
  ""
  "d4r"
  ">2c;tt"
  ">4c3c<t" // 45
  ">6c4c5"
  ">4t"
  ">4c<"
  ">2tt"
  ">4c4c5" // 50
  ">2t"
  "s"
  ""
  "d2r"
  ">2c;tt" // 55
  ">4c3c<t"
  ">6c4c3"
  ">4t"
  ">4c<"
  ">2tt" // 60
  ">4c4c3"
  ">2t"
  "s"
  ""
  ">A" // 65
  ""
  "d5r"
  ">2c9c4c5"
  "s"
  "" // 70
  "d9r"
  ">2c9c4c9"
  "s"
  ""
  "d2r" // 75
  ">2c9c4c8"
  "s"
  ""
  "d8r"
  ">2c9c4c8" // 80
  "s"
  ""
  "d4r"
  ">2c9c4c5"
  "s" // 85
  ""
  "d5r"
  ">2c;tt"
  ">4c3c=t"
  ">6c9c4c9" // 90
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c9"
  ">2t" // 95
  "s"
  ""
  "d5r"
  ">2c9c4c6"
  "s" // 100
  ""
  "d5r"
  ">2c9c4c5"
  "s"
  "" // 105
  "d7r"
  ">2c9c4c8"
  "s"
  ""
  "d5r" // 110
  ">2c;tt"
  ">4c3c<t"
  ">6c<t"
  ">8c9c4c6"
  ">6tt" // 115
  ">8c:"
  ">6t"
  ">4t"
  ">4c<"
  ">2tt" // 120
  ">4c9c4c6"
  ">2t"
  "s"
  ""
  ">@" // 125
  ""
  "d6r"
  ">2c9c4c7"
  "s"
  "" // 130
  "d4r"
  ">2c9c4c5"
  "s"
  ""
  "d7r" // 135
  ">2c9c4c8"
  "s"
  ""
  "d7r"
  ">2c9c4c8" // 140
  "s"
  ""
  "e";


namespace dex
{
const tex::parsing::Format DexFormat = tex::parsing::Format(fmt_src, fmt_bytecode);

} // namespace dex
