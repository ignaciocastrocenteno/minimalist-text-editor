# Minimalist Text Editor
A tiny coffee project where I decided to create a simple text editor, written in the C programming language.

This program performs **in-place line editing** on a text file loaded into a fixed-size memory buffer. It reads the file, prints its contents, asks the user which line to replace (0-based index), accepts the new line (including spaces), and writes the modified buffer back to the same file—**all without dynamic allocations** and using only the C standard library.

## Features
- **In-place line editing** using pointer arithmetic.
- **Minimal memory footprint**: a single primary buffer (`BUF_SIZE = 1024`) plus a small temporary buffer for the user’s input.
- **Safe I/O**:
  - Uses `fread` with explicit `'\0'` termination.
  - Uses `fgets` for reading user input (handles spaces safely).
  - Bounds checking with truncation guard if the edited content would exceed the buffer capacity.
- **Pointer-oriented design**:
  - Line navigation with `strchr` and pointer offsets.
  - Tail-preserving edits with `memmove` to handle overlaps safely.
- **Robust error handling**:
  - Validates arguments, file open, read/write results, and line range.
  - Graceful failure messages for common issues (missing file, invalid line, oversized result).
- **Zero external dependencies**: C standard library only (`stdio.h`, `string.h`, optionally `stdlib.h`).

## How it works
1. **Load**: Opens the target file in read mode and loads up to `BUF_SIZE-1` bytes into `buffer`, ensuring NUL termination.
2. **Display**: Prints the full contents to stdout and prompts for the **0-based** line number to edit.
3. **Navigate**: Walks line-by-line using `strchr` to find `'\n'` boundaries until reaching the chosen line:
   - `line_start` points to the beginning of the target line.
   - `line_end` points to the `'\n'` that ends the line (or to the string end if it is the last line without trailing newline).
4. **Replace (in-place)**:
   - Reads the **new line** into a small temporary buffer (`fgets`), strips its trailing newline.
   - Calculates size deltas and **bounds checks** the final size. If necessary, **truncates** the new line to fit the fixed buffer.
   - Uses `memmove` to shift the **tail** (everything from `line_end` to the end of the buffer) forward/backward depending on the new line’s length.
   - Copies the new line into place with `memcpy`.
5. **Save**: Reopens the file in write mode and writes the updated buffer.

**Time complexity** (worst case):  
- Navigating to the line: `O(n)` over the number of characters up to the target line.  
- Shifting tail with `memmove`: `O(t)` where `t` is the size of the tail.  
Overall linear relative to file size.

**Memory model**:
- Primary buffer: `char buffer[BUF_SIZE]`.
- Temporary input buffer for the replacement line: `char new_line[BUF_SIZE]`.
- No heap allocations, no large auxiliary copies—just careful pointer arithmetic and a single `memmove`.

## File size limits and buffer strategy
- The editor is designed for **small files** and **predictable memory usage** (e.g., configuration files, notes).
- `BUF_SIZE` defaults to **1024 bytes**. If the result of the edit would exceed this size, the program will **truncate the new line** to fit and inform the user.
- You can adjust `BUF_SIZE` to a different constant if needed, keeping in mind stack limits and portability.

## Good practices implemented
- **Fail fast**: early validation of `argc`, `fopen`, `fread`, `fwrite`.
- **Explicit termination**: guarantee `buffer[read_bytes] = '\0'` after reading.
- **Safe user input**: `fgets` instead of `scanf("%s", ...)`, preserving spaces and avoiding buffer overflow.
- **Pointer safety**:
  - Check for `NULL` when using `strchr`.
  - Use `memmove` (not `memcpy`) when regions can overlap.
- **Clear separation of concerns**:
  - `main` handles I/O orchestration (read, prompt, write).
  - `edit_line` encapsulates line location and in-place modification.
- **Portability**: relies on C standard library, works with GCC/Clang/MSVC (minor adjustments to warnings/flags may apply).
- **User guidance**: informative prompts and error messages.

## Standard libraries used
- `<stdio.h>`: file I/O (`fopen`, `fread`, `fwrite`, `fclose`), console I/O (`printf`, `scanf`, `fgets`).
- `<string.h>`: string utilities (`strlen`, `strchr`, `strcpy`/`memcpy`, `memmove`).
- `<stdlib.h>`: (optional) general utilities; included in the reference code for clarity.

## Project structure
 ```bash
  src
    └── main.c # Program source (single-file implementation)
  README.md # Document that briefly describes the idea of the project and how to execute it
  LICENSE
 ```

## Build
You can compile with any C99-capable compiler.

**GCC / CLANG:**
```bash
gcc -std=c99 -Wall -Wextra -O2 -o mini-edit main.c
clang -std=c99 -Wall -Wextra -O2 -o mini-edit main.c
```
Tip: Add `-pedantic -Werror` in learning/testing contexts to catch more issues early.

## How to execute the code
1. Prepare a small text file, e.g. sample.txt:

    ```bash
    first line
    second line
    third line
    ```

2. Run the editor:
    ```bash
    ./mini-edit sample.txt
    ```

3. The program will:
Print the contents,
Ask for the 0-based line number to edit (e.g., `1` for “second line”),
Prompt for the new content (you can type spaces; press Enter to finish).

4. The file is rewritten with the updated line.

## Example
```bash
$ ./mini-edit sample.txt
Contents:
first line
second line
third line

Enter the 0-based line number to edit: 1
Enter the new content for that line: this is the NEW second line
```

Resulting file:
```bash
first line
this is the NEW second line
third line
```

## Notes & limitations
- **Line numbering** is **0-based** (0 = first line).
If you prefer 1-based, subtract 1 from the user input before calling edit_line.

- **No backup:** the file is overwritten in place. Consider copying the file beforehand if needed.

- **Small files only:** designed intentionally for a fixed-size buffer. For larger files, consider:
    * Streaming edits line-by-line,
    * Or dynamic allocation (realloc) with a growth strategy.

- **Newline conventions:** assumes Unix-like '\n' line endings. Windows CRLF files will still work for many cases, but exact behavior depends on compile-time text/binary mode and content. Open files in binary mode if you need strict control.
