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

        template<>
        inline uint16_t swap_endianess(const uint16_t value) {
            return (value >> 8) | (value << 8);
        }

        template<>
        inline int16_t swap_endianess(const int16_t value) {
            return static_cast<int16_t>(swap_endianess(static_cast<uint16_t>(value)));
        }

        template<>
        inline uint32_t swap_endianess(const uint32_t value) {
            return ((value & 0x000000FF) << 24) |
                   ((value & 0x0000FF00) << 8)  |
                   ((value & 0x00FF0000) >> 8)  |
                   ((value & 0xFF000000) >> 24);
        }

        template<>
        inline int32_t swap_endianess(const int32_t value) {
            return static_cast<int32_t>(swap_endianess(static_cast<uint32_t>(value)));
        }

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
            return static_cast<int64_t>(swap_endianess(static_cast<uint64_t>(value)));
        }

        template<>
        inline float swap_endianess(const float value) {
            uint32_t ival = *reinterpret_cast<const uint32_t *>(&value);
            ival = swap_endianess<uint32_t>(ival);

            return *reinterpret_cast<float *>(&ival);
        }

        template<>
        inline double swap_endianess(const double value) {
            uint64_t ival = *reinterpret_cast<const uint64_t *>(&value);
            ival = swap_endianess<uint64_t>(ival);

            return *reinterpret_cast<double *>(&ival);
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
        static constexpr auto PMT_UNKNOWN_POST_VECTOR_LENGTH = std::string("\x01\x00", 2);
        static constexpr auto PMT_TRUE = std::string("\x00", 1);
        static constexpr auto PMT_FALSE = std::string("\x01", 1);
        static constexpr auto PMT_SYMBOL = std::string("\x02", 1);
        static constexpr auto PMT_INT32 = std::string("\x03", 1);
        static constexpr auto PMT_DOUBLE = std::string("\x04", 1);
        static constexpr auto PMT_COMPLEX = std::string("\x05", 1);
        static constexpr auto PMT_NULL = std::string("\x06", 1);
        static constexpr auto PMT_PAIR = std::string("\x07", 1);
        static constexpr auto PMT_VECTOR = std::string("\x08", 1);
        static constexpr auto PMT_DICT = std::string("\x09", 1);

        static constexpr auto PMT_UNIFORM_VECTOR = std::string("\x0a", 1);
        namespace uniform_vector {
            static constexpr auto UVI_U8 = std::string("\x00", 1);
            static constexpr auto UVI_S8 = std::string("\x01", 1);
            static constexpr auto UVI_U16 = std::string("\x02", 1);
            static constexpr auto UVI_S16 = std::string("\x03", 1);
            static constexpr auto UVI_U32 = std::string("\x04", 1);
            static constexpr auto UVI_S32 = std::string("\x05", 1);
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

    /// TODO: add all the vectors
    /// TODO: add tuple support

}

inline pmt::pmt_t pmt::make_dict() {
    pmt_t pmt;
    return pmt;
}

template<typename T>
void pmt::dict_add(pmt_t &pmt, const std::string &key, T value) {
    pmt += serial_tags::PMT_DICT;
    pmt += serial_tags::PMT_PAIR;
    symbol(pmt, key);
    from<T>(pmt, value);
}


inline pmt::pmt_t pmt::cons(const pmt_t& metadata, const pmt_t& vec) {
    return serial_tags::PMT_PAIR + metadata + serial_tags::PMT_NULL + vec;
}

template<typename T>
void pmt::from(pmt_t &, T) {
#if defined(__clang__) || defined(__GNUC__)
    throw not_implemented(__PRETTY_FUNCTION__);
#elif defined(_MSC_VER)
    throw not_implemented(__FUNCSIG__);
#endif
}

template<>
inline void pmt::from<bool>(pmt_t &pmt, bool value) {
    pmt += value ? serial_tags::PMT_TRUE: serial_tags::PMT_FALSE;
}

template<>
inline void pmt::from<int32_t>(pmt_t &pmt, int32_t value) {
    pmt.append(serial_tags::PMT_INT32);
    auto swapped_val = helpers::swap_endianess(value);
    pmt.append(reinterpret_cast<const char *>(&swapped_val), sizeof(swapped_val));
}

template<>
inline void pmt::from<size_t>(pmt_t &pmt, size_t value) {
    pmt.append(serial_tags::PMT_UINT64);
    auto swapped_val = helpers::swap_endianess(value);
    pmt.append(reinterpret_cast<const char *>(&swapped_val), sizeof(swapped_val));
}

template<>
inline void pmt::from<float>(pmt_t &pmt, float value) {
    from<double>(pmt, value);
}

template<>
inline void pmt::from<double>(pmt_t &pmt, double value) {
    auto swapped_val = helpers::swap_endianess(value);
    pmt.append(serial_tags::PMT_DOUBLE);
    pmt.append(reinterpret_cast<const char *>(&swapped_val), sizeof(swapped_val));
}

inline void pmt::symbol(pmt_t &pmt, const std::string &symbol) {
    pmt += serial_tags::PMT_SYMBOL;
    uint16_t symbol_size = helpers::swap_endianess(static_cast<uint16_t>(symbol.size()));
    pmt.append(reinterpret_cast<char *>(&symbol_size), sizeof(uint16_t));  // Length of symbol
    pmt += symbol;                           // Symbol data
}

template<pmt::VectorItem T>
pmt::pmt_t pmt::init_vector(uint32_t n_items, const T *data) {
    pmt_t pmt;

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
        pmt += serial_tags::uniform_vector::UVI_S64;
    } else if constexpr (std::is_same_v<T, float>) {
        pmt += serial_tags::uniform_vector::UVI_F32;
    } else if constexpr (std::is_same_v<T, double>) {
        pmt += serial_tags::uniform_vector::UVI_F64;
    } else if constexpr (std::is_same_v<T, std::complex<float>>) {
        pmt += serial_tags::uniform_vector::UVI_C32;
    } else if constexpr (std::is_same_v<T, std::complex<double>>) {
        pmt += serial_tags::uniform_vector::UVI_C64;
    }

    auto size = helpers::swap_endianess(n_items);
    pmt.append(reinterpret_cast<const char *>(&size), sizeof(n_items));
    pmt += serial_tags::PMT_UNKNOWN_POST_VECTOR_LENGTH;
    pmt.append(reinterpret_cast<const char*>(data), n_items * sizeof(std::complex<float>));

    return pmt;
}

#endif // PMT_H
