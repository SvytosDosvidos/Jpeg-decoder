#include "section.h"

void Section::add_buffer(int num) {
    if (buffer_.capacity() != length_) {
        buffer_.reserve(length_);
    }
    buffer_.push_back(num);
}

int Section::get_marker() const {
    return marker_;
}

int Section::get_length() const {
    return length_;
}

int Section::get_cnt_byte_4_length() const {
    return get_cnt_byte_4_length_;
}

int Section::get_buffer_el(int l, int r) const {
    long long el = 0;
    for (int ind = l; ind <= r; ind++) {
        el = 16 * el + buffer_[ind];
    }
    return el;
}

int Section::get_buffer_el(int ind) const {
    return buffer_[ind];
}

void Section::add_length(int length) {
    length_ = 16 * length_ + length;
}