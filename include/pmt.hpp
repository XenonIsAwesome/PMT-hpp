#ifndef PMT_H
#define PMT_H

#include <string>
#include <sstream>
#include <cstring>


namespace pmt {
    namespace type_tags {
#define PMT_TYPE_TAGS\
    X(NIL, "\x00") \
    X(BOOL, "\x01") /* true = \x01, false = \x02 */ \
    X(INTEGER, "\x0b") /* same as PMT_U64 */ \
    X(U64, "\x0b") /* same tag as PMT_INTEGER */ \
    X(REAL, "\x04") \
    \
    X(SYMBOL, "\x09\x07\x02\x00") /* [LEN:1] [DATA] */ \
    X(STRING, "\x09\x07\x01\x00") /* [LEN:1] [DATA] */ \
    \
    X(PAIR, "\x06") \
    X(DICT, "\x07") \
    X(VECTOR, "\x09\x07\x0e\x00") /* [LEN:4] [DATA] */ \
    \
    X(C32VECTOR, "\x0a\x0a\x00\x00\x00") /* [LEN:4] [DATA] */ \
    X(C64VECTOR, "\x0a\x0a\x00\x00\x01") /* [LEN:4] [DATA] */ \
    X(S8VECTOR, "\x0a\x0a\x00\x00\x02") /* [LEN:4] [DATA] */ \
    X(S16VECTOR, "\x0a\x0a\x00\x00\x03") /* [LEN:4] [DATA] */ \
    X(S32VECTOR, "\x0a\x0a\x00\x00\x04") /* [LEN:4] [DATA] */ \
    X(S64VECTOR, "\x0a\x0a\x00\x00\x05") /* [LEN:4] [DATA] */ \
    X(U8VECTOR, "\x0a\x0a\x00\x00\x06") /* [LEN:4] [DATA] */ \
    X(U16VECTOR, "\x0a\x0a\x00\x00\x07") /* [LEN:4] [DATA] */ \
    X(U32VECTOR, "\x0a\x0a\x00\x00\x08") /* [LEN:4] [DATA] */ \
    X(U64VECTOR, "\x0a\x0a\x00\x00\x09") /* [LEN:4] [DATA] */ \

#define X(name, value) \
        static constexpr auto PMT_##name = std::string(value, sizeof(value));
PMT_TYPE_TAGS
#undef X

    }

    class not_implemented : public std::logic_error {
    public:
        explicit not_implemented(const std::string& name): std::logic_error(name + " not implemented") {}
    };

    using pmt_t = std::string;

    void make_dict(pmt_t &pmt);
    pmt_t cons(const pmt_t& metadata, const pmt_t& vec);

    template<typename T>
    void dict_add(pmt_t &pmt, const std::string& key, T value);

    void symbol(pmt_t &pmt, const std::string& symbol);

    template<typename T>
    void from(pmt_t &, T);

    template<>
    void from<const std::string &>(pmt_t &pmt, const std::string& value);

    template<>
    void from<const char *>(pmt_t &pmt, const char *value);

    template<>
    void from<bool>(pmt_t &pmt, bool value);

    template<>
    void from<int>(pmt_t &pmt, int value);

    template <>
    void from<size_t>(pmt_t &pmt, size_t value);

    template<>
    void from<float>(pmt_t &pmt, float value);

    template<>
    void from<double>(pmt_t &pmt, double value);

    pmt_t init_c32vector(uint32_t n_items, std::complex<float>* data);
    pmt_t init_c64vector(uint32_t n_items, std::complex<double>* data);
    /// TODO: add all the vectors

}

inline void pmt::make_dict(pmt_t &pmt) {
    pmt += type_tags::PMT_DICT;
}

inline pmt::pmt_t pmt::cons(const pmt_t& metadata, const pmt_t& vec) {
    return metadata + type_tags::PMT_PAIR + vec;
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
    pmt.append(type_tags::PMT_BOOL);
    pmt.append(value ? "\x01": "\x02", 1);
}

template<>
inline void pmt::from<const std::string&>(pmt_t &pmt, const std::string& value) {
    from<const char *>(pmt, value.data());
}

template<>
inline void pmt::from<const char *>(pmt_t &pmt, const char* value) {
    pmt.append(type_tags::PMT_STRING);
    pmt.append(value, strlen(value));
}

template<>
inline void pmt::from<int>(pmt_t &pmt, int value) {
    pmt.append(type_tags::PMT_INTEGER);
    pmt.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

template<>
inline void pmt::from<size_t>(pmt_t &pmt, size_t value) {
    pmt.append(type_tags::PMT_U64);
    pmt.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

template<>
inline void pmt::from<float>(pmt_t &pmt, float value) {
    pmt.append(type_tags::PMT_REAL);
    pmt.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

template<>
inline void pmt::from<double>(pmt_t &pmt, double value) {
    pmt.append(type_tags::PMT_REAL);
    pmt.append(reinterpret_cast<const char *>(&value), sizeof(value));
}

inline void pmt::symbol(pmt_t &pmt, const std::string &symbol) {
    pmt += type_tags::PMT_SYMBOL;
    pmt += static_cast<char>(symbol.size());  // Length of symbol
    pmt += symbol;                           // Symbol data
}

template<typename T>
void pmt::dict_add(pmt_t &pmt, const std::string &key, T value) {
    symbol(pmt, key);
    from<T>(pmt, value);
}

inline pmt::pmt_t pmt::init_c32vector(uint32_t n_items, std::complex<float> *data) {
    pmt_t pmt;

    pmt += type_tags::PMT_C32VECTOR;
    pmt.append(reinterpret_cast<const char *>(&n_items), sizeof(n_items));
    pmt.append(reinterpret_cast<const char*>(data), n_items * sizeof(std::complex<float>));

    return pmt;
}
inline pmt::pmt_t pmt::init_c64vector(uint32_t n_items, std::complex<double> *data) {
    pmt_t pmt;

    pmt += type_tags::PMT_C64VECTOR;
    pmt.append(reinterpret_cast<const char *>(&n_items), sizeof(n_items));
    pmt.append(reinterpret_cast<const char*>(data), n_items * sizeof(std::complex<double>));

    return pmt;
}


#endif // PMT_H
