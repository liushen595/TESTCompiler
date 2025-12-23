```
Compiler
├─ CMakeLists.txt
├─ include
│  ├─ DFA_Tables.hpp
│  ├─ Lexer.hpp
│  ├─ Parser.hpp
│  ├─ Semantic.hpp
│  └─ Simulator.hpp
├─ input
│  ├─ lex_rules.txt
│  ├─ lex_rules_test.txt
│  └─ syntax_rules.txt
├─ README.md
├─ src
│  ├─ Lexer
│  │  └─ Lexer.cpp
│  ├─ main.cpp
│  ├─ Parser
│  │  └─ Parser.cpp
│  ├─ Semantic
│  │  └─ Semantic.cpp
│  └─ Simulator
│     └─ Simulator.cpp
├─ tests
│  └─ lexer
│     └─ lexer_test.cpp
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
      └─ CMakeLists.txt

```