# MD-to-Tex: Markdown to LaTeX Converter

50.051 PLC - A C/C++ package to compile MD into LaTEX

## Overview
This project is a Markdown to LaTeX converter written in C. It parses a Markdown (.md) file, constructs an abstract syntax tree (AST) representing the structured content of the document, and then renders that AST to a LaTeX (.tex) file.

## Installation

To install and compile the MD-to-Tex project, use the following command in the terminal:

```bash
make
```

This will generate the necessary executable to convert Markdown to LaTeX.

## Running Tests

To run predefined tests and view outputs, execute the included test script:

```bash
./testing.sh
```

After running the script, check the `output` directory to review the results of all test cases.

## Usage

To convert an individual Markdown file to a LaTeX document, first compile the project using `make`, then run the following command:

```bash
./md_to_tex -i <input_file_name> -o <output_file_name> 
```

Replace `<input_file_name>` with the path to your Markdown file. The output will be a `.tex` file with the same name as the input file, located in the current directory.

## Structure

- `src`: Source directory containing all the code.
  - `parser`: Contains the parser that converts raw Markdown into an AST.
    - `features`: Specific implementations for parsing Markdown features like code blocks, headers, and paragraphs.
    - `parser.c`: Main parser file that orchestrates the parsing process.
  - `renderer`: Responsible for rendering the AST to LaTeX format.
    - `renderer.c`: Main renderer file that traverses the AST and generates LaTeX.
    - `iterator.c`: Iterator tracks traversal to correctly render LaTeX syntax for Markdown elements, ensuring proper structure.
    - `features`
         - `mermaid_graph.c`: Responsible for rendering graph diagrams in LaTEX.
         - `memraid_class_diagram.c`: Responsible for rendering xlass diagrams in LaTEX.
         - `mermaid_pie.c`: Responsible for rendering pi charts in LaTEX.
         - `mermaid_seq_diagram.c `: Responsible for rendering seqeuence diagrams in LaTEX.
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

### `node.c`, `codeblock.c`, `headers.c`, `paragraph.c`, `blockquote.c`, `image.c`, `list.c`, `mathblock.c`
- Define structures and functions for node management specific to various Markdown elements.
- Detect Markdown syntax for code blocks, headers, paragraphs, blockquote, image, list, mathblock, mermaid diagrams and configure nodes with the relevant data.

### `mermaid_class_diagram.c`, `mermaid_sequence_diagram.c`, `mermaid_graph.c`, `mermaid_pie.c`
- These modules handle the parsing and rendering of various Mermaid diagram types (class diagrams, sequence diagrams, graphs, and pie charts) from Markdown to LaTeX format.

## Features

- Parsing of Markdown syntax into an AST.
- Rendering of the AST to LaTeX format.
- Supports diagrams (graphs, sequence diagrams, pie charts, class diagrams), headers, paragraphs, code blocks, and other Markdown elements.

## Compatibility

Tested on macOS, Linux, and Windows environments by our team, ensuring cross-platform reliability.

Note: For class diagrams, please include `tikz-uml.sty` in tha main folder to render them properly.

## Authors

- Yong Zhe Rui Gabriel
- Visshal Natarjan
- Kaveri Priya Putti
- Aurelius Bryan Yuwana
