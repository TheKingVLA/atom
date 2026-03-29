#include "orbital.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {

/**
 * @brief Nothing special, just factorial!
 * @param n The number for which to compute the factorial.
 * @return The factorial of n.
 */
double factorial(int n) {
    if (n < 0) return 0.0;
    double factorial_result = 1.0;
    for (int i = 2; i <= n; ++i) factorial_result *= i;
    return factorial_result;
}

/**
 * @brief Computes the associated Laguerre polynomial L_k^alpha(x).
 * @param k The degree of the polynomial.
 * @param alpha The parameter for the associated polynomial.
 * @param x The argument of the polynomial.
 * @return The value of L_k^alpha(x).
 */
double assocLaguerre(int k, int alpha, double x) {
    // L_k^alpha(x)
    if (k == 0) return 1.0;
    if (k == 1) return 1.0 + alpha - x;

    double laguerre_prev2   = 1.0;
    double laguerre_prev1   = 1.0 + alpha - x;
    double laguerre_current = 0.0;

    for (int n = 2; n <= k; ++n) {
        laguerre_current = ((2.0 * n - 1.0 + alpha - x) * laguerre_prev1 - (n - 1.0 + alpha) * laguerre_prev2) / n;
        laguerre_prev2   = laguerre_prev1;
        laguerre_prev1   = laguerre_current;
    }
    return laguerre_current;
}

/**
 * @brief Computes the associated Legendre polynomial P_l^m(x) for m >= 0.
 * @param l The degree of the polynomial.
 * @param m The order of the polynomial (must be non-negative).
 * @param x The argument of the polynomial (must be in the range [-1, 1]).
 * @return The value of P_l^m(x).
 */
double assocLegendrePositiveM(int l, int m, double x) {
    // Computes P_l^m(x) for m >= 0
    if (m < 0 || m > l) return 0.0;

    double legendre_initial = 1.0;
    if (m > 0) {
        double sqrt_one_minus_x_squared = std::sqrt(std::max(0.0, 1.0 - x * x));
        double factorial_value          = 1.0;
        for (int i = 1; i <= m; ++i) {
            legendre_initial *= -(factorial_value) * sqrt_one_minus_x_squared;
            factorial_value  += 2.0;
        }
    }

    if (l == m) return legendre_initial;

    double legendre_next = x * (2.0 * m + 1.0) * legendre_initial;
    if (l == m + 1) return legendre_next;

    double legendre_final = 0.0;
    double legendre_prev2 = legendre_initial;
    double legendre_prev1 = legendre_next;

    for (int ll = m + 2; ll <= l; ++ll) {
        legendre_final = ((2.0 * ll - 1.0) * x * legendre_prev1 - (ll + m - 1.0) * legendre_prev2) / (ll - m);
        legendre_prev2 = legendre_prev1;
        legendre_prev1 = legendre_final;
    }

    return legendre_final;
}

/**
 * @brief Computes the radial part of the hydrogen wavefunction R_nl(r).
 * @param n The principal quantum number (n >= 1).
 * @param l The azimuthal quantum number (0 <= l < n).
 * @param r The radial distance from the nucleus (r >= 0).
 * @param a0 The Bohr radius (a0 > 0).
 * @return The value of R_nl(r).
 */
double hydrogenRadial(int n, int l, double r, double a0) {
    // R_nl(r)
    if (n < 1 || l < 0 || l >= n || r < 0.0 || a0 <= 0.0) return 0.0;

    double normalized_radius = 2.0 * r / (n * a0);
    int radial_index         = n - l - 1;
    int laguerre_parameter   = 2 * l + 1;

    double normalization_factor = std::sqrt(
        std::pow(2.0 / (n * a0), 3.0) *
        factorial(radial_index) /
        (2.0 * n * factorial(n + l))
    );

    double laguerre_value = assocLaguerre(radial_index, laguerre_parameter, normalized_radius);
    return normalization_factor * std::exp(-normalized_radius / 2.0) * std::pow(normalized_radius, l) * laguerre_value;
}

struct SampleTable {
    std::vector<float> grid_values;
    std::vector<float> cumulative_distribution;
};


/**
 * @brief Builds a cumulative distribution function (CDF) from a probability density function.
 * @param xGrid The grid of x-values.
 * @param pdf The probability density function (PDF) values at the grid points.
 * @return The cumulative distribution function.
 */
SampleTable buildCDF(const std::vector<float>& grid, const std::vector<float>& pdf) {
    SampleTable table;
    table.grid_values = grid;
    table.cumulative_distribution.resize(pdf.size());

    float total_probability = std::accumulate(pdf.begin(), pdf.end(), 0.0f);
    if (total_probability <= 0.0f) {
        throw std::runtime_error("PDF normalization failed.");
    }

    float cumulative_sum = 0.0f;
    for (size_t i = 0; i < pdf.size(); ++i) {
        cumulative_sum += pdf[i] / total_probability;
        table.cumulative_distribution[i] = cumulative_sum;
    }
    table.cumulative_distribution.back() = 1.0f;
    return table;
}

/**
 * @brief Samples a value from a cumulative distribution function.
 * @param table The cumulative distribution function table.
 * @param rng The random number generator.
 * @return The sampled value.
 */
