#ifndef ORBITAL_H
#define ORBITAL_H

#include <vector>
#include "particle.h"

class OrbitalCloud {
public:

    /**
     * @brief Constructs an OrbitalCloud with the specified color.
     * @param color The RGB color of the orbital cloud, with each component in the range [0.0, 1.0].
     */
    OrbitalCloud(const Vec3& color = {0.2f, 0.5f, 1.0f});

    /**
     * @brief Generates a set of particles representing the electron cloud for a hydrogen-like atom in a specific quantum state.
     * @param n The principal quantum number (n >= 1).
     * @param l The azimuthal quantum number (0 <= l < n).
     * @param m The magnetic quantum number (-l <= m <= l).
     * @param numSamples The number of samples to generate.
     * @param a0 The Bohr radius (default is 1.0f).
     * @throws std::invalid_argument if the quantum numbers are out of valid ranges or if numSamples is not positive.
     */
    void generateHydrogenState(int n, int l, int m, int numSamples, float a0 = 1.0f);

    /**
     * @brief Draws the orbital cloud using OpenGL.
     * @param pointSize The size of each particle point (default is 2.0f).
     */
    void draw(float pointSize = 2.0f) const;

private:
    std::vector<Vec3> points;
    Vec3 color;
};

#endif