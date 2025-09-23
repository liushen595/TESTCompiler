```
compiler
├─ CMakeLists.txt
├─ include
│  └─ Lexer.hpp
├─ input
│  └─ lex_rules.txt
├─ README.md
├─ src
│  ├─ AST
│  ├─ Lexer
│  │  └─ Lexer.cpp
│  ├─ main.cpp
│  └─ Parser
├─ tests
│  ├─ lexer
│  │  └─ lexer_test.cpp
│  └─ parser
└─ Tools
   ├─ DFA-Generator
   │  ├─ CMakeLists.txt
   │  ├─ header
   │  │  ├─ DFA.hpp
   │  │  ├─ NFA.hpp
   │  │  └─ RegexEngine.hpp
   │  └─ source
   │     ├─ DFA.cpp
   │     ├─ DFA_Generator_main.cpp
   │     ├─ NFA.cpp
   │     └─ RegexEngine.cpp
   └─ Parser-Generator
      ├─ CMakeLists.txt
      ├─ header
      └─ source

```
