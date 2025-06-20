# PMT (Polymorphic Types)
A single header file that mimics gnuradios PMT to allow building PDU (protocol data unit) buffers.

## Common PMT Type Codes
| **Type Name**        | **Type Tag (Hex)**                           | **Meaning / Notes**                                  |
| -------------------- |----------------------------------------------| ---------------------------------------------------- |
| **PMT\_NIL**         | `00`                                         | Empty / null object (`pmt::PMT_NIL`)                 |
| **PMT\_BOOL**        | `01`                                         | Boolean (`01` = true, `02` = false)                  |
| **PMT\_INTEGER**     | `0b`                                         | 64-bit signed integer                                |
| **PMT\_U64**         | `0b`                                         | Unsigned 64-bit integer (same as signed, stored raw) |
| **PMT\_REAL**        | `04`                                         | 64-bit IEEE double precision float                   |
| **PMT\_SYMBOL**      | `09 07 02 00 [LEN:4] [DATA:0-255]`           | Symbol/string                                        |
| **PMT\_STRING**      | `09 07 01 00 [LEN:4] [DATA:0-255]`           | String                                               |
| **PMT\_PAIR** (cons) | `06`                                         | Pair (used for PDUs → `cons(meta, vector)`)          |
| **PMT\_DICT**        | `07`                                         | Dictionary (`pmt::make_dict()`)                      |
| **PMT\_VECTOR**      | `09 07 0e 00 [LEN:4] [DATA:0-4294967295]`    | Generic vector                                       |
| **PMT\_C32VECTOR**   | `0a 0a 00 00 00 [LEN:4] [DATA:0-4294967295]` | Uniform vector of complex float32                    |
| **PMT\_C64VECTOR**   | `0a 0a 00 00 01 [LEN:4] [DATA:0-4294967295]` | Uniform vector of complex float64                    |
| **PMT\_S8VECTOR**    | `0a 0a 00 00 02 [LEN:4] [DATA:0-4294967295]` | Uniform vector of int8                               |
| **PMT\_S16VECTOR**   | `0a 0a 00 00 03 [LEN:4] [DATA:0-4294967295]` | Uniform vector of int16                              |
| **PMT\_S32VECTOR**   | `0a 0a 00 00 04 [LEN:4] [DATA:0-4294967295]` | Uniform vector of int32                              |
| **PMT\_S64VECTOR**   | `0a 0a 00 00 05 [LEN:4] [DATA:0-4294967295]` | Uniform vector of int64                              |
| **PMT\_U8VECTOR**    | `0a 0a 00 00 06 [LEN:4] [DATA:0-4294967295]` | Uniform vector of uint8                              |
| **PMT\_U16VECTOR**   | `0a 0a 00 00 07 [LEN:4] [DATA:0-4294967295]` | Uniform vector of uint16                             |
| **PMT\_U32VECTOR**   | `0a 0a 00 00 08 [LEN:4] [DATA:0-4294967295]` | Uniform vector of uint32                             |
| **PMT\_U64VECTOR**   | `0a 0a 00 00 09 [LEN:4] [DATA:0-4294967295]` | Uniform vector of uint64                             |

## PDU Structure
1. `07` - PMT Dict type
2. Dict key/value pair sequence
    1. **Key:** `09 07 02 00 [LEN:1] [DATA:0-255]` - Symbol/string
    2. **Value:** `[Type Tag] [LEN:1?] [DATA]`
3. `06` - PMT Pair type
4. `[PMT_<type>VECTOR Type Tag] [LEN:4] [DATA:0-4294967295]`

### Example
```cpp
#include "pmt.hpp"

pmt::pmt_t make_pdu(std::complex<float>* samples, size_t n_samples, float timetag) {
   // 1. Creating the metadata pmt.
   pmt::pmt_t meta = pmt::make_dict();
   pmt::dict_add(meta, "timetag", timetag);
   
   // 2. Creating the vector pmt.
   pmt::pmt_t vec = pmt::init_c32vector(n_samples, samples);
   
   // 3. Combining into PDU
   pmt::pmt_t pdu = pmt::cons(meta, vec);
   return pdu;
}
```