#include "parser.h"

const int MARKER = 0xff;

void Parser::decode(std::string path) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        is_open_ = false;
        throw std::logic_error("The file does not open");
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    is_open_ = true;

    std::vector<int> buffer(size);
    char byte;
    int ind = 0;
    while (file.get(byte)) {
        buffer[ind] = static_cast<int>(static_cast<unsigned char>(byte));
        ind++;
    }

    std::vector<Section> sections;
    std::map<int, int> mapf;
    mapf[0xD8] = 0;//start marker
    mapf[0xFE] = 2;//comments marker
    mapf[0xDB] = 2;//table quant
    mapf[0xC0] = 2;//sof0
    mapf[0xC4] = 2;//dht
    mapf[0xDA] = 2;//sos
    mapf[0xD9] = 0;//
    //0xFE, 0xE0 - in local test

    int i = 0;
    bool findDA = false;
    while (i + 1 < size) {
        if (buffer[i] == MARKER && mapf.find(buffer[i + 1]) != mapf.end()) {
            int indStart = i + 2;
            Section this_section(buffer[i + 1], mapf[buffer[i + 1]]);
            for (int j = indStart; j < indStart + this_section.get_cnt_byte_4_length(); j++) {
                this_section.add_length(buffer[j]);
            }

            for (int j = indStart; j < indStart + this_section.get_length(); j++) {
                this_section.add_buffer(buffer[j]);
            }

            i = indStart + this_section.get_length();
            sections.push_back(this_section);

            if (this_section.get_marker() == 0xDB) {
                TableQuant new_table_quant(this_section);
                table_quants_.push_back(new_table_quant);
                map_table_quants_[new_table_quant.get_ind_table()] = new_table_quant;
            } else if (this_section.get_marker() == 0xC0) {
                Sof0 new_sof0(this_section);
                sof0s_.push_back(new_sof0);
            } else if (this_section.get_marker() == 0xC4) {
                Dht new_dht(this_section);
                dhts_.push_back(new_dht);
            } else if (this_section.get_marker() == 0xDA) {
                findDA = true;
                Sos new_sos(this_section);
                soss_.push_back(new_sos);
            }
        } else if (findDA) {
            end_symbols_.push_back(buffer[i]);
            i++;
        } else {
            throw std::logic_error("Incorrect file structure .jpg");
        }
    }

    corret_parse();
}

void Parser::corret_parse() {
    corret_parse(table_quants_);
    corret_parse(sof0s_);
    corret_parse(dhts_);
    corret_parse(soss_);
}

template<HasGetFlag T>
void Parser::corret_parse(std::vector<T> markers) {
    for (int i = 0; i < markers.size(); i++) {
        if (!markers[i].get_flag_use_bytes()) {
            throw std::logic_error("Incorrect file structure .jpg");
        }
    }
}

void Parser::ProcessingEndSymbols(std::vector<char> &bits) {
    for (int i = 0; i < end_symbols_.size(); i++) {
        int num = end_symbols_[i];
        std::string t;
        while (num > 0) {
            t.push_back('0' + num % 2);
            num /= 2;
        }

        while (t.size() < 8) {
            t.push_back('0');
        }

        while (!t.empty()) {
            bits.push_back(t.back());
            t.pop_back();
        }
    }
}

int Parser::get_size_table_quants() const {
    return table_quants_.size();
}

int Parser::get_size_sof0s() const {
    return sof0s_.size();
}

int Parser::get_size_dhts() const {
    return dhts_.size();
}

int Parser::get_size_soss() const {
    return soss_.size();
}

bool Parser::get_is_open() const {
    return is_open_;
}

Sof0 Parser::get_sof0(int ind) const {
    return sof0s_[ind];
}

Dht Parser::get_dht(int ind) const {
    return dhts_[ind];
}

TableQuant Parser::get_table_quant(int ind) const {
    return table_quants_[ind];
}

TableQuant Parser::get_map_table_quant(int ind) {
    return map_table_quants_[ind];
}

Sos Parser::get_sos(int ind) const {
    return soss_[ind];
}