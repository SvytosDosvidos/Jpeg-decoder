#include "parser.h"

const int MARKER = 0xff;

MarkerVariant CreateTableQuant(Section &section) {
    int length = section.get_length();

    if (length != 67) {
        throw std::logic_error("Incorrect file structure .jpg");
    }

    int size_byte = 1 + section.get_buffer_el(2) / 16;
    int ind_table = section.get_buffer_el(2) % 16;

    std::vector<std::vector<int>> matrix = TableQuant::create_matrix(section);

    return TableQuant(length, size_byte, ind_table, matrix);
}

MarkerVariant CreateSof0(Section &section) {
    int length = section.get_length();

    if (length < 8) {
        throw std::logic_error("Incorrect file structure .jpg");
    }

    int precision = section.get_buffer_el(2);
    int height = section.get_buffer_el(3, 4);
    int width = section.get_buffer_el(5, 6);
    int cnt_channels = section.get_buffer_el(7);
    std::vector<Channel> channels(cnt_channels);

    if (3 * cnt_channels + 8 != length) {
        throw std::logic_error("Incorrect file structure .jpg");
    }

    for (int i = 8; i < section.get_length(); i += 3) {
        int ind = (i - 8) / 3;
        channels[ind] = {
            section.get_buffer_el(i), section.get_buffer_el(i + 1) / 16,
            section.get_buffer_el(i + 1) % 16, section.get_buffer_el(i + 2)
        };
    }

    sort(channels.begin(), channels.end(), Sof0::sort_channel);

    if (channels.size() != 3) {
        throw std::logic_error("Incorrect file structure .jpg");
    }

    return Sof0(length, precision, height, width, cnt_channels, channels);
}

MarkerVariant CreateDHT(Section &section) {
    int length = section.get_length();
    int sum_bytes = 0;

    if (length < 19) {
        throw std::logic_error("Incorrect file structure .jpg");
    }

    int type_dht = section.get_buffer_el(2) / 16;
    int id = section.get_buffer_el(2) % 16;

    std::vector<std::vector<int>> codes(16);

    for (int i = 3; i < 19; i++) {
        sum_bytes += section.get_buffer_el(i);
    }

    if (3 + 16 + sum_bytes != length) {
        throw std::logic_error("Incorrect file structure .jpg");
    }

    int ind = 19;
    for (int i = 0; i < 16; i++) {
        codes[i].resize(section.get_buffer_el(i + 3));
        for (int j = 0; j < codes[i].size(); j++) {
            codes[i][j] = section.get_buffer_el(j + ind);
        }
        ind += codes[i].size();
    }

    tree *start;

    bool flag_create_tree = true;
    std::map<std::string, int> tree_list;
    Dht::create_tree(start, codes, tree_list, flag_create_tree);

    if (!flag_create_tree) {
        throw std::logic_error("Incorrect file structure .jpg");
    }
    return Dht(length, type_dht, id, flag_create_tree, tree_list);
}

MarkerVariant CreateSos(Section &section) {
    int length = section.get_length();

    if (length < 3) {
        throw std::logic_error("Incorrect file structure .jpg");
    }

    int cnt_channels = section.get_buffer_el(2);

    if (2 * cnt_channels + 3 + 3 != length) {
        throw std::logic_error("error");
    }

    std::vector<channel_sos> channels(cnt_channels);
    for (int i = 0; i < cnt_channels; i++) {
        channel_sos cur_channel;
        cur_channel.id = section.get_buffer_el(2 * i + 3);
        cur_channel.id_DC = section.get_buffer_el(2 * i + 4) / 16;
        cur_channel.id_AC = section.get_buffer_el(2 * i + 4) % 16;
        channels[i] = cur_channel;
    }

    if (section.get_buffer_el(2 * cnt_channels + 3) != 0x00 &&
        section.get_buffer_el(2 * cnt_channels + 4) != 0x3F &&
        section.get_buffer_el(2 * cnt_channels + 5) != 0x00) {
        throw std::logic_error("Incorrect file structure .jpg");
        }

    sort(channels.begin(), channels.end(), Sos::sort_sos);

    return Sos(length, cnt_channels, channels);
}

void Parser::intiCreator() {
    creator_marker_.AddCreatorMarker("table_quant", CreateTableQuant);
    creator_marker_.AddCreatorMarker("sof0", CreateSof0);
    creator_marker_.AddCreatorMarker("dht", CreateDHT);
    creator_marker_.AddCreatorMarker("sos", CreateSos);
}

void Parser::parse(std::string path) {
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

    intiCreator();

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
                MarkerVariant marker = creator_marker_.CreateMarker("table_quant", this_section);
                TableQuant table_quant = std::get<TableQuant>(marker);
                table_quants_.push_back(table_quant);
                map_table_quants_[table_quant.get_ind_table()] = table_quant;
            } else if (this_section.get_marker() == 0xC0) {
                MarkerVariant marker = creator_marker_.CreateMarker("sof0", this_section);
                Sof0 sof0 = std::get<Sof0>(marker);
                sof0s_.push_back(sof0);
            } else if (this_section.get_marker() == 0xC4) {
                MarkerVariant marker = creator_marker_.CreateMarker("dht", this_section);
                Dht dht = std::get<Dht>(marker);
                dhts_.push_back(dht);
            } else if (this_section.get_marker() == 0xDA) {
                findDA = true;
                MarkerVariant marker = creator_marker_.CreateMarker("sos", this_section);
                Sos sos = std::get<Sos>(marker);
                soss_.push_back(sos);
            }
        } else if (findDA) {
            end_symbols_.push_back(buffer[i]);
            i++;
        } else {
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

int Parser::get_size_end_symbols() const {
    return end_symbols_.size();
}

int Parser::get_el_end_symbol(int ind) const {
    return end_symbols_[ind];
}