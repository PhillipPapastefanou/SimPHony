#include "soil_hydrology_richards.h"
#include "../../auxil/tridiagonal_solver.h"
#include <cmath>
#include <algorithm>

Soil_hydrology_richards::Soil_hydrology_richards(const Parameters& parameters, const Input& input,
                                                   const Config& config,
                                                   const Soil_water_module& curve_provider):
params(parameters), input(input), config(config), curve_provider(curve_provider) {
    nsoil = static_cast<int>(parameters.soil_layers.size());
}

void Soil_hydrology_richards::update_psi_k_cache() {
    psi_cache.resize(nsoil);
    k_cache.resize(nsoil);
    for (int i = 0; i < nsoil; ++i) {
        const Soil_layer& layer = params.soil_layers[i];
        psi_cache[i] = curve_provider.Psi_from_theta(theta[i], layer);
        k_cache[i] = curve_provider.K_from_theta(theta[i], layer);
    }
}

void Soil_hydrology_richards::Init(const std::vector<float>& theta0) {
    theta.assign(theta0.begin(), theta0.end());
    update_psi_k_cache();
}

void Soil_hydrology_richards::Get_state(std::vector<double>& psi_soil, std::vector<double>& k_soil,
                                          std::vector<float>& theta_out) const {
    psi_soil = psi_cache;
    k_soil = k_cache;
    theta_out.assign(theta.begin(), theta.end());
}

