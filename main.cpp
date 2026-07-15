#include<iostream>
#include<vector>
#include<string>
#include<set>
#include<map>
#include<math.h>
#include<algorithm>
#include<fstream>
#include<unordered_map>

using namespace std;

class Section {
public:
    Section(int marker, int get_cnt_byte_4_length) : marker_(marker),
    get_cnt_byte_4_length_(get_cnt_byte_4_length), length_(0), buffer_({}) {}

    void add_buffer(int num) {
        if (buffer_.capacity() != length_) {
            buffer_.reserve(length_);
        }
        buffer_.push_back(num);
    }

    int get_marker() const {
         return marker_;
    }

    int get_length() const {
        return length_;
    }

    int get_cnt_byte_4_length() const {
        return get_cnt_byte_4_length_;
    }

    int get_buffer_el(int l, int r) const {
        long long el = 0;
        for (int ind = l; ind <= r; ind++) {
            el = 16 * el + buffer_[ind];
        }
        return el;
    }

    int get_buffer_el(int ind) const {
        return buffer_[ind];
    }

    void add_length(int length) {
         length_ = 16 * length_ + length;
    }
private:
    int marker_;
    int length_;
    std::vector<int> buffer_;

    int get_cnt_byte_4_length_;
};

const int MARKER = 0xff;

class table_quant {
public:
    table_quant(const table_quant&) = default;
    table_quant(table_quant&&) = default;
    table_quant& operator=(const table_quant&) = default;
    table_quant& operator=(table_quant&&) = default;

    table_quant(Section &section) {
        length_ = section.get_length();
        size_byte_ = section.get_buffer_el(2)/16;
        ind_table_ = section.get_buffer_el(2)%16;
    }

    int get_ind_table() const {
        return ind_table_;
    }

private:
    int length_;
    int size_byte_;
    int ind_table_;
};

class sof0 {
public:
    sof0(Section &section) {
        length_ = section.get_length();
        precision_ = section.get_buffer_el(2);
        heigth_ = section.get_buffer_el(3, 4);
        width_ = section.get_buffer_el(5, 6);
        cnt_channels_ = section.get_buffer_el(7);
        channels_.resize(cnt_channels_);
        for (int i = 8; i < section.get_length(); i += 3) {
            int ind = (i - 8)/3;
            channels_[ind] = {section.get_buffer_el(i), section.get_buffer_el(i + 1)/16,
            section.get_buffer_el(i + 1)%16, section.get_buffer_el(i + 2)};
        }
    }

    void print_sof0_section() const {
        cout << std::dec << "length = " << length_ << "\n";
        cout << std::dec << "precision = " << precision_ << "\n";
        cout << std::dec << "height = " << heigth_ << "\n";
        cout << std::dec << "width = " << width_ << "\n";
        cout << std::dec << "cnt channels = " << cnt_channels_ << "\n";

        cout << "\n";
        for (int i = 0; i < cnt_channels_; i++) {
            cout << std::dec << "ind = " << channels_[i].id << "\n";
            cout << std::dec << "H[" << channels_[i].id << "]:" << " " << channels_[i].h << "\n";
            cout << std::dec << "V[" << channels_[i].id << "]:" << " " << channels_[i].w << "\n";
            cout << std::dec << "ind table quant" << " " << channels_[i].id_quant << "\n";
            cout << "\n";
        }
    }

private:
    struct channel {
        int id;
        int h;
        int w;
        int id_quant;
    };

    int length_;
    int precision_;
    int heigth_;
    int width_;
    int cnt_channels_;
    vector<channel> channels_;
};

class dht {
private:
    struct tree {
        int num;

        tree* l;
        tree* r;
    };

public:
    dht(Section &section) {
        length_ = section.get_length();
        class_ = section.get_buffer_el(2)/16;
        id_ = section.get_buffer_el(2)%16;

        codes_.resize(16);

        int ind = 19;
        for(int i = 0; i < 16; i++) {
            //cout << "\nsz = " << std::dec << section.get_buffer_el(i + 3) << "\n";
            codes_[i].resize(section.get_buffer_el(i + 3));
            for(int j = 0; j < codes_[i].size(); j++) {
                //cout << "pair = " << j + ind << " " << section.get_length() << "\n";
                codes_[i][j] = section.get_buffer_el(j + ind);
            }
            ind += codes_[i].size();
        }
    }

