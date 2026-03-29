#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>
#include <cmath>
#include <vector>
#include <glad/glad.h>

using namespace std;

#define M_PI 3.14159265358979323846

struct Particle {
    vector<float> position; // (r, theta, phi) 
    vector<float> color;
    float radius;
    Particle(vector<float> pos, vector<float> c, float radius_particle) : position(pos), color(c), radius(radius_particle) {}

    void draw (vector<float> pos, float radius, int segments) {
        glColor3f(color[0], color[1], color[2]);
        glPushMatrix();

        vector<float> position_cartesian = sphericalToCartesian(position);
        glTranslatef(position_cartesian[0], position_cartesian[1], position_cartesian[2]);

        int stacks = segments / 2;
        int slices = segments;
        
        // Draw sphere using latitude/longitude lines
        for (int i = 0; i < stacks; ++i) {
            float lat0 = M_PI * (-0.5f + (float)i / stacks);
            float lat1 = M_PI * (-0.5f + (float)(i + 1) / stacks);
            
            float sinLat0 = sin(lat0);
            float cosLat0 = cos(lat0);
            float sinLat1 = sin(lat1);
            float cosLat1 = cos(lat1);
            
            glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= slices; ++j) {
                float lng = 2.0f * M_PI * (float)j / slices;
                float sinLng = sin(lng);
                float cosLng = cos(lng);
                
                // First vertex
                float x0 = cosLat0 * cosLng;
                float y0 = sinLat0;
                float z0 = cosLat0 * sinLng;
                glNormal3f(x0, y0, z0); // Normal Spoints outward from center
                glVertex3f(x0 * radius, y0 * radius, z0 * radius);
                
                // Second vertex
                float x1 = cosLat1 * cosLng;
                float y1 = sinLat1;
                float z1 = cosLat1 * sinLng;
                glNormal3f(x1, y1, z1); // Normal points outward from center
                glVertex3f(x1 * radius, y1 * radius, z1 * radius);
            }
            glEnd();
        }
        
        glPopMatrix();
    }

    vector<float> sphericalToCartesian(vector<float> spherical_position) {
        vector<float> cartesian(3);
        float r = spherical_position[0];
        float theta = spherical_position[1];
        float phi = spherical_position[2];
        cartesian[0] = r * sin(theta) * cos(phi); 
        cartesian[1] = r * sin(theta) * sin(phi); 
        cartesian[2] = r * cos(theta);            
        return cartesian;
    }
};

#endif