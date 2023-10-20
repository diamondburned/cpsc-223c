# Spring 2023 CPSC223C-03/04 Final

## This is a problem that has two parts.

***It is designed to take about 3 hours. It is not much longer than that if you have followed along so far. Use existing patterns and libraries (string stuff) rather than inventing much.***

# Part 1

## You are to create a templating engine for C.
* This will be a library that takes a template and a list of variables.
* The template will either be read from a file or used directly from a string.
* * It is suggested that you use ".tmpl" for template file extensions
* It will make any substitutions for the variables that are in the template.
* You are given the engine interface, and you are to implement it.

1. Templates (probably) have variables in them.
2. Variables are specified in a template as ```${varname}```
3. Engine users load a template, defines the substitutions, and generates the output
4. If a variable is used in the template but not defined, replace it with the empty string

## Example
Template:
```
<html><head><title>${page_title}</title></head>
<body>
<p>${page_head}</p>
<div>${page_text}</div>
<h4>${footer}</h4>
</body
</html>
```
Call teng_define 4 times:
```
// (Teng_template* t initialized elsewhere)
teng_define(t, "footer", "Page 1 of 97" );
teng_define(t, "page_head", "Words of Wisdom" );
teng_define(t, "page_title", "Experiences from 223C" );
teng_define(t, "page_text", "It was the best of classes, it was the worst of classes. The professor was nice, but a bit insane in what he expected. For example, he asked people to actually write code and tests! For that reason, I am giving him a 0.0 on the SOQ. However, he did bring cookies." );
```
Then generate/output the file to stdout or a .html
```
if (no output file argument)
  printf("%s\n",teng_generate(t));
if (output file argument)
  teng_generate_file(t,file name);
```
*The generated HTML is left as an exercise to the reader.*

# Part 2

## You are given a JSON file containing a book summary (```book.json```). 
* You'll need to copy things from the JSON starter pack.
* ***Remember*** You'll need to copy ```Makefile``` and ```replit.nix``` from there too.
  
## You are to use the templating engine to produce an HTML page for this book. 
* This is your main program.
### Take the file names (JSON, template file, and output file) from command line
### Generate the output and write it to the designated output file.

### In order to demonstrate this, you will need to create a template file that has the fields and HTML.
### While you will not be graded on the appearance (i.e CSS) of the page, you will be graded on the functionality and contents -- does everything render correctly and "work" correctly.

# About the project
1. The only place that will be accepted and graded is the Teams->223C submission. Period.
2. As in the midterm, answer the questions in Canvas when you are done.
3. Do not modify your Replit after you submit on Canvas.
4. You have through ***Friday May 19, 1159PM*** to submit on Canvas.

* Open books, open internet, closed friends, closed Discord, closed ChatGPT.
  * "Open Internet" means OK to research things and copy snippets, not multiple lines of code.
* Do not hesitate to ask me questions. If you are just stuck on something, I may unstick you so you can proceed. No guarantees.
  * Ask via ***Canvas or SMS only***.

***Useful tip: if you implement any of the engine in main.c, you will FAIL. main.c is for your example usage only.***
***Useful tip 2: Make it easy to use and follow the requirements. What do I ALWAYS ask for in programs?***

# Scoring
150 points total
* 50 functionality _(does it do what I asked?)_
* 50 unit tests _(how many? Enough to test all the cases and major errors!)_
* 50 style and design _(and "is it clear to me how it works?")_

## EC
* 15 points - attempt _(do a basic job, not a perfect job)_ of preventing template variables from inject rogue HTML


