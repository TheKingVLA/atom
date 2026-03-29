#ifndef PARTICLE_H
#define PARTICLE_H

#include <cmath>
#include <glad/glad.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vec3 {
    float x, y, z;
};

struct Particle {
    Vec3 spherical_coordinates;   // (r, theta, phi)
    Vec3 color;
    float radius;

    /**
     * @brief Constructs a Particle with the specified spherical position, color, and radius.
     * @param position The spherical position (r, theta, phi).
     * @param color_param The RGB color of the particle, with each component in the range [0.0, 1.0].
     * @param radius_param The radius of the particle.
     */
    Particle(Vec3 position, Vec3 color_param, float radius_param)
        : spherical_coordinates(position), color(color_param), radius(radius_param) {}

    /**
     * @brief Converts spherical coordinates to Cartesian coordinates.
     * @param spherical_coords The spherical coordinates (r, theta, phi).
     * @return The Cartesian coordinates (x, y, z).
     */
    static Vec3 sphericalToCartesian(const Vec3& spherical_coords) {
        float r     = spherical_coords.x;
        float theta = spherical_coords.y;
        float phi   = spherical_coords.z;

        return {
            r * std::sin(theta) * std::cos(phi),
            r * std::sin(theta) * std::sin(phi),
            r * std::cos(theta)
        };
    }

    /**
     * @brief Draws the particle as a sphere using OpenGL.
     * @param sphere_segments The number of segments to use for the sphere (default is 32).
     * The sphere_segments variable defines the stacks and slices of the sphere.
     */
    void draw(int sphere_segments = 32) const {
        glColor3f(color.x, color.y, color.z);
        glPushMatrix();

        Vec3 cartesian_position = sphericalToCartesian(spherical_coordinates);
        glTranslatef(cartesian_position.x, cartesian_position.y, cartesian_position.z);

        int stacks = sphere_segments / 2;
        int slices = sphere_segments;

        for (int stack_index = 0; stack_index < stacks; ++stack_index) {
            float latitude_0 = M_PI * (-0.5f + static_cast<float>(stack_index) / stacks);
            float latitude_1 = M_PI * (-0.5f + static_cast<float>(stack_index + 1) / stacks);

            float sin_latitude_0 = std::sin(latitude_0);
            float cos_latitude_0 = std::cos(latitude_0);
            float sin_latitude_1 = std::sin(latitude_1);
            float cos_latitude_1 = std::cos(latitude_1);

            glBegin(GL_TRIANGLE_STRIP);
            for (int slice_index = 0; slice_index <= slices; ++slice_index) {
                float longitude  = 2.0f * M_PI * static_cast<float>(slice_index) / slices;
                float sin_longitude = std::sin(longitude);
                float cos_longitude = std::cos(longitude);

                float vertex_x_0 = cos_latitude_0 * cos_longitude;
                float vertex_y_0 = sin_latitude_0;
                float vertex_z_0 = cos_latitude_0 * sin_longitude;
                glNormal3f(vertex_x_0, vertex_y_0, vertex_z_0);
                glVertex3f(vertex_x_0 * radius, vertex_y_0 * radius, vertex_z_0 * radius);

                float vertex_x_1 = cos_latitude_1 * cos_longitude;
                float vertex_y_1 = sin_latitude_1;
                float vertex_z_1 = cos_latitude_1 * sin_longitude;
                glNormal3f(vertex_x_1, vertex_y_1, vertex_z_1);
                glVertex3f(vertex_x_1 * radius, vertex_y_1 * radius, vertex_z_1 * radius);
            }
            glEnd();
        }

        glPopMatrix();
    }
};

#endif