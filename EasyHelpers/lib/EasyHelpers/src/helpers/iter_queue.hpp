#pragma once
#include <deque>
#include <queue>

namespace Helpers {
template <typename T, typename Container = std::deque<T> >
class iter_queue : public std::queue<T, Container> {
   public:
    using std::queue<T, Container>::queue;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;

    iterator begin() {
        return this->c.begin();
    }
    iterator end() {
        return this->c.end();
    }
    const_iterator begin() const {
        return this->c.begin();
    }
    const_iterator end() const {
        return this->c.end();
    }
    const_iterator cbegin() const {
        return this->c.cbegin();
    }
    const_iterator cend() const {
        return this->c.cend();
    }

    const_iterator find(const T& value) const {
        return std::find(this->cbegin(), this->cend(), value);
    }

    iter_queue() {
        this->c = std::deque<T>();
    }
    ~iter_queue() {
        this->c.clear();
    }
};
}  // namespace Helpers
