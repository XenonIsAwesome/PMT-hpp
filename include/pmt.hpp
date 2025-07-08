#ifndef PMT_H
#define PMT_H

#include <string>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <complex>
#include <concepts>
#include <iomanip>

namespace pmt {
    namespace helpers {
        template<typename T>
        T swap_endianess(const T value) {
            return value;
        }

        /**
         * Generic function for swapping endianess of 16-bit values
         * @param value 16-bit value
         * @return 16-bit value in swapped endianess
         */
        template<>
        inline uint16_t swap_endianess(const uint16_t value) {
            return (value >> 8) | (value << 8);
        }

        template<>
        inline int16_t swap_endianess(const int16_t value) {
            /// Casting to uint16_t and back to take adventage of the existing uint16_t function.
            const uint16_t swapped_val = swap_endianess(*reinterpret_cast<const uint16_t *>(&value));
            return *reinterpret_cast<const int16_t *>(&swapped_val);
        }

        /**
         * Generic function for swapping endianess of 32-bit values
         * @param value 32-bit value
         * @return 32-bit value in swapped endianess
         */
        template<>
        inline uint32_t swap_endianess(const uint32_t value) {
            return ((value & 0x000000FF) << 24) |
                   ((value & 0x0000FF00) << 8)  |
                   ((value & 0x00FF0000) >> 8)  |
                   ((value & 0xFF000000) >> 24);
        }

        template<>
        inline int32_t swap_endianess(const int32_t value) {
            /// Casting to uint32_t and back to take adventage of the existing uint32_t function.
            const uint32_t swapped_val = swap_endianess(*reinterpret_cast<const uint32_t *>(&value));
            return *reinterpret_cast<const int32_t *>(&swapped_val);
        }

        /**
         * Generic function for swapping endianess of 64-bit values
         * @param value 64-bit value
         * @return 64-bit value in swapped endianess
         */
        template<>
        inline uint64_t swap_endianess(const uint64_t value) {
            return ((value & 0x00000000000000FFULL) << 56) |
                   ((value & 0x000000000000FF00ULL) << 40) |
                   ((value & 0x0000000000FF0000ULL) << 24) |
                   ((value & 0x00000000FF000000ULL) << 8)  |
                   ((value & 0x000000FF00000000ULL) >> 8)  |
                   ((value & 0x0000FF0000000000ULL) >> 24) |
                   ((value & 0x00FF000000000000ULL) >> 40) |
                   ((value & 0xFF00000000000000ULL) >> 56);
        }

        template<>
        inline int64_t swap_endianess(const int64_t value) {
            /// Casting to uint64_t and back to take adventage of the existing uint64_t function.
            const uint64_t swapped_val = swap_endianess(*reinterpret_cast<const uint64_t *>(&value));
            return *reinterpret_cast<const int64_t *>(&swapped_val);
        }

        template<>
        inline float swap_endianess(const float value) {
            /// Casting to uint32_t and back to take adventage of the existing uint32_t function.
            uint32_t ival = *reinterpret_cast<const uint32_t *>(&value);
            ival = swap_endianess<uint32_t>(ival);

            return *reinterpret_cast<float *>(&ival);
        }

        template<>
        inline double swap_endianess(const double value) {
            /// Casting to uint64_t and back to take adventage of the existing uint64_t function.
            uint64_t ival = *reinterpret_cast<const uint64_t *>(&value);
            ival = swap_endianess<uint64_t>(ival);

            return *reinterpret_cast<double *>(&ival);
        }

        template<>
        inline std::complex<float> swap_endianess(const std::complex<float> value) {
            /// Swapping endianess of each float as required by the PMT.
            return {
                swap_endianess<float>(value.real()),
                swap_endianess<float>(value.imag())
            };
        }

        template<>
        inline std::complex<double> swap_endianess(const std::complex<double> value) {
            /// Swapping endianess of each double as required by the PMT.
            return {
                swap_endianess<double>(value.real()),
                swap_endianess<double>(value.imag())
            };
        }
    }

    template<typename T>
    concept VectorItem = std::is_same_v<T, int8_t> || \
        std::is_same_v<T, uint8_t> || \
        std::is_same_v<T, int16_t> || \
        std::is_same_v<T, uint16_t> || \
        std::is_same_v<T, int32_t> || \
        std::is_same_v<T, uint32_t> || \
        std::is_same_v<T, int64_t> || \
        std::is_same_v<T, uint32_t> || \
        std::is_same_v<T, float> || \
        std::is_same_v<T, double> || \
        std::is_same_v<T, std::complex<float>> || \
        std::is_same_v<T, std::complex<double>>;

