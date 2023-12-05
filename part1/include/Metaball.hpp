#ifndef METABALL_HPP
#define METABALL_HPP

#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <random>

#include "glm/glm.hpp"

class Metaball {
public:
    Metaball(glm::vec3 center, float radius, glm::vec3 velocity) : center(center), radius(radius), velocity(velocity) {
        
    }

    // Function to evaluate the metaball's field at a point
    float evaluate(const glm::vec3& point) const {
        glm::vec3 diff = point - center;
        return radius / glm::dot(diff, diff);
    }

    glm::vec3 getCenter() {
        return center;
    }

    float getRadius() {
        return radius;
    }

    void update(float deltaTime) {
        // Move the metaball according to its velocity
        center += velocity * deltaTime;

        // Check for collisions with the cube boundaries and bounce back
        for (int i = 0; i < 3; ++i) {
            if (center[i] < 0 || center[i] > 4) {
                velocity[i] *= -1; // Reverse velocity on collision

                // Add randomness
                float randomFactor = (rand() % 100 - 50) / 1000.0f; // Small random value between -0.05 and 0.05
                velocity[i] += randomFactor;

                center[i] = glm::clamp(center[i], 0.0f, 4.0f); // Keep within bounds
            }
        }
    }

private:
    glm::vec3 center;
    float radius;
    glm::vec3 velocity;
};

float evaluateMetaballs(const glm::vec3& point, const std::vector<Metaball>& metaballs) {
    float field = 0.0;
    for (const auto& metaball : metaballs) {
        field += metaball.evaluate(point); 
    }
    return field;
}

std::vector<Metaball> createRandomMetaballs(size_t count) {
    srand(static_cast<unsigned int>(time(0)));
    std::vector<Metaball> metaballs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> posDist(-3.0, 3.0);
    std::uniform_real_distribution<> velDist(-1.0, 1.0);

    for (size_t i = 0; i < count; ++i) {
        glm::vec3 position(posDist(gen), posDist(gen), 0.0f); // Random position on x, y
        glm::vec3 velocity(velDist(gen), velDist(gen), 0.0f); // Random velocity on x, y
        metaballs.push_back(Metaball(position, 1.0f, velocity)); // Assuming radius is always 1.0
    }

    return metaballs;
}


#endif
