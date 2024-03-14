# Markdown Grammar

Syntax is taken primarily from [the official markdown guide](https://www.markdownguide.org/basic-syntax/)

## Headings

To create a heading, add number signs (#) in front of a word or phrase. The number of number signs you use should correspond to the heading level. For example, to create a heading level three (<h3>), use three number signs (e.g., ### My Header).

It should only be defined at the start of a new line and there needs to be a space between \# and the text. There should also be a blank line before and after the header

The compiler does not support the alternative syntax of \= and \-

## Paragraphs

Paragraphs are created when lines are separated by blank lines

If they are part of the same paragraph, they will be in adjacent lines

## Line Breaks

Line break is supported with the `<br>` token

## Emphasis

### Bold

To bold a word, surround it with \* like as follows:

```
This *word* is bold
```

only "word" will be bold

We do not allow for the conventional usage of \*\* since it is potentially ambiguous with Italics

### Italics

To italicise a word, surround it with \_

```
This _word_ is italic
```

### Bold and Italics

Hence, by extension, bold an italics is supported by

```
This _*word*_ is bold and italic
```

The order of \* and \_ is irrelevant

## Blockquotes

Blockquotes are supported same as in conventional markdown

```
> This will be in a blockquote
```

\> is only accepted at the start of a new line for it to be a blockquote.

## Lists

For all lists, the symbol that we are looking for will be at the start of the new line

### Ordered

```
1. First Item
2. Second Item
3. Third Item
```

### Unordered

```
- First Item
- Second Item
- Third Item
```

### Nested Lists

Nested Lists are allowed through tab characters at the start of a new line

## Code

Code blocks are surrounded with \` (backticks)

If it is inline, then a single backtick will suffice

3 backticks define the code block environment to be on a newline

## Horizontal Rules

Three dashes

```
---
```

## Links

Defined as

```
[text](link)
```

## Images

Defined as

```
![alt text](img-link)
```

Images need to be defined at the start of a new line.

This is to allow for disambiguation between Images and links

## Escaping Characters

Escaping characters will be done using \\

## HTML

The compiler will not support HTML tags

# Additional Syntax

## Math Environments

### Inline

Defined with \$

### Blocks

Defined with double \$\$ at the start and the end of the block

Only allowed at the start of a new line

## Using YAML for document level information

For things like author, title and date, these will be defined as YAML

```
---
title: Homework
author: John Smith
date: 2024-10-01
---
```
