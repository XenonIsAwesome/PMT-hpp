#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "pmt.hpp"

namespace helpers::complex {
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

    void save_to_32fc(const std::vector<std::complex<float>>& data, const std::string& filename) {
        std::ofstream outfile(filename, std::ios::binary);
        if (!outfile) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }

        outfile.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(std::complex<float>));
    }

    std::vector<std::complex<float>> read_from_32fc(const std::string& filename) {
        std::ifstream infile(filename, std::ios::binary | std::ios::ate);
        if (!infile) {
            throw std::runtime_error("Failed to open file for reading: " + filename);
        }

        std::streamsize file_size = infile.tellg();
        if (file_size % sizeof(std::complex<float>) != 0) {
            throw std::runtime_error("File size is not aligned with complex<float> size");
        }

        infile.seekg(0, std::ios::beg);
        size_t num_items = file_size / sizeof(std::complex<float>);

        std::vector<std::complex<float>> data(num_items);
        infile.read(reinterpret_cast<char*>(data.data()), file_size);

        if (!infile) {
            throw std::runtime_error("Failed to read the complete file");
        }

        return data;
    }
}

namespace helpers::bin {
    void hexdump(const std::string& data) {
        const size_t bytes_per_line = 16;

        for (size_t i = 0; i < data.size(); i += bytes_per_line) {
            // Print offset
            std::cout << std::setw(8) << std::setfill('0') << std::hex << i << "  ";

            // Print hex bytes
            for (size_t j = 0; j < bytes_per_line; ++j) {
                if (i + j < data.size()) {
                    std::cout << std::setw(2) << std::setfill('0') << std::hex
                              << (static_cast<unsigned int>(static_cast<unsigned char>(data[i + j]))) << ' ';
                } else {
                    std::cout << "   ";
                }
            }

            // Print ASCII characters
            std::cout << " |";
            for (size_t j = 0; j < bytes_per_line && i + j < data.size(); ++j) {
                char c = data[i + j];
                std::cout << (std::isprint(static_cast<unsigned char>(c)) ? c : '.');
            }
            std::cout << '|' << std::endl;
        }
    }

    bool write_bin_file(const std::string& filename, const std::string& data) {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            return false;  // failed to open file
        }

        out.write(data.data(), data.size());
        return out.good();
    }
}

int main() {
    // 1. Make the metadata dictionary with the timetag
    pmt::pmt_t metadata = pmt::make_dict();
    pmt::dict_add(metadata, "timetag", 123.456);
    pmt::dict_add(metadata, "freq", 10e6);

    // 2. Convert the vector to a uniform vector PMT (complex float/double)
    static constexpr size_t n_items = 11;
    std::vector<std::complex<float>> samples = helpers::complex::read_from_32fc("samples.32fc");
    pmt::pmt_t vec = pmt::init_vector(n_items, samples.data());

    // 3. Make the PDU (pair of meta and vec)
    pmt::pmt_t pdu = pmt::cons(metadata, vec);

    helpers::bin::write_bin_file("pdu.bin", pdu);
    helpers::bin::hexdump(pdu);
}
