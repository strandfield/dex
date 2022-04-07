
# Refactoring

The parser class is too complicated.

It's design is inspired by TeX; the inspiration goes too far.
For example, every character generates a token and even the basic commands 
like `class` are macros.
It is hard to understand the state of the parsing at the end of the 
"parsing pipeline".
The leads to incoherent state that is to understand and to cryptic error 
messages.
Another consequence is that the input must adhere to a strict syntax, with 
little chance for the parser to self-correct the input.

## Reworking the parsing classes

The number of "parser" classes should be reduced and have a more 
"library" friendly desing (as opposed to an "app" design with little
extensibility).

Possible top-level parser class:

```cpp
class Parser
{
public:

  // parse a file or folder
  void parse(std::filesystem::path& path);

protected:
  // function called at the beginning of each file
  virtual void file_begin();
  // function called at the end of each file
  virtual void file_end();

  // function called at the beginning & end of each block
  virtual void block_begin();
  virtual void block_end();

  // ...etc...

  // specific parsers are used depending on the context;
  // for example, when the first token in a block is \fn
  // a ProgramParser is used to parse the block.
  // The ProgramParser holds a reference to the Parser   
  // class and can itself create subparsers
  virtual ParagraphParser* newParagraphParser();
  virtual ProgramParser* newProgramParser();

private:
  // Holds the token streams, the documents, macro expansions
  // and the stack of nodes (e.g. "program" nodes like classes
  // or document nodes like a paragraph)
  ParsingContext* m_context;
};
``` 

When parsing a file, the whole file is loaded is memory in a std::string
and the tokens use a string view.

Even when multiple documents are in memory at the same time (e.g., because 
of `input`), it is still relatively easy to match a token with the document 
by comparing the string view pointer with the document's std::string memory 
block.

When reaching a block, the whole block is tokenized.

```cpp
class Token
{
public:
  std::string_view str;
  TokenType type;
};
```

Tokens are either "words", "punctuator", "bgroup", "egroup", "command", "newline" 
or "param".
The type of the tokens is decided when the document is fully tokenized, 
but the parser can change the type while parsing (if needed, hopefully should 
be rare).

Note "newline" tokens are usually ignored but they may be useful in some 
situtations. 
"bgroup" = `{`, "egroup"= `}`

We have 3 token "streams":
- the "raw" document tokens 
- the token "preparation" stream
- the parser stream

The first stream of tokens is the tokens from the block, plus the tokens 
of `input` documents.

The second stream consists of tokens that are expanded if they are part 
of a macro.

The last stream is what is consumed by the parser.

The streams are cleared at the end of each block.
(They could also be cleared whenever a "top-level" command has 
completed)

When the parser wants to read a token, several things can happen:
if the parser stream is not at end, a token is read from there 
directly (by incrementing the "read iterator");
if no token is available in this stream, a token is taken from 
the "preparation stream".

If the preparation stream is empty, tokens are read from 
the first stream until a macro need to be expanded.
(unless the global "noexpand" param is active, in such case 
the tokens are forwarded to the parser stream).

The `Preprocessor` (or `MacroExpander`), maintains a list of expansions.
An expansion is:
- [in] a set a tokens
- [out] a std::string of the expanded tokens
- [out] a set of tokens (with their string view built using the std::string)

With that list, it is possible to trace back a token from the parser stream 
to all its expansions and ultimately to the source document.
(this will allow the production of detailled errors messages)

Note that some basic expansions like `\begin{code}` -> `\code` 
may be omitted in a future version to save memory.

When the "preparation" streams need tokens, it takes them from the 
"raw" stream.
If the raw stream current token is an `input` control sequence, 
the referenced document is loaded and its tokens are inserted 
in the stream right after the `input` token (which is skipped).

The `DocumentLoader` (or whatever is it called, maybe no class 
is needed and `Parser` can do the job) maintains an "include" tree 
so that it is possible to reattribute the tokens from the raw stream 
to their respective document and better understand how the tokens 
where produced.

The parser recursively reads tokens and produces a parsing tree 
(program nodes, paragraph, etc...).
Basically each control sequence is a function in some parser and 
that function reads the token until it is done.

Some token reading functions that the parser classe may have:
- `read()`: reads a token, possibly by processing them from streams above
- `has_available_tokens()`: wheter tokens are readily available
- `peek()`
- `at_end()`: returns whether all streams are exhausted

We need to have a "variable" system so that some values can be injected 
in the commands and or text.
For example, a value defined as "projectname: toto" in the Yaml
may be available as `\the\projectname` in the documentation block
and with commands `\drawtext[text=2*\projectname]`.
Also, variables could be defined in a block.
`\set\price={10}`
These things are still a little bit unclear...
