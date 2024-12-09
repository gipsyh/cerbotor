#pragma once

#include "btor2parser.h"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <utility>
#include <vector>

// wraps the btor2parser library and add some modern c++ features
class Btor2 {
public:
  Btor2(const char *path);
  ~Btor2();
  int64_t size() const { return std::max(0L, btor2parser_max_id(parser)) + 1; }
  int64_t
  reindex(int64_t offset = 0,
          const std::vector<std::pair<int64_t, int64_t>> &pre_indexed = {});
  std::vector<std::pair<Btor2Line, int64_t>> get_simulation();
  std::vector<std::pair<Btor2Line, int64_t>>
  get_default_simulation(const Btor2 &simulated);
  void drop(int64_t id) { btor2parser_delete_by_id(parser, id); }
  Btor2Line at(int64_t id) { return *btor2parser_get_line_by_id(parser, id); }

  int64_t max_id{}, num_states{}, num_inputs{};
  std::vector<int64_t> bads, constraints;

  class iterator {
  public:
    using value_type = Btor2Line;
    using reference = Btor2Line &;
    using pointer = Btor2Line *;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using iterator_concept = std::input_iterator_tag;

    iterator() : parser_(nullptr), line_(nullptr) {} // End iterator
    iterator(Btor2Parser *parser) : parser_(parser) {
      it_ = btor2parser_iter_init(parser_);
      line_ = btor2parser_iter_next(&it_);
    }

    reference operator*() const { return *line_; }
    pointer operator->() const { return line_; }
    iterator &operator++() {
      line_ = btor2parser_iter_next(&it_);
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    bool operator==(const iterator &other) const {
      return line_ == other.line_;
    }
    bool operator!=(const iterator &other) const { return !(*this == other); }

  private:
    Btor2Parser *parser_;
    Btor2LineIterator it_;
    Btor2Line *line_;
  };

  iterator begin() const { return iterator(parser); }
  iterator end() const { return iterator(); }

private:
  Btor2Parser *parser;
};

std::ostream &operator<<(std::ostream &os, const Btor2Line &line);
std::ostream &operator<<(std::ostream &os, const Btor2 &circuit);

static auto filter(const Btor2Tag tag) {
  return std::views::filter([tag](const Btor2Line &l) { return l.tag == tag; });
}
static auto states = filter(BTOR2_TAG_state);
static auto inputs = filter(BTOR2_TAG_input);

static auto inits = states | std::views::transform([](const auto &l) {
                      return std::pair{l.id, l.init};
                    }) |
                    std::views::filter([](const auto &p) { return p.second; });

static auto nexts = states | std::views::transform([](const auto &l) {
                      return std::pair{l.id, l.next};
                    }) |
                    std::views::filter([](const auto &p) { return p.second; });
