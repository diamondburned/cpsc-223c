# Word Count: the "wc" command

Test it out. You are reimplementing that command!

**PLUS ONE NEW OPTION**

You need to look at it's --help to understand! Make it identical except for options that are not listed below.

Note that command operates on **multiple files**.

**Especially** how specifying an option "turns off" the other default ones.

# Main options to implement
* -c / --bytes
* -l / --lines
* -L / --max-line-length
* -w / --words
* **(do the minor ones too)**

## New option to implement
* -f - output frequency counts of printable characters at end (format "<number> <char>" on a line)

# Options NOT to implement
* -m / --chars
* --files0

# Notes about testing
* Write 1-3 unit tests, no less, and no more unless you want to. That said, try out using them to help you.