#  easified grammar

<program> ::== <compound>
<compound> ::== [<statement>]
<statement> ::== "write" <expr>
<statement> ::== <name> "=" <expr>
<expr> ::== <exp-mult> || <exp-mult> [+, -] "exp-mult"
<exp-mult> ::== <top> || <top> [*, /] <top> 
<top> ::== <number> || <name>
