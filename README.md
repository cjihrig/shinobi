# shinobi

A simple new programming language

## Building

The compiler currently builds with CMake and has a single program (found in `driver.cc`) baked into the binary. Running the compiler currently logs the test program's LLVM assembly code to the console and creates an object file for it, which is written to `output.o`. The object file can also be linked and run as part of a C/C++ executable (currently not documented).

```
cmake .
cmake --build . --target shinobi
./shinobi
```

## Running tests

```
cmake --build . --target check
```

## Language design

### Data types

- No `null`/`nil`/`undefined` primitive. Only used with `Value` (see below).
- Booleans. `bool`.
- Signed integers. `i8`, `i16`, `i32`, `i64`, `i128`.
  - Two's complement.
- Unsigned integers. `u8`, `u16`, `u32`, `u64`, `u128`.
  - Byte data types should use `u8`.
- Floating point numbers. `f32`, `f64`.
  - IEEE 754.
- Character. `char`.
  - [Unicode scalar value](https://www.unicode.org/glossary/#unicode_scalar_value).
- Strings. `string`.
  - UTF-8 encoding.
- Symbol. `symbol`.
- Array. `type[]`, `type[size]`, `type[size; fill]`.
- Tuple. `(type0, type1, ..., typeN)`.
- Struct. `struct { field0: type0, ..., fieldN: typeN }`.
- Function. `function name(arg0: type0, ..., argN: typeN): returnType { body }`.
- Object.
- Error. `Error`.
  - Errors have a message, code, and cause.
  - Only `Error`s can be used as errors. In other words, strings or numbers
    cannot be errors.
- Value. `Value<type, null>`.
  - If present, the `Value` will be a value of type `type`.
  - If not present, the `Value` will be `null`.
- Result. `Result<type, error>`.
  - On success, the `Result` will be a value of type `type`.
  - On failure, the `Result` will be a value of type `error`.
