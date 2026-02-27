#include <glm/gtc/noise.hpp>


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



class Planet
{
public:
/*    //Planet settings!
    float seed;
    int latSegments;  // More segments = smoother sphere
    int lonSegments;
    float radius;   // Planet radius
    float noise;
    float amplitude;
    float frequency;
    //Scale noise
    float imguiScale;
    float variation;

    // Color based on elevation
    //float height = (noise + 1.0f) * 0.5f; // Normalize to 0-1
    //float waterLevel = 0.3f;
    float landLevel;
    float waterThreshold;  // Adjust to match your current threshold
    float blendWidth;     // Controls smoothness - smaller = sharper
    //std::vector<float>
    glm::vec3 waterColorBase;
    //std::vector<float>
    glm::vec3 waterColorMix;
    //std::vector<float>
    glm::vec3 landColorBase;
    //std::vector<float>
    glm::vec3 landColorMix;
    //std::vector<float>
    glm::vec3 mountainPeakBase;
    //std::vector<float>
    glm::vec3 mountainPeakMix;
*/
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



    glm::vec3 determineColor(float height, glm::vec3 waterColorBase, glm::vec3 waterColorMix, glm::vec3 landColorBase, glm::vec3 landColorMix, glm::vec3 mountainPeakBase, glm::vec3 mountainPeakMix, float blendWidth, float waterThreshold) 
    {
        
        
        if (height < waterThreshold - blendWidth) {
            // Deep water
            return glm::mix(waterColorBase, waterColorMix, height / waterThreshold);
        }
        else if (height < waterThreshold + blendWidth) {
            // Smooth transition zone (the fix!)
            float t = (height - (waterThreshold - blendWidth)) / (2.0f * blendWidth);
            t = glm::smoothstep(0.0f, 1.0f, t);  // Smooth interpolation
            
            glm::vec3 waterColor = glm::mix(waterColorBase, waterColorMix, height / waterThreshold);
            glm::vec3 landColor = glm::mix(landColorBase, landColorMix, (height - waterThreshold) / (1.0f - waterThreshold));
            
            return glm::mix(waterColor, landColor, t);
        }
        else if (height > waterThreshold && height < landLevel) {
            float t = (height - (landLevel - blendWidth)) / (2.0f * blendWidth);
            t = glm::smoothstep(0.0f, 1.0f, t);
            return glm::mix(landColorBase, landColorMix, t);
        }
        else {
            // Land and mountains
            //float landHeight = (height - waterThreshold) / (1.0f - waterThreshold);
            // Your existing land/mountain color logic here
            return glm::mix(mountainPeakBase, mountainPeakMix, (height - landLevel) / (1.0f + landLevel));
        }
    }


    void generateSphereVertices(std::vector<float>& vertices, int latitudeSegments, int longitudeSegments, float radius, float noise, float amplitude, float frequency, float variation) 
    {
        for (int lat = 0; lat <= latitudeSegments; ++lat) {
            float theta = lat * M_PI / latitudeSegments;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);
            
            for (int lon = 0; lon <= longitudeSegments; ++lon) {
                float phi = lon * 2.0f * M_PI / longitudeSegments;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);
                
                // Base position
                glm::vec3 basePos(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
                float NOISE = noise;
                float freq = frequency;    // Use local copies
                float amp = amplitude;
                for (int i = 0; i < 4; i++) {
                    glm::vec3 p = basePos * freq + glm::vec3(seed, seed * 1.1f, seed * 0.9f);
                    NOISE += amp * glm::perlin(p);
                    freq *= 2.0f;
                    amp *= 0.5f;
                }
                float variation = NOISE * 0.2f;
                float variedRadius = radius * (1.0f + variation);
                //float variedRadius = radius * (1.0f + NOISE);
                glm::vec3 pos = basePos * variedRadius;
                float actualRadius = glm::length(pos);
                float minRadius = radius * (1.0f - imguiScale);  // Assuming max variation is 0.2
                float maxRadius = radius * (1.0f + imguiScale);
                float height = (actualRadius - minRadius) / (maxRadius - minRadius);
                glm::vec3 color = determineColor(height, waterColorBase, waterColorMix, landColorBase, landColorMix, mountainPeakBase, mountainPeakMix, blendWidth, waterThreshold);
                
                vertices.push_back(pos[0]);
                vertices.push_back(pos[1]);
                vertices.push_back(pos[2]);            
                vertices.push_back(color[0]);
                vertices.push_back(color[1]);
                vertices.push_back(color[2]);
                vertices.push_back(1.0f);
            }
        }
    }


    void generateSphereIndices(std::vector<unsigned int>& indices, int latitudeSegments, int longitudeSegments)
    {
        for (int lat = 0; lat < latitudeSegments; ++lat) {
            for (int lon = 0; lon < longitudeSegments; ++lon) {
                    int first = lat * (longitudeSegments + 1) + lon;
                    int second = first + longitudeSegments + 1;
                
                // First triangle
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);
                
                // Second triangle
                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }


    glm::vec3 calculateBoundingBoxCenter(const std::vector<float>& vertices) {
        glm::vec3 min(FLT_MAX);
        glm::vec3 max(-FLT_MAX);
        
        // Step through vertices (stride of 7)
        for (size_t i = 0; i < vertices.size(); i += 7) {
            glm::vec3 pos(vertices[i], vertices[i + 1], vertices[i + 2]);
            min = glm::min(min, pos);
            max = glm::max(max, pos);
        }
        std::cout << "Min bounds: " << min.x << ", " << min.y << ", " << min.z << std::endl;
        std::cout << "Max bounds: " << max.x << ", " << max.y << ", " << max.z << std::endl;
        std::cout << "Radius used: " << radius << std::endl;
        std::cout << "ImguiScale: " << imguiScale << std::endl;
        std::cout << "Expected range: " << radius * (1.0f - imguiScale) << " to " << radius * (1.0f + imguiScale) << std::endl;

    return (min + max) * 0.5f;
}
};

 

