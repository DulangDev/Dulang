## This is a compiler for dulang programming language

Although the source is been compiled into machine code, not actually an executable file is generated.
There are generated classdefs that contain unlinked machine code

## dul bootstrap

This is virtual machine is used for. Some important functions such as malloc are easier to implement in cpp, not asm. Therefore we link machine code with them dynamically, mostly using mmap and looking through the needed symbols.

Also some complex processes, such as gc are a part of virtual machine.


To be continued
