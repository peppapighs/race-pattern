#pragma once

template <class T> class scope_guard {
  private:
    T f;

  public:
    scope_guard(T &&f) : f(f) {}

    ~scope_guard() { f(); }
};