# Tiger-Compiler [![Build Status](https://travis-ci.com/Compiladori/Tiger-Compiler.svg?branch=master)](https://travis-ci.com/Compiladori/Tiger-Compiler)
A Tiger compiler for the x86 architecture, implemented in C++, as described in the book "Modern Compiler Implementation" by Andrew Appel. 

## Compiler steps
1. Tokenize the source code and build the corresponding AST (Syntax, AST)
2. Set escaping variables in the AST (Escapes)
3. Semantic check (Semantic)
4. Translate the AST into the IRT (IRT, Frame, Translation)
5. Canonize the IRT (Canon)
6. Tile out the IRT into generic assembly instructions (Munch)
