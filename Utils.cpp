#include "Utils.h"

float Utils::ComputeArea(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
    float l1 = glm::length(p1 - p2);
    float l2 = glm::length(p2 - p3);
    float l3 = glm::length(p3 - p1);
    float s = (l1 + l2 + l3) / 2.0f;
    return sqrt(s * (s - l1) * (s - l2) * (s - l3));
}
