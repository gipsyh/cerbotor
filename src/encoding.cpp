#include "encoding.hpp"

#include <cassert>
#include <iostream>

void Encoding::copy(const Btor2 &btor) {
  for (auto l : btor) {
    if (l.tag == BTOR2_TAG_state) {
      l.tag = BTOR2_TAG_input;
      l.name = "input";
    }
    file << l;
  }
}

void Encoding::add_constants() {
  file << ++id << " sort bitvec 1\n";
  bool_id = id;
  file << ++id << " one " << bool_id << "\n";
  true_id = id;
}

Encoding::Encoding(const char *path, const Btor2 &witness) {
  file.open(path);
  copy(witness);
  id = witness.max_id;
  add_constants();
  offset = id;
}

Encoding::Encoding(const char *path, const Btor2 &witness, const Btor2 &model) {
  file.open(path);
  copy(witness);
  copy(model);
  id = model.max_id;
  add_constants();
  offset = id;
}

Encoding::~Encoding() {
  file.close();
  id = bool_id = true_id = offset = 0;
}

unsigned reduce(const char *op, std::vector<int64_t> v, int64_t empty_value) {
  if (v.empty()) return empty_value;
  const auto begin = v.begin();
  auto end = v.cend();
  bool odd = (begin - end) % 2;
  end -= odd;
  while (end - begin > 1) {
    auto j = begin, i = j;
    while (i != end)
      *j++ = boolean_op(op, *i++, *i++);
    if (odd) *j++ = *end;
    odd = (begin - j) % 2;
    end = j - odd;
  }
  const unsigned res = *begin;
  return res;
}
int64_t band(const std::vector<int64_t> &v) {
  return reduce("and", v, Encoding::true_id);
}
int64_t bor(const std::vector<int64_t> &v) {
  return reduce("or", v, bnot(Encoding::true_id));
}

void unroll(const Btor2 &btor) {
  for (auto l : btor) {
    ++Encoding::id;
    if (l.tag == BTOR2_TAG_state) {
      l.tag = BTOR2_TAG_input;
      l.name = "input";
    }
    l.id = next(l.id);
    assert(l.id == Encoding::id);
    l.sort.id = next(l.sort.id);
    if (l.sort.tag == BTOR2_TAG_SORT_array) {
      l.sort.array.index = next(l.sort.array.index);
      l.sort.array.element = next(l.sort.array.element);
    }
    int64_t all_args = l.nargs;
    // These three have "indexed" arguments.
    // These don't count torwards nargs and
    // should not be shifted.
    if (l.tag == BTOR2_TAG_uext || l.tag == BTOR2_TAG_sext)
      all_args += 1;
    else if (l.tag == BTOR2_TAG_slice)
      all_args += 2;
    std::vector<int64_t> args(all_args, -1);
    for (int32_t i = 0; i < l.nargs; ++i)
      args[i] = next(l.args[i]);
    for (int32_t i = l.nargs; i < all_args; ++i)
      args[i] = l.args[i];
    l.args = args.data();
    Encoding::file << l;
  }
}
