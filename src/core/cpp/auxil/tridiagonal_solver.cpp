#include "tridiagonal_solver.h"

void solve_tridiagonal(const std::vector<double>& sub,
                        const std::vector<double>& diag,
                        const std::vector<double>& super,
                        const std::vector<double>& d,
                        std::vector<double>& x) {

    const int n = static_cast<int>(diag.size());
    x.assign(n, 0.0);
    if (n == 0) return;

    std::vector<double> c_prime(n, 0.0);
    std::vector<double> d_prime(n, 0.0);

    c_prime[0] = super[0] / diag[0];
    d_prime[0] = d[0] / diag[0];

    for (int i = 1; i < n; ++i) {
        const double denom = diag[i] - sub[i] * c_prime[i - 1];
        c_prime[i] = super[i] / denom;
        d_prime[i] = (d[i] - sub[i] * d_prime[i - 1]) / denom;
    }

    x[n - 1] = d_prime[n - 1];
    for (int i = n - 2; i >= 0; --i) {
        x[i] = d_prime[i] - c_prime[i] * x[i + 1];
    }
}
