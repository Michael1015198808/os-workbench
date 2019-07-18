#ifndef IN_BUILDIN_C
_Static_assert(0,"Should not include \"help.h\" in files except buildin.c");
#endif

#define help(name) \
    extern int mysh_##name(void*[]); \
    const char* const help_##name

help(echo)="Write arguments to the standard output.";
help(cat)="Concatenate FILE(s) to standard output.";
help(help)="Display information about builtin commands or commands with help information in help.h.";
help(cd)="Change the shell working directory.";
help(wc)="Print newline, word, and byte counts for each FILE, and a total line if more than one FILE is specified.";
help(ls)="List information about the FILEs (the current directory by default).";
help(mkdir)="Create the DIRECTORY(ies), if they do not already exist.";
help(sleep)="Pause for NUMBER seconds.";
help(reboot)="Reboot the system.";
help(touch)="Update the access and modification times of each FILE to the current time.";
help(test)="A test command. Now is used to test pipe";
