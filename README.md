# os-workbench
Operating System Labs @ NJU
## Author
### 鄢振宇
#### Zhenyu, Yan
Nanjing University, 2017

### Introduction

This is my homework, and I've done many additional things to make it more beautiful and elegant.
This project will be long-time supported.
Fell free to create a new Pull request or issue if you find a bug or something to optimize this OS.
I'll reply ASAP.

### Contact me
QQ: 1015198808

### Features beyond assignment
- kmt_teardown, kmt_wait
- fork and execve in shell
- shell grammars like
  + <> (redirection)
  + | (pipe)
  + ""'' (string)
- color support(tty_set_color, tty_set_color, ls)

### To be handled
- memory recovery of pipe
  + I don't have a good idea of this
  + (Check kernel/src/pipe.c for more info)
- run on one task continuously
  + My design can't run on one task continuously, it needs to switch to idle task first
  + (Check kernel/src/kmt.c:kmt_context_switch)
