#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <ctime>
#include "RBset_for_count.hpp"

using std::set, std::vector, std::cout, std::endl;

int main() {
    const int N = 100000;       // 插入、删除、迭代器测试的数据量
    const int M = 10000;        // range 测试的查询次数（减少到 1 万次）
    std::mt19937_64 rng(20250511);

    // 生成随机数据
    vector<long long> keys(N);
    for (auto& x : keys) x = (long long)rng();
    vector<long long> del_keys = keys;
    shuffle(del_keys.begin(), del_keys.end(), rng);

    cout << "=== Insertion Benchmark (" << N << " ops) ===" << endl;

    // RBTree 插入
    {
        ESet<long long> avl;
        clock_t t0 = clock();
        for (auto x : keys) avl.emplace(x);
        clock_t t1 = clock();
        cout << "RBTree emplace: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
        cout << "RBTree rotate count: " << avl.getRotateCount() << endl;
        cout << "RBTree color fix count: " << avl.getColorFixCount() << endl;
    }

    // std::set 插入
    {
        set<long long> ss;
        clock_t t0 = clock();
        for (auto x : keys) ss.emplace(x);
        clock_t t1 = clock();
        cout << "std::set emplace: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    cout << endl << "=== Deletion Benchmark (" << N << " ops) ===" << endl;

    // RBTree 删除
    {
        ESet<long long> avl;
        for (auto x : keys) avl.emplace(x);
        avl.resetCounters(); // 重置计数器
        clock_t t0 = clock();
        for (auto x : del_keys) avl.erase(x);
        clock_t t1 = clock();
        cout << "RBTree erase: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
        cout << "RBTree rotate count: " << avl.getRotateCount() << endl;
        cout << "RBTree color fix count: " << avl.getColorFixCount() << endl;
    }

    // std::set 删除
    {
        set<long long> ss;
        for (auto x : keys) ss.emplace(x);
        clock_t t0 = clock();
        for (auto x : del_keys) ss.erase(x);
        clock_t t1 = clock();
        cout << "std::set erase: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    cout << endl << "=== Range Query Benchmark (" << M << " ops) ===" << endl;

    vector<std::pair<long long, long long>> ranges(M);
    for (auto& p : ranges) {
        long long a = (long long)rng();
        long long b = (long long)rng();
        if (a > b) std::swap(a, b);
        p = {a, b};
    }

    // RBTree range查询
    {
        ESet<long long> avl;
        for (auto x : keys) avl.emplace(x);
        clock_t t0 = clock();
        size_t total = 0;
        for (const auto& p : ranges) total += avl.range(p.first, p.second);
        clock_t t1 = clock();
        cout << "RBTree range: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    // std::set range查询（模拟）
    {
        set<long long> ss;
        for (auto x : keys) ss.emplace(x);
        clock_t t0 = clock();
        size_t total = 0;
        for (const auto& p : ranges) {
            if (p.first > p.second) continue;
            auto it_l = ss.lower_bound(p.first);
            auto it_r = ss.upper_bound(p.second);
            total += std::distance(it_l, it_r);
        }
        clock_t t1 = clock();
        cout << "std::set range (simulated): " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    cout << endl << "=== Iterator Benchmark (" << N << " ops) ===" << endl;

    // RBTree ++it
    {
        ESet<long long> avl;
        for (auto x : keys) avl.emplace(x);
        auto it = avl.begin();
        clock_t t0 = clock();
        for (int i = 0; i < N; ++i) {
            ++it;
            if (it == avl.end()) it = avl.begin();
        }
        clock_t t1 = clock();
        cout << "RBTree ++it: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    // std::set ++it
    {
        set<long long> ss;
        for (auto x : keys) ss.emplace(x);
        auto it = ss.begin();
        clock_t t0 = clock();
        for (int i = 0; i < N; ++i) {
            ++it;
            if (it == ss.end()) it = ss.begin();
        }
        clock_t t1 = clock();
        cout << "std::set ++it: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    // RBTree --it
    {
        ESet<long long> avl;
        for (auto x : keys) avl.emplace(x);
        auto it = avl.end();
        clock_t t0 = clock();
        for (int i = 0; i < N; ++i) {
            --it;
            if (it == avl.begin()) it = avl.end();
        }
        clock_t t1 = clock();
        cout << "RBTree --it: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    // std::set --it
    {
        set<long long> ss;
        for (auto x : keys) ss.emplace(x);
        auto it = ss.end();
        clock_t t0 = clock();
        for (int i = 0; i < N; ++i) {
            --it;
            if (it == ss.begin()) it = ss.end();
        }
        clock_t t1 = clock();
        cout << "std::set --it: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    cout << endl << "=== Assignment Operator Benchmark ===" << endl;

    // RBTree 赋值
    {
        ESet<long long> avl_large;
        for (auto x : keys) avl_large.emplace(x);
        clock_t t0 = clock();
        ESet<long long> avl_copy = avl_large;
        clock_t t1 = clock();
        cout << "RBTree operator=: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    // std::set 赋值
    {
        set<long long> ss_large;
        for (auto x : keys) ss_large.emplace(x);
        clock_t t0 = clock();
        set<long long> ss_copy = ss_large;
        clock_t t1 = clock();
        cout << "std::set operator=: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
    }

    return 0;
}