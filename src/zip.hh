#include <vector>
#include <utility>

namespace nolang
{

template<typename T, typename Y>
class Zip
{
public:
    Zip(std::vector<T> a, std::vector<Y> b) { zip(a, b); }
    inline std::vector<std::pair<T, Y>> operator()() const { return items; }

private:
    inline void zip(std::vector<T> a, std::vector<Y> b) {
        size_t s = a.size();
        if (s > b.size()) s = b.size();
        for (size_t i = 0; i < s; ++i) {
            items.push_back(std::make_pair(a[i], b[i]));
        }
    }
    std::vector<std::pair<T, Y>> items;
};

template<typename T, typename Y>
typename std::vector<std::pair<T, Y>> zip(const std::vector<T> &a, const std::vector<Y> &b)
{
    return Zip<T, Y>(a, b)();
}

}
