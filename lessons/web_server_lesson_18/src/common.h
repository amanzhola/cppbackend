#pragma once

#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio.hpp>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

namespace net = boost::asio;
namespace sys = boost::system;

template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);

    std::vector<std::jthread> workers;
    workers.reserve(n - 1);

    while (--n) {
        workers.emplace_back(fn);
    }

    fn();
}
