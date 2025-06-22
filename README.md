# PMT (Polymorphic Types)
A single header library that mimics gnuradios PMT to allow building PDU (protocol data unit) buffers.

## Using the library
### Adding the library to your project 
1. Drag the header file into your project.
2. Use this repository as a submodule and add link it to your project.
```cmake
target_link_libraries(${PROJECT_NAME} INTERFACE PMT_LIB)
```

### Usage example
```cpp
#include "pmt.hpp"

pmt::pmt_t make_pdu(std::complex<float>* samples, size_t n_samples, float timetag) {
   // 1. Creating the metadata pmt.
   pmt::pmt_t meta = pmt::make_dict();
   pmt::dict_add(meta, "timetag", timetag);
   
   // 2. Creating the vector pmt.
   pmt::pmt_t vec = pmt::init_vector(n_samples, samples);
   
   // 3. Combining into PDU
   pmt::pmt_t pdu = pmt::cons(meta, vec);
   return pdu;
}
```

## PMT Explaination
### Common PMT Type Codes

| **Type Name**           | **Type Tag (Hex)** | **Meaning / Notes**                               |
|-------------------------|--------------------|---------------------------------------------------|
| **PMT\_TRUE**           | `00`               | Boolean `true`                                    |
| **PMT\_FALSE**          | `01`               | Boolean `false`                                   |
| **PMT\_SYMBOL**         | `02`               | Symbol / String                                   |
| **PMT\_INT32**          | `03`               | Signed 32-bit integer                             |
| **PMT\_DOUBLE**         | `04`               | 64-bit IEEE double precision float                |
| **PMT\_COMPLEX**        | `05`               | 128-bit IEEE double precision complex float       |
| **PMT\_NULL**           | `06`               | NULL                                              |
| **PMT\_PAIR** (cons)    | `07`               | Pair (used for PDUs → `cons(meta, vector)`)       |
| **PMT\_VECTOR**         | `08`               | Generic vector                                    |
| **PMT\_DICT**           | `09`               | Dictionary (`pmt::make_dict()`)                   |
| **PMT\_UNIFORM_VECTOR** | `0a`               | Uniform vector type tag                           |
| **UVI\_U8**             | `0a 00`            | Uniform vector subtype for `uint8_t`              |
| **UVI\_S8**             | `0a 01`            | Uniform vector subtype for `int8_t`               |
| **UVI\_U16**            | `0a 02`            | Uniform vector subtype for `uint16_t`             |
| **UVI\_S16**            | `0a 03`            | Uniform vector subtype for `int16_t`              |
| **UVI\_U32**            | `0a 04`            | Uniform vector subtype for `uint32_t`             |
| **UVI\_S32**            | `0a 05`            | Uniform vector subtype for `int32_t`              |
| **UVI\_U64**            | `0a 06`            | Uniform vector subtype for `uint64_t`             |
| **UVI\_S64**            | `0a 07`            | Uniform vector subtype for `int64_t`              |
| **UVI\_F32**            | `0a 08`            | Uniform vector subtype for `float`                |
| **UVI\_F64**            | `0a 09`            | Uniform vector subtype for `double`               |
| **UVI\_C32**            | `0a 0a`            | Uniform vector subtype for `std::complex<float>`  |
| **PMT\_C64**            | `0a 0b`            | Uniform vector subtype for `std::complex<double>` |
| **PMT\_C64**            | `0a 0b`            | Uniform vector subtype for `std::complex<double>` |
| **PMT\_C64**            | `0a 0b`            | Uniform vector subtype for `std::complex<double>` |

### PDU Structure
> NOTE: All values are in big endian.
```cpp
07 // PST_PAIR (PDU)
   // Foreach key-value pair:
   09 // PST_DICT
   07 // PST_PAIR (Key value pair)
      02 // PST_SYMBOL
      [LEN:2] // symbol length (uint16_t)
      [SYMBOL] // char*

      [VALUE_TYPE_TAG]
      [VALUE]

06 // PMT_NULL - pair split

   0a // PMT_UNIFORM_VECTOR
   [UNIFORM_VECTOR_SUBTYPE_TYPE_TAG]
   [LEN:4] // vector length (uint32_t)
   01 00 - unknown
   ... - vector data
```