float sampleFromCDF(const SampleTable& table, std::mt19937& rng) {
    std::uniform_real_distribution<float> uniform_distribution(0.0f, 1.0f);
    float random_value = uniform_distribution(rng);

    auto cdf_iterator = std::lower_bound(table.cumulative_distribution.begin(), table.cumulative_distribution.end(), random_value);
    int index         = std::clamp(static_cast<int>(cdf_iterator - table.cumulative_distribution.begin()), 1, static_cast<int>(table.cumulative_distribution.size()) - 1);

    float cdf_lower  = table.cumulative_distribution[index - 1];
    float cdf_upper  = table.cumulative_distribution[index];
    float grid_lower = table.grid_values[index - 1];
    float grid_upper = table.grid_values[index];

    float interpolation_factor = (random_value - cdf_lower) / (cdf_upper - cdf_lower + 1e-12f);
    return grid_lower + interpolation_factor * (grid_upper - grid_lower);
}

/**
 * @brief Builds the radial cumulative distribution function for a hydrogen-like atom.
 * @param n The principal quantum number.
 * @param l The azimuthal quantum number.
 * @param a0 The Bohr radius.
 * @return The radial CDF.
 */
SampleTable buildRadialCDF(int n, int l, float a0) {
    const int radial_grid_size = 5000;
    const float max_radius     = 12.0f * n * n * a0;

    std::vector<float> radial_grid(radial_grid_size);
    std::vector<float> probability_density(radial_grid_size);

    for (int i = 0; i < radial_grid_size; ++i) {
        float radius   = max_radius * static_cast<float>(i) / static_cast<float>(radial_grid_size - 1);
        radial_grid[i] = radius;

        double radial_wavefunction = hydrogenRadial(n, l, radius, a0);
        double radial_probability  = radius * radius * radial_wavefunction * radial_wavefunction;  // p_r(r) ∝ r^2 |R_nl(r)|^2
        probability_density[i]     = static_cast<float>(radial_probability);
    }

    return buildCDF(radial_grid, probability_density);
}

/**
 * @brief Builds the theta cumulative distribution function for a hydrogen-like atom.
 * @param l The azimuthal quantum number.
 * @param m The magnetic quantum number.
 * @return The theta CDF.
 */
SampleTable buildThetaCDF(int l, int m) {
    const int theta_grid_size = 3000;
    int magnetic_quantum_number_abs = std::abs(m);

    std::vector<float> theta_grid(theta_grid_size);
    std::vector<float> probability_density(theta_grid_size);

    for (int i = 0; i < theta_grid_size; ++i) {
        float theta   = static_cast<float>(M_PI) * static_cast<float>(i) / static_cast<float>(theta_grid_size - 1);
        theta_grid[i] = theta;

        double cos_theta           = std::cos(theta);
        double legendre_polynomial = assocLegendrePositiveM(l, magnetic_quantum_number_abs, cos_theta);
        double theta_probability   = std::sin(theta) * legendre_polynomial * legendre_polynomial;   // p_theta(theta) ∝ sin(theta) [P_l^m(cos theta)]^2
        probability_density[i]     = static_cast<float>(std::max(0.0, theta_probability));
    }

    return buildCDF(theta_grid, probability_density);
}

} // namespace

OrbitalCloud::OrbitalCloud(const Vec3& c) : electron_color(c) {}

void OrbitalCloud::generateHydrogenState(int n, int l, int m, int numSamples, float a0) {
    if (n < 1)              throw std::invalid_argument("n must be >= 1");
    if (l < 0 || l >= n)    throw std::invalid_argument("l must satisfy 0 <= l < n");
    if (std::abs(m) > l)    throw std::invalid_argument("|m| must be <= l");
    if (numSamples <= 0)    throw std::invalid_argument("numSamples must be > 0");
    if (a0 <= 0.0f)         throw std::invalid_argument("a0 must be > 0");

    electron_positions.clear();
    electron_positions.reserve(numSamples);

    SampleTable radial_cdf = buildRadialCDF(n, l, a0);
    SampleTable theta_cdf  = buildThetaCDF(l, m);

    std::random_device random_device;
    std::mt19937 random_generator(random_device());
    std::uniform_real_distribution<float> uniform_distribution(0.0f, 1.0f);

    for (int i = 0; i < numSamples; ++i) {
        float sampled_radius = sampleFromCDF(radial_cdf, random_generator);
        float sampled_theta  = sampleFromCDF(theta_cdf, random_generator);
        float sampled_phi    = 2.0f * static_cast<float>(M_PI) * uniform_distribution(random_generator);   // p_phi(phi) is uniform in [0, 2pi)

        Vec3 position {
            sampled_radius * std::sin(sampled_theta) * std::cos(sampled_phi),
            sampled_radius * std::sin(sampled_theta) * std::sin(sampled_phi),
            sampled_radius * std::cos(sampled_theta)
        };

        electron_positions.push_back(position);
    }
}

void OrbitalCloud::draw(float point_size) const {
    glDisable(GL_LIGHTING);
    glPointSize(point_size);
    glColor3f(electron_color.x, electron_color.y, electron_color.z);

    glBegin(GL_POINTS);
    for (const auto& position : electron_positions) {
        glVertex3f(position.x, position.y, position.z);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}