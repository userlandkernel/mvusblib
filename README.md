# mvusblib
Minerva USB Library for darwin macOS &amp; iOS

# Building
Please read `BUILDING.md`

# Contributing
- Functions should be easy to use
- Stack is preferred over heap memory
- Always initialize structure pointers and nullify free'd pointers
- Check where possible for invalid provided arguments like null for example
- Log messages go to standard error instead of standard output
- Use kernreturn values accordingly for functions that perform low-level logic and may fail
- Never exit the program for an error, instead use function return with error code.