    namespace serial_tags {
        static auto PMT_UNKNOWN_POST_VECTOR_LENGTH = std::string("\x01\x00", 2);
        static auto PMT_TRUE = std::string("\x00", 1);
        static auto PMT_FALSE = std::string("\x01", 1);
        static auto PMT_SYMBOL = std::string("\x02", 1);
        static auto PMT_INT32 = std::string("\x03", 1);
        static auto PMT_DOUBLE = std::string("\x04", 1);
        static auto PMT_COMPLEX = std::string("\x05", 1);
        static auto PMT_NULL = std::string("\x06", 1);
        static auto PMT_PAIR = std::string("\x07", 1);
        static auto PMT_VECTOR = std::string("\x08", 1);
        static auto PMT_DICT = std::string("\x09", 1);

        static constexpr auto PMT_UNIFORM_VECTOR = std::string("\x0a", 1);
        namespace uniform_vector {
            static auto UVI_U8 = std::string("\x00", 1);
            static auto UVI_S8 = std::string("\x01", 1);
            static auto UVI_U16 = std::string("\x02", 1);
            static auto UVI_S16 = std::string("\x03", 1);
            static auto UVI_U32 = std::string("\x04", 1);
            static auto UVI_S32 = std::string("\x05", 1);
            static constexpr auto UVI_U64 = std::string("\x06", 1);
            static constexpr auto UVI_S64 = std::string("\x07", 1);
            static constexpr auto UVI_F32 = std::string("\x08", 1);
            static constexpr auto UVI_F64 = std::string("\x09", 1);
            static constexpr auto UVI_C32 = std::string("\x0a", 1);
            static constexpr auto UVI_C64 = std::string("\x0b", 1);
        }

        static constexpr auto PMT_UINT64 = std::string("\x0b", 1);
        static constexpr auto PMT_TUPLE = std::string("\x0c", 1);
        static constexpr auto PMT_INT64 = std::string("\x0d", 1);
    }

    class not_implemented : public std::logic_error {
    public:
        explicit not_implemented(const std::string& name): std::logic_error(name + " not implemented") {}
    };

    using pmt_t = std::string;

    pmt_t make_dict();
    pmt_t cons(const pmt_t& metadata, const pmt_t& vec);

    template<typename T>
    void dict_add(pmt_t &pmt, const std::string& key, T value);

    void symbol(pmt_t &pmt, const std::string& symbol);

    template<typename T>
    void from(pmt_t &, T);

    template<>
    void from<bool>(pmt_t &pmt, bool value);

    template<>
    void from<int32_t>(pmt_t &pmt, int value);

    template <>
    void from<size_t>(pmt_t &pmt, size_t value);

    /// TODO: add all int sizes

    template<>
    void from<float>(pmt_t &pmt, float value);

    template<>
    void from<double>(pmt_t &pmt, double value);

    template<VectorItem T>
    pmt_t init_vector(uint32_t n_items, const T *data);

    /// TODO: add tuple support

}

inline pmt::pmt_t pmt::make_dict() {
    /// Does nothing in my implementation besides createing an empty pmt_t
    /// Exists only for compatibilty purposes
    pmt_t pmt;
    return pmt;
}

template<typename T>
void pmt::dict_add(pmt_t &pmt, const std::string &key, T value) {
    /// Adding dict and pair type tags
    pmt += serial_tags::PMT_DICT;
    pmt += serial_tags::PMT_PAIR;

    /// Appending key and value
    symbol(pmt, key);
    from<T>(pmt, value);
}


inline pmt::pmt_t pmt::cons(const pmt_t& metadata, const pmt_t& vec) {
    return serial_tags::PMT_PAIR + metadata + serial_tags::PMT_NULL + vec;
}

template<typename T>
void pmt::from(pmt_t &, T) {
    /// Default function that throws not implemented
#if defined(__clang__) || defined(__GNUC__)
    throw not_implemented(__PRETTY_FUNCTION__);
#elif defined(_MSC_VER)
    throw not_implemented(__FUNCSIG__);
#endif
}

