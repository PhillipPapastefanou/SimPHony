//
// Thomas algorithm for a single tridiagonal linear system.
//
#pragma once
#include <vector>

// Solves A x = d for a tridiagonal matrix A defined by:
//   sub[i]   = A(i, i-1)   (sub[0] is unused)
//   diag[i]  = A(i, i)
//   super[i] = A(i, i+1)   (super[n-1] is unused)
// All vectors (sub, diag, super, d) must have the same length n; x is
// resized to n and filled with the solution.
void solve_tridiagonal(const std::vector<double>& sub,
                        const std::vector<double>& diag,
                        const std::vector<double>& super,
                        const std::vector<double>& d,
                        std::vector<double>& x);
