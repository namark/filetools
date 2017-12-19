# Intro
Each cpp file is a standalone little program. The initial idea was to show that modern C++(just by itself and standard library) can be as "easy to use" as some higher level languages for certain simple tasks. That was a total failure, and I decided to cheat and move some of the code out into a few simple libraries, so while the programs here look more or less clear and concise and don't exceed 200 lines, they depend on some external code that does not... at all.

Since these are file tools they all depend on [libsimple_file](https://github.com/namark/libsimple_file). Additional dependencies for each program (if any) are listed below.

---

## shred.cpp
Splits a given file(s) into small pieces, generating a .info file(s) that can be used to reassemble them. You can specify the size of the pieces (in bytes) with the -s option, which affects all the files specified after it. The size can be specified several times in a single command. Use -- to prevent option parsing if it conflicts with your file name(s). <br />
When run without parameters enters interactive mode, expecting a piece size followed by a list of files to "shred", input on separate lines (not just separated with whitespace). To specify new size just input an empty line and the program will expect a size again. <br />
Does not modify or delete the input files. <br />
Any errors will likely terminate the program.

**Additional dependencies** <br />
[libsimple_support](https://github.com/namark/libsimple_support)

---

## glue.cpp
Assembles a file(s) split by [shred](#shredcpp) program according to specified .info file(s). <br />
When run without parameters enters interactive mode, expecting a list of .info files to "glue", input on separate lines (not just separated with whitespace). <br />
Does not modify or delete the input files or associated pieces. <br />
Any errors will likely terminate the program.

---

## swap.cpp
Look how easy it is to swap two files now! Please don't ever use this...

---

## diff.cpp
Couldn't find an option for existing diff tools to show difference byte by byte as opposed to line by line(I didn't look too hard) so thought I could write my own.It was fun. It takes two files and outputs the position and size of differences(additions/deletions) between them. Completely non standard output format but makes sense in the context of the underlying algorithm. <br />
Does not modify or delete the input files or associated pieces. <br />
Can't handle "large" files.<br />
Any errors will likely terminate the program.

**Additional dependencies** <br />
[libsimple_geom](https://github.com/namark/libsimple_geom)

---

## crop.cpp
Displays a portion of a file, specified by a position and size(position:size), or a range(start-end). Has a lot(relatively) of parameters... I should document those as well... someday...<br />
Does not modify or delete the input files or associated pieces. <br />
Can't handle "large" files.<br />
Any errors will likely terminate the program.

**Additional dependencies** <br />
[libsimple_support](https://github.com/namark/libsimple_support)

---

<br />
<br />

# Building and installation
It's all in the makefile... but you'll need to install the dependencies manually. Make sure to use the latest stable compiler available to you, since that's what I do and have no idea what specific language or standard library features I relied on. I also primarily use GNU/linux based systems so no guarantee that the makefile won't spectacularly break for you, if you don't.

Here is an example of installing all dependencies and building all programs.
``` shell
# get the project
git clone https://github.com/namark/filetools

# get the dependencies
git clone https://github.com/namark/libsimple_file
git clone https://github.com/namark/libsimple_support
git clone https://github.com/namark/libsimple_geom

# install dependencies locally for this porject only
cd libsimple_file
make install PREFIX=../filetools
cd ../libsimple_support
make install PREFIX=../filetools
cd ../libsimple_geom
make install PREFIX=../filetools
cd ..

# remove repositories of the dependencies
rm -rf libsimple_file libsimple_support libsimple_geom

# build the entire project
cd filetools
make
```
After this you should have all the binaries available in ./out directory, and can install them anywhere with `make install`(by default /usr/local/bin/).

Each .cpp file is compiled to(and installed as) a binary with the same name, prefixed with the letter f, without an extension. <br />
For example `swap.cpp` will turn into `fswap`.


