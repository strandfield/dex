

# `dex` - C++ documentation

[![Build Status](https://api.travis-ci.com/strandfield/dex.svg?branch=master)](https://travis-ci.com/github/strandfield/dex)
[![Build status](https://ci.appveyor.com/api/projects/status/5psv3b7db6jrt97m?svg=true)](https://ci.appveyor.com/project/strandfield/dex)
[![codecov](https://codecov.io/gh/strandfield/dex/branch/master/graph/badge.svg?token=GfrJWRlYwn)](https://codecov.io/gh/strandfield/dex)

> Wellll, whadya know !

`dex` is an experimental doxygen-inspired documentation tools for C++ programs.

## Documenting the code

### Manual

A simple manual can be generated by:
- running `dex` at the root of this repository
- compiling the generated LaTeX source `dex.tex` in `doc/output`

The generation of this manual is done as part of the AppVeyor 
CI build.
The PDF should therefore be available from the [build artifacts](https://ci.appveyor.com/project/strandfield/dex/build/artifacts).

### Source file documentation

Like doxygen, code is documented using documentation blocks.

```cpp
/*!
 * \fn int add(int a, int b)
 * \brief adds two integers
 * \param the first integer
 * \param the second integer
 */
int add(int a, int b)
{
  return a + b;
}
```

However, unlike doxygen, `dex` does not attempt to parse the C++ code at 
all.
It only sees the documentation blocks, which therefore must specify what 
they are documenting.
(As a consequence, the documentation block does not need to be placed near 
the documented function; the documented function does not even need to 
exist for that matter)

### dex.yml

`dex` must be run in a directory containing a `dex.yml` file describing the 
inputs files/directories and the output directory.

Example:

```yaml
input: 
  - doc/src/manual.dex
output: doc
variables:
  project:
    name: dex
```

### Output directory

The output pipeline is inspired by [Jekyll](https://jekyllrb.com/), a static 
website generator.

The pipeline uses a "template" output directory that is used to generate the 
output.

Files and directories in the template that starts with an underscore are 
used to configure the output engine.

Files that do not start with a "front-matter" are directly copied to the 
real output directory.

Files that start with a "front-matter" are processed by the output engine.
Like Jekyll, the output engine supports a language similar to Shopify's 
[liquid](https://shopify.dev/api/liquid) language.

Example of markdown file with a front-matter:

```
---
layout: default
---
# Documentation

Welcome !

{% if project and project.name %}
This is {{ project.name }}!
{% endif %}
```

## TODO: refactoring

The `dex` parser design is inspired by TeX, but the inspiration goes too far.

As a consequence, the code is hard to understand, buggy, and error messages 
are sometimes cryptic, badly localized.

See [REFACTOR.md](REFACTOR.md) for some refactoring ideas that may be 
implemented in the future.
