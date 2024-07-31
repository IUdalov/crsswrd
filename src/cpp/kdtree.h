#pragma once

#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <cassert>

#ifndef DEBUG_KDTREE
#   define DEBUG_KDTREE 0
#endif

namespace {

template<typename T1, typename T2>
void assertAllValuesSame(const T1& v1, const T2& v2) {
    assert(v1.size() == v2.size());
    std::set<typename T1::value_type> s1;
    for(const auto& i1: v1)
        s1.insert(i1);
    for(const auto& i2: v2)
        assert(s1.contains(i2));
}

bool matchPattern(const std::string& pattern, const std::string& str) {
    for(size_t i = 0; i < pattern.size(); i++)
        if (pattern[i] != str[i] and pattern[i] != '#')
            return false;
    return true;
}

} // namespace

class KDTree {
public:
    explicit KDTree(const std::vector<std::string>& words)
            : len_(words[0].size())
            , data_(words) {
        for(const auto& s: data_)
            assert(len_ == s.size());
        index_.resize(len_);
        buildIndex(0, 0, data_.size());
    }

    size_t size() const {
        return data_.size();
    }

    std::string pretty() const {
        std::stringstream ss;
        ss << "Index:" << std::endl;
        for(size_t pos = 0; pos < len_; pos++) {
            ss << "  " << pos << ":" << std::endl;
            for(const auto& kv: index_[pos])
                ss << "    ch: " << kv.second << ", index: " <<  kv.first << ", word: " << data_[kv.first] << std::endl;
        }
        return ss.str();
    }

    std::vector<std::string> lookup(const std::string& pattern) const {
        if (DEBUG_KDTREE) {
            return lookupStrict(pattern);
        }

        assert(pattern.size() == len_);
        std::vector<std::string> result;
        lookupImpl(pattern, 0, 0, data_.size(), 0, result);
        return result;
    }

private:
    size_t len_;
    std::vector<std::string> data_;
    std::vector<std::vector<std::pair<size_t, char>>> index_;

    void lookupImpl(const std::string& pattern, size_t pos, size_t start, size_t fin, size_t indexOffset, std::vector<std::string>& result) const {
        if (pos + 1 == len_) {
            for(; start < fin; start++)
                if (matchPattern(pattern, data_[start]))
                    result.push_back(data_[start]);
            return;
        }
        auto [medIdx, medSym] = index_[pos][indexOffset];
        if (pattern[pos] == '#') {
            lookupImpl(pattern, pos + 1, start, medIdx, indexOffset * 2, result);
            lookupImpl(pattern, pos + 1, medIdx, fin, indexOffset * 2 + 1, result);
            return;
        }

        if (pattern[pos] < medSym)
            lookupImpl(pattern, pos + 1, start, medIdx, indexOffset * 2, result);
        else
            lookupImpl(pattern, pos + 1, medIdx, fin, indexOffset * 2 + 1, result);
    }

    std::vector<std::string> lookupStrict(const std::string& pattern) const {
        auto res = lookup(pattern);
        assertAllValuesSame(res, lookupDummy(pattern));
        return res;
    }

    std::vector<std::string> lookupDummy(const std::string& pattern) const {
        assert(pattern.size() == len_);
        std::vector<std::string> result;
        for(const auto& str : data_)
            if (matchPattern(pattern, str))
                result.push_back(str);
        return result;
    }

private:
    void buildIndex(size_t pos, size_t start, size_t fin) {
        if (pos >= len_)
            return;

        sort(std::begin(data_) + start, std::begin(data_) + fin, [pos](const std::string& lhs, const std::string& rhs) -> bool {
            return lhs[pos] < rhs[pos];
        });

        char medSym = data_[(start + fin) / 2][pos];
        size_t medPos = start;
        for(;medPos < fin; medPos++)
            if (data_[medPos][pos] == medSym)
                break;

        index_[pos].push_back({medPos, medSym});
        buildIndex(pos + 1, start, medPos);
        buildIndex(pos + 1, medPos, fin);
    }
};

class KDDict {
public:
    explicit KDDict(const std::string& path, size_t maxLen = 20) {
        std::map<size_t, std::vector<std::string>> groupedByLen;
        for(const auto& s: readLines(path)) {
            groupedByLen[s.size()].push_back(s);
        }

        for(const auto& kv: groupedByLen)
            if (kv.first <= maxLen)
                trees_.emplace(kv.first, kv.second);
    }

    std::vector<std::string> lookup(const std::string& pattern) const {
        auto it = trees_.find(pattern.size());
        if (it == std::end(trees_))
            return {};
        return it->second.lookup(pattern);
    }

    std::string pretty(bool verbose = false) const {
        std::stringstream ss;
        for(const auto& kv: trees_) {
            ss << "Word len: " << kv.first << ", words: " << kv.second.size() << std::endl;
            if (verbose)
                ss << kv.second.pretty() << std::endl;
        }
        ss << "Total words: " << size() << std::endl;
        return ss.str();
    }

    size_t size() const {
        size_t sum = 0;
        for(const auto& kv: trees_)
            sum += kv.second.size();
        return sum;
    }


private:
    std::map<size_t, KDTree> trees_;

    std::vector<std::string> readLines(const std::string& path) {
        std::ifstream in(path);
        if (!in.good())
            throw std::runtime_error("Unable to read dict from file");
        std::string buf;
        std::vector<std::string> result;
        while(getline(in, buf))
            result.push_back(buf);
        return result;
    }
};
