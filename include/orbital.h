#ifndef ORBITAL_H
#define ORBITAL_H

#include <vector>
#include "particle.h"

struct OrbitalCloud {
    std::vector<Vec3> points;
    Vec3 color;

    OrbitalCloud(const Vec3& c = {0.2f, 0.4f, 1.0f}) : color(c) {}

    void generate1s(int numSamples, float a0 = 1.0f);
    void draw(float pointSize = 2.0f) const;
};

#endif