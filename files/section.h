#pragma once

#include<vector>

class Section {
public:
    Section(int marker, int get_cnt_byte_4_length) : marker_(marker),
                                                     get_cnt_byte_4_length_(get_cnt_byte_4_length), length_(0),
                                                     buffer_({}) {
    }

    Section() {}

    void add_buffer(int num);

    int get_marker() const;
    int get_length() const;
    int get_cnt_byte_4_length() const;

    int get_buffer_el(int l, int r) const;
    int get_buffer_el(int ind) const;

    void add_length(int length);
private:
    int marker_;
    int length_;
    std::vector<int> buffer_;

    int get_cnt_byte_4_length_;
};