#include <boost/range/istream_range.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <iostream>
#include <unordered_map>

using namespace boost;
using namespace boost::adaptors;

template <typename C, typename T = typename C::value_type>
std::unordered_map<T, unsigned int> count_occurences(const C &collection) {
  std::unordered_map<T, unsigned int> counts{};
  counts.reserve(collection.size() * 5 / 3);
  for_each(collection, [&counts](const T& elem) {
    unsigned int new_count = 1;
    auto search = counts.find(elem);
    if (search != counts.end()) {
      const auto current_count = search->second;
      new_count += current_count;
    }
    counts[elem] = new_count;
  });
  return counts;
}

template <typename C,
          typename T = typename C::value_type,
          typename CIter = typename C::const_iterator,
          typename Iter = typename C::iterator>
class collection_append {
private: C &collection;
public:
  using value_type = T;
  using const_iterator = CIter;
  using iterator = Iter;
  explicit collection_append(C &a_collection) : collection(a_collection) {}
  collection_append() = delete;
  collection_append(const collection_append&) = delete;
  collection_append(collection_append&&) = delete;
  ~collection_append() = default;
  collection_append&operator =(const collection_append&) = delete;
  collection_append&operator =(collection_append&&) = delete;

  collection_append& append(T &&item) {
    this->collection.emplace(this->collection.end(), std::move(item));
    return *this;
  }

  template <typename _InputIterator, typename = std::_RequireInputIter<_InputIterator>>
  void insert(iterator __position, _InputIterator __first, _InputIterator __last) {
    this->collection.insert(__position, __first, __last);
  }

  constexpr inline iterator begin() const noexcept {
    return this->collection.begin();
  }

  constexpr inline iterator end() const noexcept {
    return this->collection.end();
  }
};

template <typename C>
void print_collection(const C& collection) {
  for(const auto &elem : collection) {
    printf("%d: %s\n", elem.first, elem.second.c_str());
  }
}


int main() {
  auto const is_alpha_word = [](const std::string &word) {
    for(const char c : word) {
      if (std::isalpha(c) == 0) { return false; }
    }
    return true;
  };
  auto const to_lower_case = [](const std::string &word) {
    std::string word_copy{word};
    std::transform(word_copy.begin(), word_copy.end(), word_copy.begin(), ::tolower);
    return word_copy;
  };
  auto const flip_pair = [](const std::pair<std::string, unsigned int> &entry) {
    return std::make_pair(entry.second, entry.first);
  };
  using count_pair_t = std::pair<unsigned int, std::string>;
  auto const compare_counts = [](const count_pair_t &x, const count_pair_t &y) {
    return x.first > y.first;
  };
  auto const compare_words = [](const count_pair_t &x, const count_pair_t &y) {
    return x.second.compare(y.second) < 0;
  };
  auto const found_pred = [](const count_pair_t &arg1, const count_pair_t &arg2) { return arg1.first == arg2.first; };



  std::vector<std::string> words;
  words.reserve(64 * 1024);
  collection_append<decltype(words)> add_words{words};
  push_back(add_words,
            istream_range<std::string>(std::cin)
            | filtered(is_alpha_word)
            | transformed(to_lower_case)
  );

  auto counts_map = count_occurences(words);

  std::vector<count_pair_t> count_pairs;
  count_pairs.reserve(counts_map.size());
  std::set<unsigned int> just_counts;
  auto const collect_just_counts = [&just_counts](const count_pair_t &item) { just_counts.emplace(item.first); return true; };
  sort(push_back(count_pairs, counts_map | transformed(flip_pair) | filtered(collect_just_counts)), compare_counts);

  const auto just_counts_rng = adaptors::reverse(make_iterator_range(just_counts.begin(), just_counts.end()));

  std::array<count_pair_t, 1> the_search_item{ count_pair_t{0, std::string{}} };
  auto current = count_pairs.begin();
  const auto very_end = count_pairs.end();
  int check_count = 0, sub_rng_count = 0;
  for(const unsigned int n : just_counts_rng) {
    check_count++;
    the_search_item[0].first = n;
    current = std::find_first_of(current, very_end, the_search_item.begin(), the_search_item.end(), found_pred);
    if (current != very_end) {
      const auto sub_rng_end = std::find_end(current, very_end, the_search_item.begin(), the_search_item.end(), found_pred);
      const auto sub_rng = make_iterator_range(current, sub_rng_end != very_end ? sub_rng_end + 1 : sub_rng_end);
      sort(sub_rng, compare_words);
      if (sub_rng_end != very_end) {
        current = sub_rng_end;
      }
      sub_rng_count++;
    } else {
      break;
    }
  }



  fputs("\nDEBUG: ", stderr);
  for(const unsigned int n : just_counts_rng) {
    fprintf(stderr, "%d, ", n);
  }
  putc('\n', stderr);

  fprintf(stderr, "\nDEBUG: set count: %lu, check count: %d, sub range count: %d\n\n",
          just_counts.size(), check_count, sub_rng_count);


  // main output
  print_collection(count_pairs);


  fputs("\nDEBUG:\n", stderr);
  auto rng2 = boost::unique(sort(words));
  copy(rng2, std::ostream_iterator<std::string>(std::cerr, "\n"));
}