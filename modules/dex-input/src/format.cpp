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
  "\\def\\e{"
  "  \\p@rseword\\c@ll\\@e"
  "}"
  "" // 35
  "\\def\\textit#1{"
  "  \\@begintextit #1\\@endtextit"
  "}"
  ""
  "\\def\\c{" // 40
  "  \\p@rseword\\c@ll\\@c"
  "}"
  ""
  "\\def\\texttt#1{"
  "  \\@begintexttt #1\\@endtexttt" // 45
  "}"
  ""
  "\\def\\href{"
  "  \\p@rseword\\p@rseword\\c@ll\\@href"
  "}" // 50
  ""
  "\\def\\image{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\p@rseword\\c@ll\\@image" // 55
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseword\\c@ll\\@image"
  "  }" // 60
  "}"
  ""
  "\\def\\list{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{" // 65
  "      \\c@ll\\@list"
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\c@ll\\@list" // 70
  "  }"
  "}"
  ""
  "\\def\\li{"
  "  \\@ifnextchar[{" // 75
  "    \\def\\@fteroptions{"
  "      \\c@ll\\@li"
  "    }"
  "    \\p@rseoptions"
  "  }{" // 80
  "    \\c@ll\\@li"
  "  }"
  "}"
  ""
  "% Dex - Program %" // 85
  ""
  "\\def\\class{"
  "  \\p@rseline\\c@ll\\cl@ss"
  "}"
  "" // 90
  "\\def\\namespace{"
  "  \\p@rseline\\c@ll\\n@mesp@ce"
  "}"
  ""
  "\\def\\fn{" // 95
  "  \\p@rseline\\c@ll\\functi@n"
  "}"
  ""
  "\\def\\variable{"
  "  \\p@rseline\\c@ll\\v@ri@ble" // 100
  "}"
  ""
  "\\def\\enum{"
  "  \\p@rseline\\c@ll\\@enum"
  "}" // 105
  ""
  "\\def\\value{"
  "  \\@ifnextchar[{"
  "    \\def\\@afteroptions{"
  "      \\p@rseword\\c@ll\\enumv@lue" // 110
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseword\\c@ll\\enumv@lue"
  "  }" // 115
  "}"
  ""
  "\\def\\brief{"
  "  \\p@rseline\\c@ll\\@brief"
  "}" // 120
  ""
  "\\def\\param{"
  "  \\p@rseline\\c@ll\\p@r@m"
  "}"
  "" // 125
  "\\def\\returns{"
  "  \\p@rseline\\c@ll\\@returns"
  "}"
  ""
  "\\def\\since{" // 130
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\@ifleftbrace{"
  "        \\p@rseword\\c@ll\\@since"
  "      }{" // 135
  "        \\beginsince"
  "      }"
  "    }"
  "    \\p@rseoptions"
  "  }{" // 140
  "    \\p@rseline\\c@ll\\@since"
  "  }"
  "}"
  ""
  "% Dex - Manual %" // 145
  ""
  "\\def\\manual{"
  "  \\p@rseline\\c@ll\\@manual"
  "}"
  "" // 150
  "\\def\\part{"
  "  \\p@rseline\\c@ll\\@part"
  "}"
  ""
  "\\def\\chapter{" // 155
  "  \\p@rseline\\c@ll\\@chapter"
  "}"
  ""
  "\\def\\section{"
  "  \\p@rseline\\c@ll\\@section" // 160
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
  "d1r"
  ">2c9c4c2"
  "s"
  "" // 35
  "d6pr"
  ">2c<>1pc:"
  "s"
  ""
  "d1r" // 40
  ">2c9c4c2"
  "s"
  ""
  "d6pr"
  ">2c<>1pc:" // 45
  "s"
  ""
  "d4r"
  ">2c9c9c4c5"
  "s" // 50
  ""
  "d5r"
  ">2c;tt"
  ">4c3c<t"
  ">6c9c4c6" // 55
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c6"
  ">2t" // 60
  "s"
  ""
  "d4r"
  ">2c;tt"
  ">4c3c<t" // 65
  ">6c4c5"
  ">4t"
  ">4c<"
  ">2tt"
  ">4c4c5" // 70
  ">2t"
  "s"
  ""
  "d2r"
  ">2c;tt" // 75
  ">4c3c<t"
  ">6c4c3"
  ">4t"
  ">4c<"
  ">2tt" // 80
  ">4c4c3"
  ">2t"
  "s"
  ""
  ">A" // 85
  ""
  "d5r"
  ">2c9c4c5"
  "s"
  "" // 90
  "d9r"
  ">2c9c4c9"
  "s"
  ""
  "d2r" // 95
  ">2c9c4c8"
  "s"
  ""
  "d8r"
  ">2c9c4c8" // 100
  "s"
  ""
  "d4r"
  ">2c9c4c5"
  "s" // 105
  ""
  "d5r"
  ">2c;tt"
  ">4c3c=t"
  ">6c9c4c9" // 110
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c9"
  ">2t" // 115
  "s"
  ""
  "d5r"
  ">2c9c4c6"
  "s" // 120
  ""
  "d5r"
  ">2c9c4c5"
  "s"
  "" // 125
  "d7r"
  ">2c9c4c8"
  "s"
  ""
  "d5r" // 130
  ">2c;tt"
  ">4c3c<t"
  ">6c<t"
  ">8c9c4c6"
  ">6tt" // 135
  ">8c:"
  ">6t"
  ">4t"
  ">4c<"
  ">2tt" // 140
  ">4c9c4c6"
  ">2t"
  "s"
  ""
  ">@" // 145
  ""
  "d6r"
  ">2c9c4c7"
  "s"
  "" // 150
  "d4r"
  ">2c9c4c5"
  "s"
  ""
  "d7r" // 155
  ">2c9c4c8"
  "s"
  ""
  "d7r"
  ">2c9c4c8" // 160
  "s"
  ""
  "e";


namespace dex
{
const tex::parsing::Format DexFormat = tex::parsing::Format(fmt_src, fmt_bytecode);
} // namespace dex