void Soil_hydrology_richards::Step(double precip_rate, const std::vector<double>& root_uptake, double dt) {

    const double RHO_WATER = params.constants.RHO_WATER;
    const double PI = params.constants.PI;

    // ----------------------------------------------------------------------------------
    // Diffusivity D(theta) = K(theta) * dpsi/dtheta, via a central-difference numerical
    // derivative of the existing psi(theta) pedotransfer function -- shared across Van
    // Genuchten and Campbell without needing a hand-derived closed form per curve type.
    // ----------------------------------------------------------------------------------
    std::vector<double> K(nsoil), D(nsoil);
    for (int i = 0; i < nsoil; ++i) {
        const Soil_layer& layer = params.soil_layers[i];
        K[i] = curve_provider.K_from_theta(theta[i], layer);

        const double eps = 1.0e-6;
        const double psi_plus = curve_provider.Psi_from_theta(theta[i] + eps, layer);
        const double psi_minus = curve_provider.Psi_from_theta(theta[i] - eps, layer);
        const double dpsi_dtheta = (psi_plus - psi_minus) / (2.0 * eps);

        D[i] = std::max(0.0, K[i] * dpsi_dtheta);
    }

    // Arithmetic-mean interface values (JSBACH's "Arithmetic_" interpolation option).
    // K_inter[i]/D_inter[i] is the value at the interface ABOVE layer i (between layer i-1
    // and layer i); index 0 is unused (top layer has no layer above it).
    std::vector<double> K_inter(nsoil, 0.0), D_inter(nsoil, 0.0);
    // dz_half[i]: distance between the midpoints of layer i-1 and layer i; index 0 unused.
    std::vector<double> dz_half(nsoil, 0.0);
    for (int i = 1; i < nsoil; ++i) {
        K_inter[i] = 0.5 * (K[i - 1] + K[i]);
        D_inter[i] = 0.5 * (D[i - 1] + D[i]);
        dz_half[i] = 0.5 * (params.soil_layers[i - 1].depth + params.soil_layers[i].depth);
    }

    // ----------------------------------------------------------------------------------
    // Infiltration / runoff partition (JSBACH "Uniform_" scale scheme, no ice impedance,
    // no ARNO subgrid variability): infiltration capped by the top layer's SATURATED
    // conductivity (a fixed soil property, params.soil_layers[0].k_soil_sat -- matching
    // JSBACH's hyd_cond_sat_sl1, an input parameter, not the theta-dependent K used for
    // vertical transport between layers). Using the unsaturated K[0] here instead would be
    // wrong: K(theta) collapses toward zero as the topsoil dries out, so the infiltration
    // cap would shrink right when rain is most needed to rewet a dry profile, regardless of
    // how high k_soil_sat itself is set. Reduced by a site-level steepness parameter;
    // everything above the cap is lost as runoff.
    // ----------------------------------------------------------------------------------
    const double precip_m_s = precip_rate / RHO_WATER;
    const double k_sat_top = params.soil_layers[0].k_soil_sat;
    const double infil_cap = std::max(0.0, k_sat_top * (1.0 - std::sin(params.surface_runoff_steepness * PI / 2.0)));
    const double infilt_m_s = std::min(precip_m_s, infil_cap);
    const double runoff_m_s = precip_m_s - infilt_m_s;

    // Root water uptake sink term, converted from mol H2O m-2 s-1 to an equivalent
    // depth-rate [m s-1] per layer.
    std::vector<double> sink(nsoil, 0.0);
    for (int i = 0; i < nsoil && i < static_cast<int>(root_uptake.size()); ++i) {
        sink[i] = root_uptake[i] / (params.constants.KG_H2O_To_Mol * RHO_WATER);
    }

    // Free-drainage lower boundary, capped by max_drainage_conductivity (analogous to
    // JSBACH4's k_brock): a restrictive layer/shallow water table can limit outflow below
    // what the bottom layer's own conductivity would otherwise allow. Uncapped by default
    // (max_drainage_conductivity == +inf).
    const double k_bottom_drain = std::min(K[nsoil - 1], params.max_drainage_conductivity);

    // ----------------------------------------------------------------------------------
    // Build and solve the implicit tridiagonal system for theta^{n+1} (mixed-form Richards
    // equation, coefficients K/D frozen at start-of-step theta^n -- see the plan for the
    // full derivation of these coefficients).
    // ----------------------------------------------------------------------------------
    std::vector<double> sub(nsoil, 0.0), diag(nsoil, 0.0), super(nsoil, 0.0), rhs(nsoil, 0.0);

    for (int i = 0; i < nsoil; ++i) {
        const double dz = params.soil_layers[i].depth;

        diag[i] = dz / dt;
        rhs[i] = dz / dt * theta[i] - sink[i];

        if (i == 0) {
            rhs[i] += infilt_m_s;
        } else {
            sub[i] = -D_inter[i] / dz_half[i];
            diag[i] += D_inter[i] / dz_half[i];
            rhs[i] += K_inter[i];
        }

        if (i == nsoil - 1) {
            rhs[i] -= k_bottom_drain;
        } else {
            super[i] = -D_inter[i + 1] / dz_half[i + 1];
            diag[i] += D_inter[i + 1] / dz_half[i + 1];
            rhs[i] -= K_inter[i + 1];
        }
    }

    std::vector<double> theta_new;
    solve_tridiagonal(sub, diag, super, rhs, theta_new);

    // ----------------------------------------------------------------------------------
    // Clip to the physical range. Saturation-excess anywhere in the profile is routed to
    // "drainage" (rather than cascaded back upward layer-by-layer) -- a deliberate
    // simplification for this single-column model; mass is still conserved (counted as an
    // additional outflow), just not necessarily attributed to the exact layer it overflowed
    // from. Under-shoot below a small floor (e.g. root uptake exceeding available water) is
    // simply clamped; the small resulting mass-balance error is accepted at this scope.
    // ----------------------------------------------------------------------------------
    double drainage_m_s = k_bottom_drain;
    const double theta_floor = 1.0e-4;
    for (int i = 0; i < nsoil; ++i) {
        const double theta_s = params.soil_layers[i].theta_s;
        if (theta_new[i] > theta_s) {
            const double excess_depth = (theta_new[i] - theta_s) * params.soil_layers[i].depth;
            drainage_m_s += excess_depth / dt;
            theta_new[i] = theta_s;
        } else if (theta_new[i] < theta_floor) {
            theta_new[i] = theta_floor;
        }
    }

    theta = theta_new;
    update_psi_k_cache();

    infiltration_kg_m2_s1 = infilt_m_s * RHO_WATER;
    runoff_kg_m2_s1 = runoff_m_s * RHO_WATER;
    drainage_kg_m2_s1 = drainage_m_s * RHO_WATER;
}
