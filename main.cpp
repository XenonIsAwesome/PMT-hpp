#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "pmt.hpp"

std::vector<std::complex<float>> random_vcf(size_t n_items) {
    std::vector<std::complex<float>> result;
    result.reserve(n_items);

    // Random number generation setup
    std::random_device rd;                           // Seed for random engine
    std::mt19937 gen(rd());                          // Mersenne Twister RNG
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);  // Uniform distribution [-1, 1]

    // Generate random complex<float> numbers
    for (size_t i = 0; i < n_items; ++i) {
        result.emplace_back(dist(gen), dist(gen));
    }

    return result;
}

void print_hex(const std::string &data) {
    static constexpr size_t bytes_per_line = 16;
    size_t i = 0;

    for (auto c : data) {
        std::cout << std::setw(2) << std::setfill('0') << std::hex
                      << (static_cast<unsigned int>(static_cast<unsigned char>(c))) << ' ';
        if (i++ > bytes_per_line) {
            i = 0;
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

bool write_bin_file(const std::string& filename, const std::string& data) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        return false;  // failed to open file
    }

    out.write(data.data(), data.size());
    return out.good();
}

int main() {
    // 1. Make the metadata dictionary with the timetag
    pmt::pmt_t metadata;

    pmt::make_dict(metadata);
    pmt::dict_add(metadata, "timetag", 123.456f);

    // 2. Convert the vector to a uniform vector PMT (complex float/double)
    static constexpr size_t n_items = 10;
    std::vector<std::complex<float>> samples = random_vcf(n_items);
    pmt::pmt_t vec = pmt::init_c32vector(n_items, samples.data());
    print_hex(vec);

    // 3. Make the PDU (pair of meta and vec)
    pmt::pmt_t pdu = pmt::cons(metadata, vec);

    write_bin_file("pdu.bin", pdu);
    // print_hex(pdu);
}
