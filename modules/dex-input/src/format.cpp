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
  "\\def\\image{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{" // 50
  "      \\p@rseword\\c@ll\\@image"
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseword\\c@ll\\@image" // 55
  "  }"
  "}"
  ""
  "\\def\\list{"
  "  \\@ifnextchar[{" // 60
  "    \\def\\@fteroptions{"
  "      \\c@ll\\@list"
  "    }"
  "    \\p@rseoptions"
  "  }{" // 65
  "    \\c@ll\\@list"
  "  }"
  "}"
  ""
  "\\def\\li{" // 70
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\c@ll\\@li"
  "    }"
  "    \\p@rseoptions" // 75
  "  }{"
  "    \\c@ll\\@li"
  "  }"
  "}"
  "" // 80
  "% Dex - Program %"
  ""
  "\\def\\class{"
  "  \\p@rseline\\c@ll\\cl@ss"
  "}" // 85
  ""
  "\\def\\namespace{"
  "  \\p@rseline\\c@ll\\n@mesp@ce"
  "}"
  "" // 90
  "\\def\\fn{"
  "  \\p@rseline\\c@ll\\functi@n"
  "}"
  ""
  "\\def\\variable{" // 95
  "  \\p@rseline\\c@ll\\v@ri@ble"
  "}"
  ""
  "\\def\\enum{"
  "  \\p@rseline\\c@ll\\@enum" // 100
  "}"
  ""
  "\\def\\value{"
  "  \\@ifnextchar[{"
  "    \\def\\@afteroptions{" // 105
  "      \\p@rseword\\c@ll\\enumv@lue"
  "    }"
  "    \\p@rseoptions"
  "  }{"
  "    \\p@rseword\\c@ll\\enumv@lue" // 110
  "  }"
  "}"
  ""
  "\\def\\brief{"
  "  \\p@rseline\\c@ll\\@brief" // 115
  "}"
  ""
  "\\def\\param{"
  "  \\p@rseline\\c@ll\\p@r@m"
  "}" // 120
  ""
  "\\def\\returns{"
  "  \\p@rseline\\c@ll\\@returns"
  "}"
  "" // 125
  "\\def\\since{"
  "  \\@ifnextchar[{"
  "    \\def\\@fteroptions{"
  "      \\@ifleftbrace{"
  "        \\p@rseword\\c@ll\\@since" // 130
  "      }{"
  "        \\beginsince"
  "      }"
  "    }"
  "    \\p@rseoptions" // 135
  "  }{"
  "    \\p@rseline\\c@ll\\@since"
  "  }"
  "}"
  "" // 140
  "% Dex - Manual %"
  ""
  "\\def\\manual{"
  "  \\p@rseline\\c@ll\\@manual"
  "}" // 145
  ""
  "\\def\\part{"
  "  \\p@rseline\\c@ll\\@part"
  "}"
  "" // 150
  "\\def\\chapter{"
  "  \\p@rseline\\c@ll\\@chapter"
  "}"
  ""
  "\\def\\section{" // 155
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
  "d5r"
  ">2c;tt"
  ">4c3c<t" // 50
  ">6c9c4c6"
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c6" // 55
  ">2t"
  "s"
  ""
  "d4r"
  ">2c;tt" // 60
  ">4c3c<t"
  ">6c4c5"
  ">4t"
  ">4c<"
  ">2tt" // 65
  ">4c4c5"
  ">2t"
  "s"
  ""
  "d2r" // 70
  ">2c;tt"
  ">4c3c<t"
  ">6c4c3"
  ">4t"
  ">4c<" // 75
  ">2tt"
  ">4c4c3"
  ">2t"
  "s"
  "" // 80
  ">A"
  ""
  "d5r"
  ">2c9c4c5"
  "s" // 85
  ""
  "d9r"
  ">2c9c4c9"
  "s"
  "" // 90
  "d2r"
  ">2c9c4c8"
  "s"
  ""
  "d8r" // 95
  ">2c9c4c8"
  "s"
  ""
  "d4r"
  ">2c9c4c5" // 100
  "s"
  ""
  "d5r"
  ">2c;tt"
  ">4c3c=t" // 105
  ">6c9c4c9"
  ">4t"
  ">4c<"
  ">2tt"
  ">4c9c4c9" // 110
  ">2t"
  "s"
  ""
  "d5r"
  ">2c9c4c6" // 115
  "s"
  ""
  "d5r"
  ">2c9c4c5"
  "s" // 120
  ""
  "d7r"
  ">2c9c4c8"
  "s"
  "" // 125
  "d5r"
  ">2c;tt"
  ">4c3c<t"
  ">6c<t"
  ">8c9c4c6" // 130
  ">6tt"
  ">8c:"
  ">6t"
  ">4t"
  ">4c<" // 135
  ">2tt"
  ">4c9c4c6"
  ">2t"
  "s"
  "" // 140
  ">@"
  ""
  "d6r"
  ">2c9c4c7"
  "s" // 145
  ""
  "d4r"
  ">2c9c4c5"
  "s"
  "" // 150
  "d7r"
  ">2c9c4c8"
  "s"
  ""
  "d7r" // 155
  ">2c9c4c8"
  "s"
  ""
  "e";


namespace dex
{
const tex::parsing::Format DexFormat = tex::parsing::Format(fmt_src, fmt_bytecode);

} // namespace dex
