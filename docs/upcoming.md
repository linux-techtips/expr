# EXPR Documentation

## Introduction
expr is a language that hasn't been made yet. The following documentation is a proposal
for how the language will be implemented and, while anything is subject to change, these
docs provide a great jumping off point for the future of expr.

## Priciples
- **Low Level**
- **Simplicity* over *Complexity**
- **Explicit** over **Implicit**
- **Data Oriented** over **Object Oriented**
- No `unsafe` or Borrow Checking
- No **Hidden Control Flow**
- **Manual** or **RAII** Resource Management
- Clear and concise line of communication with the compiler
- Clear and concise method of creating macros and interfacing with the AST
- If possible, minimize the amount of ways to perform the same actions.
- Before adding new language features, see if current language features can be extended

## Table of Contents

<table>
<tr><td width=33% valign=top>

* [Expr Pattern](#expr-pattern)
* [Comments](#comments)
* [Keywords](#keywords)
* [Operators](#arithmatic-operators)
  * [Arithmatic](#arithmatic-operators)
  * [Conditional](#conditional-operators)
  * [Complex](#complex-operators)
* [Primitives](#primitives)
* [Literals](#literals)

</td><td width=33% valign=top>

* [Variables](#variables)
* [Arrays](#arrays)
* [Slices](#slices)
* [Addresses](#pointers)
* [Casting](#casting)
* [Types](#types)
  * [Aliasing](#aliasing)
  * [ZST's](#zsts)
  * [Structs](#structs)
  * [Enums](#enums)
* [Match](#match)

</td><td valign=top>

* [If Conditions](#if-conditions)
* [If Expressions](#if-expressions)
* [Loop and Range](#loop-and-range)
* [Until](#until)
* [Functions](#functions)
* [Impl Methods](#impl-methods)
* [Closures](#closures)
* [Higher Order Functions](#higher-order-functions)
* [Traits](#traits)

</td></tr>
<tr><td width=33% valign=top>

* [Generics](#generics)
  * [Generic type](#generic-type)
  * [Generic fn](#generic-fn)
  * [Generic impl](#generic-impl)
  * [Generic trait](#generic-trait)
  * [Generic const](#generic-const)

</td><td width=33% valign=top>

* [Error Handling](#error-handling)
  * [Panic](#panic)
  * [Option](#option)
  * [Result](#result)
* [Memory Management](#memory-management)
* [Drop Trait](#drop-trait)
* [Allocators](#allocators)

</td><td valign=top>

* [Standard Library](#standard-library)
* [Builtin Functions](#builtin-functions)
* [Attributes](#attributes)
* [Conditional Compilation](#conditional-compilation)
* [Comptime](#comptime)
* [The AST](#the-ast)
* [Reflection](#reflection)
* [FFI](#ffi)

</td></tr>
</table>

## Expr Pattern

The expr pattern is unique to expr and is the core of the language itself.
It is the pattern used to create variables, types, functions, etc. and remains consistent
throughout the language.
The expr pattern is as follows:

`ident` : `type` `constness` `expr`

```rust

x : ssize = 10

entry : fn : {
  println("Hello World")
}

Person : type : {
  name: String
  age: u8
}

```

While using the expr pattern will be touched on in different sections,
this pattern will be used throughout the language and is integral to it's design.

<!--
For now, code snippet comments will be done using rust style comments
until markdown support is implemented.
-->

## Comments
```asm

; This is a single line comment

;; This is a multi-line comment
  ; It can even have nested comments
;;

```

## Keywords
```rust

use pub

type trait impl

fn yeet

match or is

loop until in break continue

if else elif

true false

as

mut

```

## Arithmatic Operators
```rust

+   sum           integrals, real
-   diff          integrals, real
*   product       integrals, real
/   quotient      integrals, real
%   remainder     integrals

~   bitwise NOT   integrals
&   bitwise AND   integrals
|   bitwise OR    integrals
^   bitwise XOR   integrals

!   logical NOT   boolean
&&  logical AND   boolean
||  logical OR    boolean
!=  logical XOR   boolean

<<  left shift    integral << unsigned integral
>>  right shift   integral >> unsigned integral

```

## Conditional Operators
```rust

<   less-than                   eq <-> eq
>   greater-than                eq <-> eq
==  equal-to                    eq <-> eq

<=  less-than /or equal-to      eq <-> eq
>=  greater-than /or equal-to   eq <-> eq

```

## Complex Operators
```rust

=   reassign  any

*   const-address-of  any
&   mut-address-of    any
@   at-address        any

[]  array-at  any

.   type-access any.any

```

## Primitives
```rust

bool // boolean

s8 s16 s32 s64 ssize s128 // signed integers
u8 u16 u32 u64 usize u128 // unsigned integers
   d16 d32 d64 dsize d128 // decimal

rune // unicode character

() // tuple 

```

## Literals
```rust

true    -> boolean
false   -> boolean

69      -> ssize
420.    -> dsize
34u     -> usize

69s32   -> s32
420.d32 -> d32
34u32   -> u32

0b1001  -> ssize
0x1001  -> ssize
0o101   -> ssize

69.420e-7  -> dsize
10_000_000 -> ssize

`a`     -> rune
'lol'   -> [rune]

(`a`, 1u8)                -> (rune, u8)
[1, 2, 3, 4, 5]           -> [ssize:5]
{'hello': 1, 'world': 2}  -> {[rune]:ssize}

```

## Variables

Variables are any statement that's type is not a `fn` or `type`.
If a variable is declared at the toplevel, it is `static` and it's value must be known at comptime.
If a toplevel variable is declared, it **cannot** be made public. Only the current module can have access to it.
If a variable is mutable, it can be reaasigned using the `=` [operator](#complex-operators).
Variables also have the ability to have their type inferred from the result of the expression being
assigned to it.
Variables also have to be initialized.

Examples:

```rust

// Creating a static constant ssize with the hex value of 0xBADCAT
MAGIC : ssize : 0xBADCAT
// Creating a static rune array with the length of 6 set to 'static'
STATIC : [rune:7] = 'staticy'

entry : fn : {

  // Reassign STATIC to 'tacocat'
  STATIC = 'tacocat'
  // Creating a constant ssize set to 34
  x :: 34
  // Creating a mutable ssize set to 35
  y := 35

  // Creating a mutable ssize set to the result of the expression 'x + y * MAGIC'
  z := x + y * MAGIC

  // Reassigning to 386
  y = 386

  // Reassigning to the result of the expression 'x + y * MAGIC'
  z = x + y * MAGIC
}

```

## Arrays

Arrays are a contiguous chunk of memory that contain the same yupe `T`.
Arrays must also have their length and type known at compile time.

Format: [`T`:`len`]

```rust

size_arr : [ssize:10] : [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
str_arr : [[rune]:2] : ['Hello', 'World']

```

## Slices

***TODO***

Slices are similar to arrays but their sizes are not set. Effectively, a slice
is a reference to the begining and a reference to the end of a contiguous chunk of
memory. A Slice also stores the difference between the begining and end pointer
so its size can be calculated.

Format: [`T`]

```rust

// Not sure if I should do slice literals or implicit casts from arrays to slices

```

## References

**TODO**

In expr, references are a memory address that points to another location in memory.
Unlike other langauges, references are not aliases, and have special mutability rules.
There are two types of references, a constant reference: `&`, and a mutable reference `*`.
A constant reference does not mean that the address cannot be reassigned, it simply states
that the underlying data that is is pointing to cannot be changed. The at address operator `@`
can be used to dereference the reference and retrieve the underlying data. You can only change
this data if the you have a mutable reference `*`

Examples

```rust

x :: 10
y := 20

ptr_mut := *x // Creates a mut ref
ref_mut := &x // Creates a const ref

ptr_const :: *x // Creates a constant mut ref
ref_const :: &x // Creates a constant const ref

@ptr = 30 // Changes the value of x to 20
@ref = 40 // FAILS: You cannot change the value of a const ref

ptr = *y // ptr is declared as mutable and can be reassigned
ref = &y // ref is declared as mutable and can be reassigned

ptr_const = *y // FAILS: You cannot reassign the address of a constant
ref_const = &y // FAILS: You cannot reassing the address of a constant

```

## Casting

**TODO**

## Types

**TODO**

## Aliasing

All types including builtin types can be aliased or renamed. In the following example
a new type `char` is created that shadows the type of `u8`.

```rust

char : type : u8

x : char : 10
y : u8 : 10

assert(typeof(x) == typeof(y))

```

## ZST's

Zero Sized Types are types without any fields or entries that are used in conjunction
with other types to create enumerated types.

```rust

ZST : type : {}

```

## Structs

Structures are types with fields. Each field in the structure can be accessed using
the `.` operator. The `.` operator can also be used on references to types.

```rust

Person : type : {
  name: [rune]
  age: u8
}

String : type : {
  data: *rune
  len: usize
  cap: usize
}

bob :: Person{ name: "Bob", age: 69 }
println("There is a person {} who is {} years old", bob.name, bob.age)

```

## Enums

By far one of the most powerful types in expr that allows for the creation of
many complex types. Enums can be created using other types in conjunction with
the `or` keyword.

```rust

Character : type : u8 or u16 or rune

```

## Match

The `match` keyword can be used to create control flow from the variants and fields
that a type has.

```rust

Person : type : {
  name: String
  age: u8
}

CoolType : type : usize or [rune] or Person

cool : CoolType : "Hello World"

match cool {
  usize is t => println("Got a usize: {}", t),
  [rune] is t => println("Got a rune slice: {}", t),
  Person is t{ name, age } => {
    println("Got a person: {} who is {} years old", name, age)
  },
}

```

## If Conditions

**TODO**

## If Expressions

**TODO**

## Loop and Range

**TODO**

## Until

**TODO**

## Functions

**TODO**

## Impl Methods

**TODO**

## Closures

**TODO**

## Higher Order Functions

**TODO**

## Traits

**TODO**

## Generics

**TODO**

## Generic `type`

**TODO**

## Generic `fn`

**TODO**

## Generic `trait`

**TODO**

## Generic `const`

**TODO**

## Error Handling

**TODO**

## Panic

**TODO**

## Option

**TODO**

## Result

**TODO**

## Memory Management

**TODO**

## Drop Trait

**TODO**

## Allocators

**TODO**

## Standard Library

**TODO**

## Builtin Functions

**TODO**

## Attributes
**TODO**

## Conditional Compilation

**TODO**

## The AST

**TODO**

## Reflection

**TODO**

## FFI

**TODO**

