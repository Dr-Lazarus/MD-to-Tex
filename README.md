# MD-to-Tex: Markdown to LaTeX Converter

50.051 PLC - A C/C++ package to compile MD into LaTEX

## Overview
This project is a Markdown to LaTeX converter written in C. It parses a Markdown (.md) file, constructs an abstract syntax tree (AST) representing the structured content of the document, and then renders that AST to a LaTeX (.tex) file.

## Compilation

The project includes a `Makefile` for easy compilation. Use the following command to compile the project:

```
make
```

This will generate the `md_to_tex` executable within the project directory.

## Usage

To convert a Markdown file to LaTeX, run:

```
./md_to_tex <md_file>
```

The LaTeX output will be written to `output.tex` in the current directory.

## Structure

- `src`: Source directory containing all the code.
  - `parser`: Contains the parser that converts raw Markdown into an AST.
    - `features`: Specific implementations for parsing Markdown features like code blocks, headers, and paragraphs.
    - `parser.c`: Main parser file that orchestrates the parsing process.
  - `renderer`: Responsible for rendering the AST to LaTeX format.
    - `renderer.c`: Main renderer file that traverses the AST and generates LaTeX.
  - `util`: Utility files for common operations.
    - `node.c`: Handles node operations within the AST.
    - `tree.c`: Functions for managing the AST including initialization and cleanup.
  - `iterator`: Provides an iterator for traversing the AST.
    - `iterator.c`: Implementation of the AST iterator.
  - `main.c`: The entry point of the program, handling file input and output operations.

- `Makefile`: Defines the project's build commands.

- `md_to_tex`: Executable generated after compilation.

- `output.tex`: The resulting LaTeX file after conversion.
## Detailed Functionality

### `main.c`
- Checks for a file path provided as a command-line argument.
- Begins the parsing process with `parse_source`, using the provided file path.
- Utilizes the returned AST to print the document structure for debugging and to render the LaTeX document.
- Outputs the rendered document to `output.tex`, adhering to LaTeX document structure.

### `parser.c`
- `read_source_code` reads the entire Markdown file into a string.
- `parse_line` analyzes each line to classify its type and update the AST by creating or appending nodes.
- `parse_source` constructs the AST, reading the file line by line and calling `parse_line` for each line.

### `tree.c`
- `init_tree` initializes the root node of the AST.
- `print_tree_data` recursively prints the structure of the AST for debugging purposes.
- `append_to_root` and `link_children` manage node relationships within the AST.
- `free_tree` handles the proper deallocation of the AST memory.

### `renderer.c`
- `traverse_ast` processes the AST to render LaTeX output.
- Contains various `convert_*` functions that translate Markdown nodes to LaTeX syntax based on their type.

### `iterator.c`
- Offers an iterator pattern to facilitate AST traversal without recursion.

### `node.c`, `codeblock.c`, `headers.c`, `paragraph.c`
- Define structures and functions for node management specific to various Markdown elements.
- Detect Markdown syntax for code blocks, headers, and paragraphs, and configure nodes with the relevant data.

## Features

- Parsing of Markdown syntax into an AST.
- Rendering of the AST to LaTeX format.
- Supports diagrams, headers, paragraphs, code blocks, and other Markdown elements.

## Dependencies

- Standard C libraries only.
