#include "btor2.hpp"

#include <fstream>

// conviennce functions for encoding
struct Encoding {
  static std::ofstream file;
  static int64_t id, bool_id, true_id, offset;

  void copy(const Btor2 &btor);
  void add_constants();
  Encoding(const char *path, const Btor2 &witness);
  Encoding(const char *path, const Btor2 &witness, const Btor2 &model);
  ~Encoding();
};

inline int64_t boolean_op(const char *op, int64_t a, int64_t b = 0,
                          int64_t c = 0) {
  Encoding::file << ++Encoding::id << " " << op << " " << Encoding::bool_id
                 << " " << a;
  if (b) Encoding::file << " " << b;
  if (c) Encoding::file << " " << c;
  Encoding::file << "\n";
  return Encoding::id;
}

// add a line and return the id
inline int64_t bnot(int64_t a) { return boolean_op("not", a); }
inline int64_t beq(int64_t a, int64_t b) { return boolean_op("eq", a, b); }
inline int64_t bbad(int64_t a) {
  Encoding::file << ++Encoding::id << " bad " << a;
  return Encoding::id;
}

int64_t band(const std::vector<int64_t> &v);
inline int64_t band(int64_t a) { return a; }
template <typename... Args> inline int64_t band(int64_t a, Args... args) {
  return boolean_op("and", a, band(args...));
}

int64_t bor(const std::vector<int64_t> &v);
inline int64_t bor(int64_t a) { return a; }
template <typename... Args> inline int64_t bor(int64_t a, Args... args) {
  return boolean_op("or", a, bor(args...));
}

inline int64_t next(int64_t id) { return id + Encoding::offset; }
inline std::vector<int64_t> next(const std::vector<int64_t> &v) {
  auto r = std::views::transform(v, [](int64_t id) { return next(id); });
  return std::vector<int64_t>(r.begin(), r.end());
}

// May only be used directly after initializing the Encoding
void unroll(const Btor2 &btor);
