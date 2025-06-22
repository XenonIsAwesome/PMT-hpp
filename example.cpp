#include <fstream>
#include <iostream>
#include <vector>
#include <pmt.hpp>

/**
 * Reads a .32fc file into a vector of complex floats.
 * @param filepath Path of the .32fc file
 * @return File data
 */
std::vector<std::complex<float>> read_samples(const std::string &filepath) {
    std::ifstream samples_file(filepath, std::ios::binary);

    std::stringstream err_ss;
    if (!samples_file) {
        err_ss << "Could not open file " << filepath << " for reading.";
        throw std::runtime_error(err_ss.str());
    }

    std::vector<std::complex<float>> samples;
    while (true) {
        float real, imag;
        samples_file.read(reinterpret_cast<char *>(&real), sizeof(real));
        samples_file.read(reinterpret_cast<char *>(&imag), sizeof(imag));

        samples.emplace_back(real, imag);
        if (samples_file.eof()) break;
        if (!samples_file) {
            err_ss << "Error while reading samples from file " << filepath << ".";
            throw std::runtime_error(err_ss.str());
        }
    }

    samples_file.close();
    return samples;
}

/**
 * Encodes a PDU packet based on given data.
 * @param samples Samples to encode in the PDU
 * @param n_samples The amount of samples
 * @param timetag The timetag to add to the metadata.
 * @return PMT object of the PDU
 */
pmt::pmt_t make_pdu(const std::complex<float>* samples, size_t n_samples, float timetag) {
    // 1. Creating the metadata pmt.
    pmt::pmt_t meta = pmt::make_dict();
    pmt::dict_add(meta, "timetag", timetag);

    // 2. Creating the vector pmt.
    pmt::pmt_t vec = pmt::init_vector(n_samples, samples);

    // 3. Combining into PDU
    pmt::pmt_t pdu = pmt::cons(meta, vec);
    return pdu;
}

int main() {
    auto samples = read_samples("samples.32fc");
    pmt::pmt_t pdu = make_pdu(samples.data(), samples.size(), 123.456f);
    std::cout << pdu;
}