template<>
inline void pmt::from<bool>(pmt_t &pmt, bool value) {
    /// Appending PMT_TRUE or PMT_FALSE based on the value
    pmt += value ? serial_tags::PMT_TRUE: serial_tags::PMT_FALSE;
}

template<>
inline void pmt::from<int32_t>(pmt_t &pmt, int32_t value) {
    /// Appending int32_t type tag
    pmt += serial_tags::PMT_INT32;

    /// Appending value in big endian
    auto swapped_val = helpers::swap_endianess(value);
    pmt.append(reinterpret_cast<const char *>(&swapped_val), sizeof(swapped_val));
}

template<>
inline void pmt::from<size_t>(pmt_t &pmt, size_t value) {
    /// Appending uint64_t type tag
    pmt += serial_tags::PMT_UINT64;

    /// Appending value in big endian
    auto swapped_val = helpers::swap_endianess(value);
    pmt.append(reinterpret_cast<const char *>(&swapped_val), sizeof(swapped_val));
}

template<>
inline void pmt::from<float>(pmt_t &pmt, float value) {
    /// float == double in PMT
    from<double>(pmt, value);
}

template<>
inline void pmt::from<double>(pmt_t &pmt, double value) {
    /// Appending double type tag
    pmt += serial_tags::PMT_DOUBLE;

    /// Appending value in big endian
    auto swapped_val = helpers::swap_endianess(value);
    pmt.append(reinterpret_cast<const char *>(&swapped_val), sizeof(swapped_val));
}

inline void pmt::symbol(pmt_t &pmt, const std::string &symbol) {
    /// Appending symbol type tag
    pmt += serial_tags::PMT_SYMBOL;

    /// Appending symbol size in big endian
    uint16_t symbol_size = helpers::swap_endianess(static_cast<uint16_t>(symbol.size()));
    pmt.append(reinterpret_cast<char *>(&symbol_size), sizeof(uint16_t));

    /// Appending symbol
    pmt += symbol;
}

template<pmt::VectorItem T>
pmt::pmt_t pmt::init_vector(uint32_t n_items, const T *data) {
    pmt_t pmt;

    /// Appending vector type tag
    pmt += serial_tags::PMT_UNIFORM_VECTOR;
    if constexpr (std::is_same_v<T, int8_t>) {
        pmt += serial_tags::uniform_vector::UVI_S8;
    } else if constexpr (std::is_same_v<T, uint8_t>) {
        pmt += serial_tags::uniform_vector::UVI_U8;
    } else if constexpr (std::is_same_v<T, int16_t>) {
        pmt += serial_tags::uniform_vector::UVI_S16;
    } else if constexpr (std::is_same_v<T, uint16_t>) {
        pmt += serial_tags::uniform_vector::UVI_U16;
    } else if constexpr (std::is_same_v<T, int32_t>) {
        pmt += serial_tags::uniform_vector::UVI_S32;
    } else if constexpr (std::is_same_v<T, uint32_t>) {
        pmt += serial_tags::uniform_vector::UVI_U32;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        pmt += serial_tags::uniform_vector::UVI_S64;
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        pmt += serial_tags::uniform_vector::UVI_U64;
    } else if constexpr (std::is_same_v<T, float>) {
        pmt += serial_tags::uniform_vector::UVI_F32;
    } else if constexpr (std::is_same_v<T, double>) {
        pmt += serial_tags::uniform_vector::UVI_F64;
    } else if constexpr (std::is_same_v<T, std::complex<float>>) {
        pmt += serial_tags::uniform_vector::UVI_C32;
    } else if constexpr (std::is_same_v<T, std::complex<double>>) {
        pmt += serial_tags::uniform_vector::UVI_C64;
    }

    /// Reserving first for performance
    pmt.reserve(pmt.size() + n_items * sizeof(T));

    /// Appending vector size in big endian
    auto size = helpers::swap_endianess(n_items);
    pmt.append(reinterpret_cast<const char *>(&size), sizeof(uint32_t));

    pmt += serial_tags::PMT_UNKNOWN_POST_VECTOR_LENGTH;

    /// Append each item from the data in big endian
    for (size_t i = 0; i < n_items; i++) {
        const T val = data[i];
        T swapped_val = helpers::swap_endianess<T>(val);
        pmt.append(reinterpret_cast<const char *>(&swapped_val), sizeof(T));
    }

    return pmt;
}

#endif // PMT_H
