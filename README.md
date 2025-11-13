```
Compiler
├─ CMakeLists.txt
├─ include
│  ├─ AST.hpp
│  ├─ DFA_Tables.hpp
│  ├─ Lexer.hpp
│  ├─ LL1_Table.hpp
│  └─ Parser.hpp
├─ input
│  ├─ lex_rules.txt
│  ├─ lex_rules_test.txt
│  └─ syntax_rules.txt
├─ README.md
├─ src
│  ├─ AST
│  │  └─ AST.cpp
│  ├─ Lexer
│  │  └─ Lexer.cpp
│  ├─ main.cpp
│  └─ Parser
│     └─ Parser.cpp
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
      │  ├─ Grammar.hpp
      │  ├─ LL1Table.hpp
      │  └─ ParserGenerator.hpp
      └─ source
         ├─ Grammar.cpp
         ├─ LL1Table.cpp
         ├─ ParserGenerator.cpp
         └─ Parser_Generator_main.cpp

```