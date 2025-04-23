# lightjson - JSON Builder and Parser

A lightweight JSON builder and parser library implemented in C.

## Project Structure

```
.
├── include/            # Public header files
│   ├── json_builder.h  # JSON builder header file
│   └── json_parser.h   # JSON parser header file
├── src/                # Source code
│   ├── json_builder.c  # JSON builder implementation
│   └── json_parser.c   # JSON parser implementation
├── test/               # Test code
│   ├── test_json.c     # Test cases
│   ├── unity.c         # Unity test framework
│   └── unity.h         # Unity test framework header
├── examples/           # Example code
│   └── main.c          # Usage example
├── docs/               # Documentation
├── build/              # Build directory
└── Makefile            # Build script
```

## Features

- JSON Builder: Create and manipulate JSON objects and arrays
- JSON Parser: Parse JSON strings into in-memory data structures
- UTF-8 encoding support
- Support for nested objects and arrays
- Support for special characters and escaping

## Build and Usage

### Building the Library

```bash
make
```

### Running Tests

```bash
make test
```

### Example

Check `examples/main.c` to learn how to use the library.

## API Documentation

### JSON Builder

- `json_builder_create()` - Create a JSON builder
- `json_builder_free()` - Free a JSON builder
- `json_builder_start_object()` - Start a JSON object
- `json_builder_end_object()` - End a JSON object
- `json_builder_start_array()` - Start a JSON array
- `json_builder_end_array()` - End a JSON array
- `json_builder_add_string()` - Add a string key-value pair
- `json_builder_add_number()` - Add a number key-value pair
- `json_builder_add_bool()` - Add a boolean key-value pair
- `json_builder_add_null()` - Add a null key-value pair
- `json_builder_append()` - Add a raw string
- `json_builder_get_string()` - Get the built JSON string

### JSON Parser

- `json_parse()` - Parse a JSON string
- `json_value_free()` - Free a JSON value
- `json_value_get_string()` - Get a string value
- `json_value_get_number()` - Get a number value
- `json_value_get_bool()` - Get a boolean value
- `json_value_get_object()` - Get an object value
- `json_value_get_array()` - Get an array value
- `json_get_error()` - Get error information

## License

[MIT License](LICENSE)
