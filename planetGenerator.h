#ifndef PLANETGENERATOR_H
#define PLANETGENERATOR_H


class Planet
{
public:
	float seed = 99999;
    int latSegments = 50;  // More segments = smoother sphere
    int lonSegments = 50;
    float radius = 3.0f;   // Planet radius
    float noise = 0.0f;
    float amplitude = 1.0f;
    float frequency = 2.0f;
    float imguiScale = 0.2f;
    float variation = noise * imguiScale;
    float landLevel = 0.5f;
    float waterThreshold = 0.5f;  // Adjust to match your current threshold
    float blendWidth = 0.05f;     // Controls smoothness - smaller = sharper
    glm::vec3 waterColorBase = glm::vec3(0.0f, 0.1f, 0.4f);
    glm::vec3 waterColorMix = glm::vec3(0.1f, 0.3f, 0.7f);
    glm::vec3 landColorBase = glm::vec3(0.8f, 0.7f, 0.4f);
    glm::vec3 landColorMix = glm::vec3(0.2f, 0.6f, 0.2f);
    glm::vec3 mountainPeakBase = glm::vec3(0.2f, 0.6f, 0.2f);
    glm::vec3 mountainPeakMix = glm::vec3(0.9f, 0.9f, 0.9f);


	glm::vec3 determineColor(float height, glm::vec3 waterColorBase, glm::vec3 waterColorMix, glm::vec3 landColorBase, glm::vec3 landColorMix, glm::vec3 mountainPeakBase, glm::vec3 mountainPeakMix, float blendWidth, float waterThreshold);


	void generateSphereVertices(std::vector<float>& vertices, int latitudeSegments, int longitudeSegments, float radius, float noise, float amplitude, float frequency, float variation);


	void generateSphereIndices(std::vector<unsigned int>& indices, int latitudeSegments, int longitudeSegments);


	glm::vec3 calculateBoundingBoxCenter(const std::vector<float>& vertices);
};
