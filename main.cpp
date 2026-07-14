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

    int get_marker() const { return marker_; }
    int get_length() const { return length_; }
    int get_cnt_byte_4_length() const { return get_cnt_byte_4_length_; }
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
    void add_length(int length) { length_ = 16 * length_ + length; }
private:
    int marker_;
    int length_;
    std::vector<unsigned char> buffer_;

    int get_cnt_byte_4_length_;
};

const int MARKER = 0xff;

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

    std::vector<Section> sections;

    Section start_section(0xD8, 0);
    Section comment_section(0xFE, 2);
    Section sof0_section(0xC0, 0);

    sections.push_back(start_section);
    sections.push_back(comment_section);
    sections.push_back(sof0_section);

    int i = 0;
    int ind_sector = 0;
    while (i + 1 < size && ind_sector < sections.size()) {
        Section this_section = sections[ind_sector];
        cout << "\n\n";
        cout << buffer[i] << " " << buffer[i + 1] << "\n";
        cout << MARKER << " " << this_section.get_marker() << "\n";
        cout << "\n";
        if (buffer[i] == MARKER && buffer[i + 1] == this_section.get_marker()) {
            int indStart = i + 2;
            for (int j = indStart; j < indStart + this_section.get_cnt_byte_4_length(); j++) {
                this_section.add_length(static_cast<int>(buffer[j]));
            }
            for (int j = indStart; j < indStart + this_section.get_length(); j++) {
                this_section.add_buffer(buffer[j]);
            }
            i = indStart + this_section.get_cnt_byte_4_length();
            ind_sector++;
        } else {
            //cout << "hui\n";
        }
        int a;
        cin >> a;
    }
}

int main() {
    Decode("tiny.jpg");
}
