#pragma once

#include <iostream>
#include <mutex>

struct SyncCout {
    static std::mutex mut;
    std::scoped_lock<std::mutex> lock{SyncCout::mut};

    template <typename T>
    friend const SyncCout &operator<<(const SyncCout &s, T &&v) {
        std::cout << std::forward<T>(v);
        return s;
    }

    friend const SyncCout &operator<<(const SyncCout &s,
                                      std::ostream &(*f)(std::ostream &)) {
        std::cout << f;
        return s;
    }
};

std::mutex SyncCout::mut;

struct SyncCerr {
    static std::mutex mut;
    std::scoped_lock<std::mutex> lock{SyncCerr::mut};

    template <typename T>
    friend const SyncCerr &operator<<(const SyncCerr &s, T &&v) {
        std::cerr << std::forward<T>(v);
        return s;
    }

    friend const SyncCerr &operator<<(const SyncCerr &s,
                                      std::ostream &(*f)(std::ostream &)) {
        std::cerr << f;
        return s;
    }
};

std::mutex SyncCerr::mut;
