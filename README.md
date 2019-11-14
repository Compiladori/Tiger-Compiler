# Tiger-Compiler
A Tiger compiler for the x86 architecture, implemented in C++, as described in the book "Modern Compiler Implementation" by Andrew Appel. 

## Compiler steps
1. Tokenize the source code and build the corresponding AST (Syntax, AST)
2. Set escaping variables in the AST (Escapes)
3. Semantic check (Semantic)
4. Translation of the AST into the IRT (IRT, Frame, Translation)
5. Canonization of the IRT (Canon)
6. Tiling of the IRT into assembly instructions, known as munch (Munch)
