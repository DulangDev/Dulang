##  Formal grammatics of Dulang v2
### very informal avout line breaks, only where mandatory

(...) means one or none
[...] means none or any
a{ x || y }b means axb || ayb

all: program, compound, statement, expression, lvalue, 



<program>   ::== <compound>
<compound> ::== (<statement>)[ <statement>  ]
<statement> ::== "return " <expr> "\n"
<statement> ::== "write " <expr> "\n"
<statement> ::== ("if " || "while " ) <expr> "{\n" <compound> "\n}\n" 
<statement> ::=="for " <name> " in " <expression> "{\n" <coumpound> "\n}\n"
<statement> ::== <lvalue> <assign-op> <expression> "\n" || <name>[, <name>] "=" <expression> "\n"
<lvalue> ::== <name> || <lval-subscr>
<lval-subscr> ::== <name> || <name>"[" <exp-plus> "]" || <name> "." <lval-subscr> 
<assign-op> ::== "=" || "+=" || "-=" || "*=" || "/="

motivation of <exp-plus>. subscribe can be performed with either numeric or string value

<statement> ::== <statement-func> || <statement-class>
<statement-func> ::==  "fun " {<name> || <subscr>} "(" (<funcarg>) [, <funcarg>] "){\n" <compound> "\n}\n"
<statement-class> ::== "class "{<name> || <subscr> } "{" [  <statement-func> || <statement-class> ] "\n}\n"
<expression> ::== <classdef> || <funcdef> || <logor>
<logor> ::== <logand> "or" <logand> || <logand>
<funcdef> ::== "fun (" (<funcarg>) [, <funcarg>] "){\n" <compound> "\n}"
<funcdef> ::== "{" (<funcarg>)[, <funcarg>] "->" { (\n) <compound> (\n) || <expression> } "}"
<classdef> ::== "class {\n" <compound> "\n}"
<funcarg> ::== <name> || <name> "=" <literal>
<logand> ::== <lognot> "and <lognot"> || <lognot>
<lognot> ::== "not" <exp-comparison> || <exp-comparison>

its because `a in (1, 2, 3, 4) == 0` means `(a in (1, 2, 3, 4)) == 0`. comparison op makes operand non-collection

<exp-comparison> ::== <exp-in> || <exp-in> {"==" || ">=" || "<=" || "<" || ">"} <exp-in>
<exp-in> ::== <exp-plus> || <exp-unary> "in" {<exp-unary> || <range>}
<range> ::== <exp-plus>".."<exp-plus>

we consider `1 + 2 in arr` as bad code so there are just options to do something like `a.b in arr`
or `"2" in arr`

<exp-plus> ::== <exp-mult> { "+" || "-" } <exp-mult> || <exp-mult>
<exp-mult> ::== <exp-unary> { "+" || "-" || "**" } <exp-unary>

`**` operator is degree operator. `2 ** 3 == 8` -> `true`

<exp-unary> ::== {"." || "+" || "-"} <exp-postfix> || <exp-postfix>
<exp-postfix> ::== <top-level> || <exp-postfix> { "(" || "[" || "{" } <expression> { ")" || "]" "}" } || <exp-postfix> "." <name>
<top-level> ::== <name> ||  <dul-json>  || "("<expression>")"
<name> ::== ['_' || 'a-z' || 'A-Z'] *
<dul-json> ::== "{" ( {<name> || <string>} ":" <dul-json> )[ , {<name> || <string>} ":" <dul-json> ] "}"
<dul-json> ::== "[" (<dul-json>)[ , <dul-json> ] "]"
<dul-json> ::== <string> || <number>
