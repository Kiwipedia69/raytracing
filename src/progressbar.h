#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <mutex>

class ProgressBar {
public:
    explicit ProgressBar(size_t max,
                         std::string message = {},
                         size_t width = 50,
                         std::chrono::milliseconds min_refresh = std::chrono::milliseconds{33})
        : _max(std::max<size_t>(1, max))
        , _counter(0)
        , _width(std::max<size_t>(1, width))   // _width avant _message
        , _filled(0)
        , _message(std::move(message))
        , _bar(_width, ' ')
        , _min_refresh(min_refresh)
        , _last_draw(clock::now())
    {
        std::lock_guard<std::mutex> lock(_mtx);
        redraw(true);
    }

    ~ProgressBar() {
        std::lock_guard<std::mutex> lock(_mtx);
        std::cout << '\n';
        std::cout.flush();
    }

    // Incrémente (thread-safe)
    void tick(size_t n = 1) {
        std::lock_guard<std::mutex> lock(_mtx);
        _counter = std::min(_counter + n, _max);
        maybe_redraw_unlocked();
    }

    // Opérateurs ++
    void operator++()    { tick(1); }
    void operator++(int) { tick(1); }

    // Définit la valeur courante (thread-safe)
    void set(size_t value) {
        std::lock_guard<std::mutex> lock(_mtx);
        _counter = std::min(value, _max);
        maybe_redraw_unlocked();
    }

    // ---- Compat: permet d'utiliser pb.update(x) dans main.cpp
    void update(size_t value) { set(value); }
    // ---- Fin compat

    void finish() { set(_max); }

private:
    using clock = std::chrono::steady_clock;

    void maybe_redraw_unlocked() {
        size_t filled = (_counter * _width) / _max;
        auto now = clock::now();

        if (filled != _filled || now - _last_draw >= _min_refresh || _counter == _max) {
            _filled = filled;
            redraw(false);
            _last_draw = now;
        }
    }

    void redraw(bool force) {
        // le mutex doit déjà être verrouillé
        std::fill(_bar.begin(), _bar.end(), ' ');
        std::fill_n(_bar.begin(), _filled, '#');

        unsigned percent = static_cast<unsigned>((_counter * 100) / _max);

        std::cout << '\r'
                  << _message
                  << "\x1b[1;35m  [\x1b[0m"
                  << "\x1b[1;36m" << _bar << "\x1b[0m"
                  << "\x1b[1;35m] \x1b[0m"
                  << "\x1b[1;33m" << percent << "%\x1b[0m";

        if (force || _counter == _max)
            std::cout.flush();
    }

    // état
    size_t _max{1};
    size_t _counter{0};
    size_t _width{50};
    size_t _filled{0};
    std::string _message;
    std::string _bar;

    // rafraîchissement
    std::chrono::milliseconds _min_refresh{33};
    clock::time_point _last_draw{};

    // synchro
    mutable std::mutex _mtx;
};
