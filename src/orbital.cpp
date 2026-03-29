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
    double result = 1.0;
    for (int i = 2; i <= n; ++i) result *= i;
    return result;
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

    double Lkm2 = 1.0;
    double Lkm1 = 1.0 + alpha - x;
    double Lk = 0.0;

    for (int n = 2; n <= k; ++n) {
        Lk = ((2.0 * n - 1.0 + alpha - x) * Lkm1 - (n - 1.0 + alpha) * Lkm2) / n;
        Lkm2 = Lkm1;
        Lkm1 = Lk;
    }
    return Lk;
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

    double pmm = 1.0;
    if (m > 0) {
        double somx2 = std::sqrt(std::max(0.0, 1.0 - x * x));
        double fact = 1.0;
        for (int i = 1; i <= m; ++i) {
            pmm *= -(fact) * somx2;
            fact += 2.0;
        }
    }

    if (l == m) return pmm;

    double pmmp1 = x * (2.0 * m + 1.0) * pmm;
    if (l == m + 1) return pmmp1;

    double pll = 0.0;
    double p_lm2 = pmm;
    double p_lm1 = pmmp1;

    for (int ll = m + 2; ll <= l; ++ll) {
        pll = ((2.0 * ll - 1.0) * x * p_lm1 - (ll + m - 1.0) * p_lm2) / (ll - m);
        p_lm2 = p_lm1;
        p_lm1 = pll;
    }

    return p_lm1;
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

    double rho = 2.0 * r / (n * a0);
    int k = n - l - 1;
    int alpha = 2 * l + 1;

    double norm = std::sqrt(
        std::pow(2.0 / (n * a0), 3.0) *
        factorial(k) /
        (2.0 * n * factorial(n + l))
    );

    double lag = assocLaguerre(k, alpha, rho);
    return norm * std::exp(-rho / 2.0) * std::pow(rho, l) * lag;
}

struct SampleTable {
    std::vector<float> x;
    std::vector<float> cdf;
};


/**
 * @brief Builds a cumulative distribution function from a probability density function.
 * @param xGrid The grid of x-values.
 * @param pdf The probability density function values at the grid points.
 * @return The cumulative distribution function.
 */
SampleTable buildCDF(const std::vector<float>& grid, const std::vector<float>& pdf) {
    SampleTable table;
    table.x = grid;
    table.cdf.resize(pdf.size());

    float sum = std::accumulate(pdf.begin(), pdf.end(), 0.0f);
    if (sum <= 0.0f) {
        throw std::runtime_error("PDF normalization failed.");
    }

    float running = 0.0f;
    for (size_t i = 0; i < pdf.size(); ++i) {
        running += pdf[i] / sum;
        table.cdf[i] = running;
    }
    table.cdf.back() = 1.0f;
    return table;
}

/**
 * @brief Samples a value from a cumulative distribution function.
 * @param table The cumulative distribution function table.
 * @param rng The random number generator.
 * @return The sampled value.
 */
float sampleFromCDF(const SampleTable& table, std::mt19937& rng) {
    std::uniform_real_distribution<float> U(0.0f, 1.0f);
    float u = U(rng);

    auto it = std::lower_bound(table.cdf.begin(), table.cdf.end(), u);
    int i = std::clamp(static_cast<int>(it - table.cdf.begin()), 1, static_cast<int>(table.cdf.size()) - 1);

    float c0 = table.cdf[i - 1];
    float c1 = table.cdf[i];
    float x0 = table.x[i - 1];
    float x1 = table.x[i];

    float t = (u - c0) / (c1 - c0 + 1e-12f);
    return x0 + t * (x1 - x0);
}

/**
 * @brief Builds the radial cumulative distribution function for a hydrogen-like atom.
 * @param n The principal quantum number.
 * @param l The azimuthal quantum number.
 * @param a0 The Bohr radius.
 * @return The radial CDF.
 */
SampleTable buildRadialCDF(int n, int l, float a0) {
    const int Nr = 5000;
    const float rMax = 12.0f * n * n * a0;

    std::vector<float> rGrid(Nr);
    std::vector<float> pdf(Nr);

    for (int i = 0; i < Nr; ++i) {
        float r = rMax * static_cast<float>(i) / static_cast<float>(Nr - 1);
        rGrid[i] = r;

        double R = hydrogenRadial(n, l, r, a0);
        double pr = r * r * R * R;  // p_r(r) ∝ r^2 |R_nl(r)|^2
        pdf[i] = static_cast<float>(pr);
    }

    return buildCDF(rGrid, pdf);
}

/**
 * @brief Builds the theta cumulative distribution function for a hydrogen-like atom.
 * @param l The azimuthal quantum number.
 * @param m The magnetic quantum number.
 * @return The theta CDF.
 */
SampleTable buildThetaCDF(int l, int m) {
    const int Ntheta = 3000;
    int mAbs = std::abs(m);

    std::vector<float> thetaGrid(Ntheta);
    std::vector<float> pdf(Ntheta);

    for (int i = 0; i < Ntheta; ++i) {
        float theta = static_cast<float>(M_PI) * static_cast<float>(i) / static_cast<float>(Ntheta - 1);
        thetaGrid[i] = theta;

        double x = std::cos(theta);
        double P = assocLegendrePositiveM(l, mAbs, x);
        double ptheta = std::sin(theta) * P * P;   // p_theta(theta) ∝ sin(theta) [P_l^m(cos theta)]^2
        pdf[i] = static_cast<float>(std::max(0.0, ptheta));
    }

    return buildCDF(thetaGrid, pdf);
}

} // namespace

OrbitalCloud::OrbitalCloud(const Vec3& c) : color(c) {}

void OrbitalCloud::generateHydrogenState(int n, int l, int m, int numSamples, float a0) {
    if (n < 1) throw std::invalid_argument("n must be >= 1");
    if (l < 0 || l >= n) throw std::invalid_argument("l must satisfy 0 <= l < n");
    if (std::abs(m) > l) throw std::invalid_argument("|m| must be <= l");
    if (numSamples <= 0) throw std::invalid_argument("numSamples must be > 0");
    if (a0 <= 0.0f) throw std::invalid_argument("a0 must be > 0");

    points.clear();
    points.reserve(numSamples);

    SampleTable radialCDF = buildRadialCDF(n, l, a0);
    SampleTable thetaCDF  = buildThetaCDF(l, m);

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> U(0.0f, 1.0f);

    for (int i = 0; i < numSamples; ++i) {
        float r = sampleFromCDF(radialCDF, rng);
        float theta = sampleFromCDF(thetaCDF, rng);
        float phi = 2.0f * static_cast<float>(M_PI) * U(rng);   // p_phi(phi) is uniform in [0, 2pi)

        Vec3 p {
            r * std::sin(theta) * std::cos(phi),
            r * std::sin(theta) * std::sin(phi),
            r * std::cos(theta)
        };

        points.push_back(p);
    }
}

void OrbitalCloud::draw(float pointSize) const {
    glDisable(GL_LIGHTING);
    glPointSize(pointSize);
    glColor3f(color.x, color.y, color.z);

    glBegin(GL_POINTS);
    for (const auto& p : points) {
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}