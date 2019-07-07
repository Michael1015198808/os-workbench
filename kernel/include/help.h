#ifndef IN_BUILDIN_C
_Static_assert(0,"Should not include \"help.h\" in files except buildin.c");
#endif
#define help(name) \
    const char* const help_##name
help(echo)="Write arguments to the standard output.";
help(cat)="Concatenate FILE(s) to standard output.";
help(help)="Display information about builtin commands or commands with help information in help.h.";
help(cd)="Change the shell working directory.";
help(wc)="Print newline, word, and byte counts for each FILE, and a total line if more than one FILE is specified.";
help(ls)="List information about the FILEs (the current directory by default).";
