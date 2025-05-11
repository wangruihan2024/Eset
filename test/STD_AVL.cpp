#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <ctime>
#include "AVLset.hpp"

using std::set, std::vector, std::cout, std::endl;

int main() {
    const int N = 100000;
    // 随机数生成
    std::mt19937_64 rng(20250511);

    // 生成随机数据
    vector<long long> keys(N);
    for (auto& x : keys) x = (long long)rng();
    // 原序列打乱作为删除序列
    vector<long long> del_keys = keys;
    shuffle(del_keys.begin(), del_keys.end(), rng);

    cout << "=== Insertion Benchmark (" << N << " ops) ===" << endl;

    // AVLTree 插入
    {
        ESet<long long> avl;
        clock_t t0 = clock();
        for (auto x : keys) avl.emplace(x);
        clock_t t1 = clock();
        cout << "AVLTree emplace: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
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

    // AVLTree 删除
    {
        ESet<long long> avl;
        for (auto x : keys) avl.emplace(x);
        clock_t t0 = clock();
        for (auto x : del_keys) avl.erase(x);
        clock_t t1 = clock();
        cout << "AVLTree erase: " << double(t1 - t0) / CLOCKS_PER_SEC << " s" << endl;
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

    return 0;
}