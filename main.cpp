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

    int get_buffer_el(int l, int r) {
        long long el = 0;
        for (int ind = l; ind <= r; ind++) {
            el = 16 * el + buffer_[ind];
        }
        return el;
    }

    int get_buffer_el(int ind) {
        return buffer_[ind];
    }

    void add_length(int length) {
         length_ = 16 * length_ + length;
    }
private:
    int marker_;
    int length_;
    std::vector<unsigned char> buffer_;

    int get_cnt_byte_4_length_;
};

const int MARKER = 0xff;

class table_quant {
public:
    table_quant(int length, int size_byte, int ind_table) : length_(length),
    size_byte_(size_byte), ind_table_(ind_table) {}

    int get_ind_table() const {
        return ind_table_;
    }

private:
    int length_;
    int size_byte_;
    int ind_table_;
};

void print_sof0_section(Section section) {
    cout << "length = " << section.get_length() << "\n";
    cout << "precision = " << section.get_buffer_el(2) << "\n";
    cout << "height = " << section.get_buffer_el(3, 4) << "\n";
    cout << "width = " << section.get_buffer_el(5, 6) << "\n";
    cout << "cnt channels = " << section.get_buffer_el(7) << "\n";

    for (int i = 8; i < section.get_length(); i += 3) {
        int ind = (i - 8)/3 + 1;
        cout << "ind = " << ind << "\n";
        cout << "H[" << ind << "]:" << " " << section.get_buffer_el(i + 1)/16 << "\n";
        cout << "V[" << ind << "]:" << " " << section.get_buffer_el(i + 1)%16 << "\n";
        cout << "ind table quant" << " " << section.get_buffer_el(i + 2) << "\n";
        cout << "\n";
    }
}

void Decode(string path) {
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size == 0) {
        throw std::runtime_error("file is empty");
    }

    std::vector<unsigned int> buffer(size);
    char byte;
    int ind = 0;
    while (file.get(byte)) {
        buffer[ind] = (int)static_cast<unsigned char>(byte);
        cout << buffer[ind] << " ";
        ind++;
        if(ind % 8 == 0) {
            cout << "\n";
        }
    }
    cout << "\n";

    std::vector<Section> sections;
    map<int, int> mapf;
    mapf[0xD8] = 0;
    mapf[0xE0] = 2;
    mapf[0xDB] = 2;
    mapf[0xC0] = 2;
    mapf[0xC4] = 2;
    mapf[0xDA] = 2;
    mapf[0xD9] = 0;

    map<int, table_quant> table_quants;

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
                this_section.add_length(static_cast<int>(buffer[j]));
            }
            for (int j = indStart; j < indStart + this_section.get_length(); j++) {
                this_section.add_buffer(buffer[j]);
            }
            i = indStart + this_section.get_length();
            sections.push_back(this_section);

            if(this_section.get_marker() == 0xDA) {
                findDA = true;
            } else if(this_section.get_marker() == 0xDB) {
                table_quant new_table_quant(this_section.get_length(), this_section.get_buffer_el(2)/16, this_section.get_buffer_el(2)%16);
                table_quants[new_table_quant.get_ind_table()] = new_table_quant;
            }
            //cout << indStart << " " << this_section.get_length() << " " << buffer.size() << "\n";
            //cout << '\n';
            //int a;
            //cin >> a;
        } else if(findDA) {
            nums.push_back(buffer[i]);
            i++;
        } else {
            //throw
        }
    }

    cout << "end.\n";
}

int main() {
    //Decode("progressive.jpg");
    Decode("tiny.jpg");
}
