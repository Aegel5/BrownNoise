namespace Rand {
    std::mt19937 m_rng{ std::random_device{}() };        // Быстрый генератор PRNG
    auto rand_real(auto a, auto b) {
        std::uniform_real_distribution<decltype(a)> d(a, b);
        return d(m_rng);
    }
    auto rand_i(auto a, auto b) {
        std::uniform_int_distribution<decltype(a)> d(a, b);
        return d(m_rng);
    }
    bool rand_bool() {
        return m_rng() & 1;
    }
}