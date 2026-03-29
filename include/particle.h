#ifndef PARTICLE_H
#define PARTICLE_H

#include <cmath>
#include <vector>
#include <glad/glad.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vec3 {
    float x, y, z;
};

struct Particle {
    Vec3 spherical_posititon; // (r, theta, phi) 
    Vec3 color;
    float radius;
    Particle(Vec3 pos, Vec3 c, float radius_particle) : spherical_posititon(pos), color(c), radius(radius_particle) {}

    static Vec3 sphericalToCartesian(const Vec3& s) {
        float r = s.x;
        float theta = s.y;
        float phi = s.z;

        return {
            r * std::sin(theta) * std::cos(phi),
            r * std::sin(theta) * std::sin(phi),
            r * std::cos(theta)
        };
    }

    void draw(int segments = 32) const {
        glColor3f(color.x, color.y, color.z);
        glPushMatrix();

        Vec3 p = sphericalToCartesian(spherical_posititon);
        glTranslatef(p.x, p.y, p.z);

        int stacks = segments / 2;
        int slices = segments;

        for (int i = 0; i < stacks; ++i) {
            float lat0 = M_PI * (-0.5f + static_cast<float>(i) / stacks);
            float lat1 = M_PI * (-0.5f + static_cast<float>(i + 1) / stacks);

            float sinLat0 = std::sin(lat0);
            float cosLat0 = std::cos(lat0);
            float sinLat1 = std::sin(lat1);
            float cosLat1 = std::cos(lat1);

            glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= slices; ++j) {
                float lng = 2.0f * M_PI * static_cast<float>(j) / slices;
                float sinLng = std::sin(lng);
                float cosLng = std::cos(lng);

                float x0 = cosLat0 * cosLng;
                float y0 = sinLat0;
                float z0 = cosLat0 * sinLng;
                glNormal3f(x0, y0, z0);
                glVertex3f(x0 * radius, y0 * radius, z0 * radius);

                float x1 = cosLat1 * cosLng;
                float y1 = sinLat1;
                float z1 = cosLat1 * sinLng;
                glNormal3f(x1, y1, z1);
                glVertex3f(x1 * radius, y1 * radius, z1 * radius);
            }
            glEnd();
        }

        glPopMatrix();
    }
};

#endif