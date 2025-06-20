#include "pmt.hpp"

pmt::pmt_t make_pdu(const std::complex<float>* samples, size_t n_samples, float timetag) {
    // 1. Creating the metadata pmt.
    pmt::pmt_t meta;
    pmt::make_dict(meta);
    pmt::dict_add(meta, "timetag", timetag);

    // 2. Creating the vector pmt.
    pmt::pmt_t vec = pmt::init_c32vector(n_samples, samples);

    // 3. Combining into PDU
    pmt::pmt_t pdu = pmt::cons(meta, vec);
    return pdu;
}

int main() {
    const char *samples = "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd";
    auto pdu = make_pdu(reinterpret_cast<const std::complex<float>*>(samples), 1, 123.456);
    return 0;
}