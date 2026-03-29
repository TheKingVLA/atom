#include "orbital.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float sampleFromCDF(const std::vector<float>& x,
                           const std::vector<float>& cdf,
                           std::mt19937& rng) {
    std::uniform_real_distribution<float> U(0.0f, 1.0f);
    float u = U(rng);

    auto it = std::lower_bound(cdf.begin(), cdf.end(), u);
    int i = std::clamp(static_cast<int>(it - cdf.begin()), 1, static_cast<int>(cdf.size()) - 1);

    float c0 = cdf[i - 1];
    float c1 = cdf[i];
    float x0 = x[i - 1];
    float x1 = x[i];

    float t = (u - c0) / (c1 - c0 + 1e-12f);
    return x0 + t * (x1 - x0);
}

void OrbitalCloud::generate1s(int numSamples, float a0) {
    points.clear();
    points.reserve(numSamples);

    // Build radial PDF/CDF for p(r) ∝ r^2 exp(-2r/a0)
    const int Nr = 4000;
    const float rMax = 12.0f * a0;

    std::vector<float> rGrid(Nr);
    std::vector<float> pdf(Nr);
    std::vector<float> cdf(Nr);

    for (int i = 0; i < Nr; ++i) {
        float r = rMax * static_cast<float>(i) / static_cast<float>(Nr - 1);
        rGrid[i] = r;
        pdf[i] = r * r * std::exp(-2.0f * r / a0);
    }

    float sum = std::accumulate(pdf.begin(), pdf.end(), 0.0f);
    for (float& v : pdf) v /= sum;

    float running = 0.0f;
    for (int i = 0; i < Nr; ++i) {
        running += pdf[i];
        cdf[i] = running;
    }
    cdf.back() = 1.0f;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> U(0.0f, 1.0f);

    for (int i = 0; i < numSamples; ++i) {
        float r = sampleFromCDF(rGrid, cdf, rng);

        float u = U(rng);
        float v = U(rng);

        float theta = std::acos(1.0f - 2.0f * u);
        float phi = 2.0f * M_PI * v;

        Vec3 p{
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