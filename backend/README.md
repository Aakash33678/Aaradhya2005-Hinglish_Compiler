# Hinglish Programming Language 🇮🇳

A compiler for **Hinglish** — a simple programming language that uses Hindi-inspired keywords written in English (Hinglish). Source files use the `.li` extension.

---

## Table of Contents

- [Getting Started](#getting-started)
- [Installation & Running](#installation--running)
- [Language Syntax](#language-syntax)
- [Keywords Reference](#keywords-reference)
- [Data Types](#data-types)
- [Operators](#operators)
- [Examples](#examples)
- [Compiler Phases](#compiler-phases)
- [Error Handling](#error-handling)
- [Limitations](#limitations)

---

## Getting Started

### Requirements

- A C++ compiler with C++17 support (`g++` recommended)
- A terminal or command prompt

### Project Structure

```
your-project/
├── main.cpp        ← Compiler source code
├── README.md       ← This file
└── test.li         ← Your Hinglish program
```

---

## Installation & Running

### Step 1 — Compile the Compiler

Open your terminal in the project folder and run:

```bash
g++ -std=c++17 -o hinglish main.cpp
```

This creates an executable called `hinglish`.

### Step 2 — Write Your Program

Create a file with the `.li` extension (e.g., `test.li`) and write your Hinglish code inside it.

### Step 3 — Run Your Program

**Linux / Mac:**
```bash
./hinglish test.li
```

**Windows:**
```bash
hinglish.exe test.li
```

### One-Line Build + Run

```bash
g++ -std=c++17 -o hinglish main.cpp && ./hinglish test.li
```

---

## Language Syntax

### Basic Rules

- Every statement ends with a **semicolon** `;`
- Code blocks are wrapped in **curly braces** `{ }`
- Variable names are **case-sensitive**
- Only **integer** and **string** values are supported
- Strings must be wrapped in **double quotes** `"like this"`

---

## Keywords Reference

| Hinglish Keyword | Meaning         | Equivalent in C |
|------------------|-----------------|-----------------|
| `val`            | Declare variable | `int`           |
| `prt`            | Print to screen  | `printf()`      |
| `agar`           | If condition     | `if`            |
| `nhi-to`         | Else             | `else`          |
| `bhejo`          | Return a value   | `return`        |

> ⚠️ These keywords are **reserved** — you cannot use them as variable names.

---

## Data Types

Hinglish currently supports two types:

| Type    | Description              | Example              |
|---------|--------------------------|----------------------|
| Integer | Whole numbers only       | `val x = 42;`        |
| String  | Text in double quotes    | `prt("Hello!");`     |

> ⚠️ Strings can only be **printed**, not stored in variables.

---

## Operators

### Arithmetic Operators

| Operator | Description    | Example       |
|----------|----------------|---------------|
| `+`      | Addition        | `val z = x + y;` |
| `-`      | Subtraction     | `val z = x - y;` |
| `*`      | Multiplication  | `val z = x * y;` |
| `/`      | Division        | `val z = x / y;` |

### Comparison Operators (used in `agar` conditions)

| Operator | Description           |
|----------|-----------------------|
| `==`     | Equal to              |
| `!=`     | Not equal to          |
| `<`      | Less than             |
| `>`      | Greater than          |
| `<=`     | Less than or equal    |
| `>=`     | Greater than or equal |

### Assignment Operator

| Operator | Description     | Example        |
|----------|-----------------|----------------|
| `=`      | Assign a value  | `val x = 10;`  |

---

## Examples

### Hello World

```
prt("Namaste Duniya!");
```

**Output:**
```
Namaste Duniya!
```

---

### Variables and Arithmetic

```
val x = 10;
val y = 5;
val z = x + y;
prt(z);
```

**Output:**
```
15
```

---

### Subtraction, Multiplication, Division

```
val a = 20;
val b = 4;
val add = a + b;
val sub = a - b;
val mul = a * b;
val div = a / b;
prt(add);
prt(sub);
prt(mul);
prt(div);
```

**Output:**
```
24
16
80
5
```

---

### If-Else (`agar` / `nhi-to`)

```
val a = 10;
val b = 20;
agar (a < b) {
    prt("b bada hai");
} nhi-to {
    prt("a bada hai");
}
```

**Output:**
```
b bada hai
```

---

### Comparing Two Numbers

```
val score = 85;
val passing = 40;
agar (score >= passing) {
    prt("Pass ho gaye!");
} nhi-to {
    prt("Fail ho gaye.");
}
```

**Output:**
```
Pass ho gaye!
```

---

### Using `bhejo` (Return)

```
val x = 100;
val y = 200;
val total = x + y;
bhejo total;
```

**Output:**
```
Return: 300
```

---

### Complex Expression

```
val a = 3;
val b = 4;
val c = a * b;
val d = c + 2;
prt(d);
```

**Output:**
```
14
```

---

### Nested Conditions

```
val x = 10;
val y = 10;
agar (x == y) {
    prt("Dono barabar hain");
} nhi-to {
    prt("Alag hain");
}
```

**Output:**
```
Dono barabar hain
```

---

## Compiler Phases

When you run a `.li` file, the compiler shows output for **all 5 phases**:

### Phase 1 — Lexical Analysis
Breaks the source code into tokens (keywords, identifiers, numbers, operators, etc.)
```
Line 1, Column 0: Keyword = val
Line 1, Column 4: ID = x
Line 1, Column 6: ASSIGN = =
Line 1, Column 8: NUMBER = 10
Line 1, Column 10: SEMI = ;
```

### Phase 2 — Syntax Analysis (Parse Tree)
Builds an **Abstract Syntax Tree (AST)** and prints it in JSON format:
```json
{
  "type": "Program",
  "children": [
    {
      "type": "Declaration",
      "value": "x",
      "children": [
        { "type": "NumberLiteral", "value": "10" }
      ]
    }
  ]
}
```

### Phase 3 — Semantic Analysis
Checks for:
- Undeclared variables
- Duplicate variable declarations

Prints the **Symbol Table**:
```
x: variable
y: variable
z: variable
```

### Phase 4 — Intermediate Code (Three-Address Code)
```
0: x = 10
1: y = 5
2: T1 = x + y
3: z = T1
4: print z
```

### Phase 5 — Assembly Code
```
0: mov eax, 10
1: mov ebx, 5
2: add eax, ebx
3: mov z, eax
```

### Final — Program Output
The actual result of running your `.li` program is printed at the end under:
```
=== Output of Input Code ===
```

---

## Error Handling

The compiler reports errors with **line and column numbers**. It stops at the first phase where errors are found.

### Common Errors

| Error Message | Cause | Fix |
|---------------|-------|-----|
| `Illegal character '?' at line 2` | Unknown symbol in code | Remove invalid characters |
| `Expected ';' at end of declaration` | Missing semicolon | Add `;` at end of statement |
| `Undeclared variable 'x'` | Using a variable before declaring it | Add `val x = ...;` before using it |
| `Variable 'x' already declared` | Declaring the same variable twice | Remove or rename the duplicate |
| `Expected '(' after 'prt'` | Missing parentheses in print | Use `prt(value);` format |
| `Cannot use reserved keyword as identifier` | Using a keyword as variable name | Rename the variable |

---

## Limitations

The current version of Hinglish does **not** support:

- Loops (`while`, `for`) — coming soon
- Functions / procedures
- Floating point numbers (only integers)
- Storing strings in variables (strings can only be printed directly)
- Arrays or lists
- Multiple files / imports
- Nested `agar` inside another `agar` body with complex conditions

---

## Quick Reference Card

```
┌─────────────────────────────────────────────────────────┐
│               HINGLISH QUICK REFERENCE                  │
├──────────────────┬──────────────────────────────────────┤
│ Declare variable │ val name = value;                    │
│ Print number     │ prt(variable);                       │
│ Print string     │ prt("text");                         │
│ If condition     │ agar (condition) { ... }             │
│ If-Else          │ agar (...) { ... } nhi-to { ... }    │
│ Return value     │ bhejo value;                         │
└──────────────────┴──────────────────────────────────────┘
```

---

## License

This project is open for personal and educational use.

---

*Hinglish Compiler — Apni bhasha mein code karo!* 🚀