    void print_dht() const {
        cout << std::dec << "length = " << length_ << "\n";
        cout << std::dec << "class = " << class_ << "\n";
        cout << std::dec << "id = " << id_ << "\n";
        cout << "\n";

        for(int i = 0; i < 16; i++) {
            cout << std::dec << "id_block = " << i + 1 << "\n";
            cout << std::dec << "size_block = " << codes_[i].size() << "\n";
            for(auto &it : codes_[i]) {
                cout << it << " ";
            }
            cout << "\n\n";
        }
    }

    void dfs(tree *v, int num, int this_d, int d) {
        if(!v->l || (v->l && v->l->num == -1)) {
            if(!v->l) {
                *v->l = {-1, nullptr, nullptr};
                if(this_d == d) {
                    v->l->num = num;
                    return;
                } else {
                    dfs(v->l, num, this_d + 1, d);
                }
            } else {
                dfs(v->l, num, this_d + 1, d);
            }
        }

        if(!v->l || (v->l && v->l->num == -1)) {
            if(!v->l) {
                *v->l = {-1, nullptr, nullptr};
                if(this_d == d) {
                    v->l->num = num;
                    return;
                } else {
                    dfs(v->l, num, this_d + 1, d);
                }
            } else {
                dfs(v->l, num, this_d + 1, d);
            }
        }
    }

    void create_tree() {
        *start = {-1, nullptr, nullptr};
        for(int i = 0; i < codes_.size(); i++) {
            int cnt = codes_[i].size();
            while(cnt--) {

            }
        }
    }

private:
    int length_;
    int class_;
    int id_;
    vector<vector<int>> codes_;
    tree* start;
};

void Decode(string path) {
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size == 0) {
        throw std::runtime_error("file is empty");
    }

    std::vector<int> buffer(size);
    char byte;
    int ind = 0;
    while (file.get(byte)) {
        buffer[ind] = static_cast<int>(static_cast<unsigned char>(byte));
        cout << std::hex << buffer[ind] << " , " << std::dec << buffer[ind] << " | ";
        //cout << buffer[ind] << " | ";
        ind++;
        if(ind % 8 == 0) {
            cout << "\n";
        }
    }
    cout << "\n";

    std::vector<Section> sections;
    map<int, int> mapf;
    mapf[0xD8] = 0;
    mapf[0xFE] = 2;//0xFE, 0xE0 - in local test
    mapf[0xDB] = 2;
    mapf[0xC0] = 2;
    mapf[0xC4] = 2;
    mapf[0xDA] = 2;
    mapf[0xD9] = 0;

    vector<table_quant> table_quants;
    vector<sof0> sof0s;
    vector<dht> dhts;

    int i = 0;
    vector<int> nums;
    bool findDA = false;
    while (i + 1 < size) {
        if (buffer[i] == MARKER && mapf.find(buffer[i + 1]) != mapf.end()) {
            cout << "find\n";
            cout << std::hex << buffer[i] << " " << buffer[i + 1] << "\n";
            int indStart = i + 2;
            Section this_section(buffer[i + 1], mapf[buffer[i + 1]]);
            for (int j = indStart; j < indStart + this_section.get_cnt_byte_4_length(); j++) {
                this_section.add_length(buffer[j]);
            }
            cout << "add\n";
            for (int j = indStart; j < indStart + this_section.get_length(); j++) {
                this_section.add_buffer(buffer[j]);
                cout << std::dec << buffer[j] << " ";
            }
            cout << "\n\n";
            i = indStart + this_section.get_length();
            sections.push_back(this_section);

            if(this_section.get_marker() == 0xDA) {
                findDA = true;
            } else if(this_section.get_marker() == 0xDB) {
                table_quant new_table_quant(this_section);
                table_quants.push_back(new_table_quant);
            } else if(this_section.get_marker() == 0xC0) {
                sof0 new_sof0(this_section);
                sof0s.push_back(new_sof0);
            } else if(this_section.get_marker() == 0xC4) {
                dht new_dht(this_section);
                dhts.push_back(new_dht);
            }
        } else if(findDA) {
            nums.push_back(buffer[i]);
            i++;
        } else {
            //throw
        }
    }

    cout << "sof0\n\n";
    sof0s[0].print_sof0_section();
    cout << "\n";

    cout << "dht\n";
    for(auto &this_dht : dhts) {
        this_dht.print_dht();
        cout << "\n";
    }

    cout << "end.\n";
}

int main() {
    //Decode("progressive.jpg");
    Decode("test_jpg.jpg");
}
