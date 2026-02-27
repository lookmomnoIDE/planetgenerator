#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <nfd.h>
#include <sstream>
#include <vector>
#include <cmath>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <imgui/imgui.h>
#include <imgui/imconfig.h>
#include <imgui/imgui_internal.h>
#include <imgui/imstb_rectpack.h>
#include <imgui/imstb_textedit.h>
#include <imgui/imstb_truetype.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <glm/vec2.hpp>               // vec2, bvec2, dvec2, ivec2 and uvec2
#include <glm/vec3.hpp>               // vec3, bvec3, dvec3, ivec3 and uvec3
#include <glm/vec4.hpp>               // vec4, bvec4, dvec4, ivec4 and uvec4
#include <glm/mat2x2.hpp>             // mat2, dmat2
#include <glm/mat2x3.hpp>             // mat2x3, dmat2x3
#include <glm/mat2x4.hpp>             // mat2x4, dmat2x4
#include <glm/mat3x2.hpp>             // mat3x2, dmat3x2
#include <glm/mat3x3.hpp>             // mat3, dmat3
#include <glm/mat3x4.hpp>             // mat3x4, dmat2
#include <glm/mat4x2.hpp>             // mat4x2, dmat4x2
#include <glm/mat4x3.hpp>             // mat4x3, dmat4x3
#include <glm/mat4x4.hpp>             // mat4, dmat4
#include <glm/common.hpp>             // all the GLSL common functions: abs, min, mix, isnan, fma, etc.
#include <glm/exponential.hpp>        // all the GLSL exponential functions: pow, log, exp2, sqrt, etc.
#include <glm/geometric.hpp>          // all the GLSL geometry functions: dot, cross, reflect, etc.
#include <glm/integer.hpp>            // all the GLSL integer functions: findMSB, bitfieldExtract, etc.
#include <glm/matrix.hpp>             // all the GLSL matrix functions: transpose, inverse, etc.
#include <glm/packing.hpp>            // all the GLSL packing functions: packUnorm4x8, unpackHalf2x16, etc.
#include <glm/trigonometric.hpp>      // all the GLSL trigonometric functions: radians, cos, asin, etc.
#include <glm/vector_relational.hpp>  // all the GLSL vector relational functions: equal, less, etc.
#include <glm/gtc/matrix_transform.hpp>     // translate, rotate
#include <glm/ext/matrix_clip_space.hpp>    // perspective
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "planetGenerator.h"



std::vector<float> myVertices;
std::vector<unsigned int> myIndices;

void playMusic(const char* musicFile)
{
    std::cout << "playing music!" << std::endl;
    PlaySoundA(musicFile, NULL, SND_FILENAME | SND_ASYNC);
    std::cout << "Music complete!" << std::endl;
}


void exportToPLY(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
                 const std::string& filename)
{
    std::ofstream file(filename);
    int vertexCount = vertices.size() / 7;
    int faceCount = indices.size() / 3;
    
    file << "ply\n";
    file << "format ascii 1.0\n";
    file << "element vertex " << vertexCount << "\n";
    file << "property float x\n";
    file << "property float y\n";
    file << "property float z\n";
    file << "property uchar red\n";
    file << "property uchar green\n";
    file << "property uchar blue\n";
    file << "property uchar alpha\n";  // Add alpha channel
    file << "element face " << faceCount << "\n";
    file << "property list uchar int vertex_indices\n";
    file << "end_header\n";
    
    // Write vertices with RGBA
    for (size_t i = 0; i < vertices.size(); i += 7) {
        file << vertices[i] << " " << vertices[i+1] << " " << vertices[i+2] << " ";
        
        int r = std::max(0, std::min(255, (int)(vertices[i+3] * 255)));
        int g = std::max(0, std::min(255, (int)(vertices[i+4] * 255)));
        int b = std::max(0, std::min(255, (int)(vertices[i+5] * 255)));
        int a = std::max(0, std::min(255, (int)(vertices[i+6] * 255)));
        
        file << r << " " << g << " " << b << " " << a << "\n";
    }
    
    // Write faces
    for (size_t i = 0; i < indices.size(); i += 3) {
        file << "3 " << indices[i] << " " << indices[i+1] << " " << indices[i+2] << "\n";
    }
    
    file.close();
}


static Planet planetGenerator{};

//ImGui default render settings
bool show_demo_window = false;
static bool showSavePopup = false;
static float tempSeed = planetGenerator.seed;  // Temporary buffer for editing
static bool seedChanged = false;
bool dark = true;
bool shouldExit = false;
bool play = false;
bool cameraRotate = false;
float cameraDistance = 20.0f;
float lookatx = 0.0f;
float lookaty = 2.0f;
float lookatz = 0.0f;

//debug
static int frameCount = 0;



//Clear Screen color setting(default: black)
float col1[4] = {0.0f, 0.0f, 0.0f, 1.0f};

//camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
//Camera last position initialization
float lastX;
float lastY;
bool firstMouse = true;
bool cursor = false;

//timing
float deltaTime = 0.0f; //Time between current frame and last frame
float lastFrame = 0.0f;


static void glfw_error_callback(int error, const char* description)
{
    std::cout << stderr << "GLFW error %d: %s\n" << error << description <<std::endl;
}


void clearScreen(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void draw()
{
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawElements(GL_TRIANGLES, myIndices.size(), GL_UNSIGNED_INT, 0);
}


void framebuffer_size_callback(GLFWwindow* window, int SCREENWIDTH, int SCREENHEIGHT)
{
	glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
}


void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        if (show_demo_window)
        {
            show_demo_window = false;
        }
        else if (!show_demo_window)
        {
            show_demo_window = true;
        }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        if(cursor)
        {
            cursor = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        }
        else if(!cursor)
        {
            cursor = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.MovementSpeed = 25.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.MovementSpeed = 2.5f;

}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


static void ShowMyMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                showSavePopup = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                shouldExit = true;
            }
            if (ImGui::MenuItem("Toggle Demo Window", "p")) {
                if (show_demo_window)
                {
                    show_demo_window = false;
                }
                else if (!show_demo_window)
                {
                    show_demo_window = true;
                }
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Toggle light or dark")) {
                if (dark)
                {
                    dark = false;
                }
                else if(!dark)
                {
                    dark = true;
                }
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}


void renderUI();

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

// Then use:


int main()
{
    
	planetGenerator::generateSphereVertices(myVertices, latSegments, lonSegments, radius, noise, amplitude, frequency, variation);
    planetGenerator::generateSphereIndices(myIndices, latSegments, lonSegments);
    std::cout << "Vertices: " << myVertices.size() << std::endl;
    std::cout << "Indices: " << myIndices.size() << std::endl;

    // Calculate sphere center
    std::cout << "=== INITIAL SPHERE INFO ===" << std::endl;
    glm::vec3 sphereCenter = calculateBoundingBoxCenter(myVertices);
    std::cout << "Bounding box center: " << sphereCenter.x << ", " << sphereCenter.y << ", " << sphereCenter.z << std::endl;

    std::cout << "Sphere center: " << sphereCenter[0] << sphereCenter[1] << sphereCenter[2] << std::endl;

    // Initialize GLFW once
    
    if(!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);


    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    if (!primary) {
        std::cout << "Failed to get primary monitor" << std::endl;
        glfwTerminate();
        return -1;
    }


    GLFWvidmode const* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	float SCREENWIDTH = mode->width;
	float SCREENHEIGHT = mode->height;
	std::cout << "Screen size: " << SCREENWIDTH << "x" << SCREENHEIGHT << std::endl;
	lastX = SCREENWIDTH/2.0f;
	lastY = SCREENHEIGHT/2.0f;



    float main_scale = 1.0f;
    try {
        main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(primary);
        std::cout << "Content scale: " << main_scale << std::endl;
    } catch (...) {
        std::cout << "Warning: Could not get content scale, using 1.0" << std::endl;
        main_scale = 1.0f;
    }

    GLFWwindow* window = glfwCreateWindow(SCREENWIDTH*main_scale, SCREENHEIGHT*main_scale, "Planet Generator:P", NULL, NULL);
    //GLFWwindow* window = glfwCreateWindow(1920, 1080, "MyMochi", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create openGL window" << std::endl;
        glfwTerminate();
        return -1;
    }
    std::cout << "Window created" << std::endl;
    
    glfwMakeContextCurrent(window);
    if (glfwGetCurrentContext() != window) {
    std::cout << "ERROR: Failed to make context current!" << std::endl;
    glfwTerminate();
    return -1;
    }
    std::cout << "Context made current successfully" << std::endl;    

    int gladVersion = gladLoadGL();
    std::cout << "GLAD loaded, version: " << gladVersion << std::endl;

    if (gladVersion == 0) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    std::cout << "GLAD initialized" << std::endl;
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "OpenGL Context Version: " << major << "." << minor << std::endl;

    const GLubyte* version = glGetString(GL_VERSION);
    if (version) {
        std::cout << "OpenGL Version String: " << version << std::endl;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    const GLubyte* renderer = glGetString(GL_RENDERER);

    if (version) {
        std::cout << "OpenGL Version: " << version << std::endl;
    } else {
        std::cout << "ERROR: glGetString(GL_VERSION) returned NULL" << std::endl;
    }

    if (renderer) {
        std::cout << "Renderer: " << renderer << std::endl;
    } else {
        std::cout << "ERROR: glGetString(GL_RENDERER) returned NULL" << std::endl;
    }
    // ADD THIS CODE HERE:
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // Check if glCreateShader function was loaded
    if (glCreateShader == nullptr) {
        std::cout << "ERROR: glCreateShader function pointer is NULL!" << std::endl;
        return -1;
    }
    std::cout << "glCreateShader function loaded successfully" << std::endl;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    std::cout << "Callbacks initialized" << std::endl;

    //enable blending and depth testing
    //
    std::cout << "Boutta load that shader!" << std::endl;

    Shader shader("../src/cool.vert", "../src/beans.frag");

    std::cout << "shader loaded sucessfully!" << std::endl;

    unsigned int VBO, EBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);  // Create VBO
    glGenBuffers(1, &EBO);  // Create EBO

    glBindVertexArray(VAO);

    // VERTEX DATA goes in VBO (GL_ARRAY_BUFFER)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        myVertices.size() * sizeof(float),
        myVertices.data(),
        GL_STATIC_DRAW);

    // INDEX DATA goes in EBO (GL_ELEMENT_ARRAY_BUFFER)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        myIndices.size() * sizeof(unsigned int),
        myIndices.data(),
        GL_STATIC_DRAW);

    // Now set up vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
	shader.use();
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    std::cout << "Initializing ImGui..." << std::endl;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	io.ConfigDpiScaleFonts = true;
	io.ConfigDpiScaleViewports = false;

	std::cout << "Initializing ImGui backends..." << std::endl;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");


	glViewport(0, 0, SCREENWIDTH * main_scale, SCREENHEIGHT * main_scale);
	std::cout << "Entering main loop..." << std::endl;





	while(!glfwWindowShouldClose(window))
	{
		// Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
		processInput(window);

		//RENDERING HERE
		clearScreen(col1[0], col1[1], col1[2], col1[3]);
        // In your render loop or input handler
        static float theta = 0.0f;
        static float phi = 0.0f;
		shader.use();
		
		// pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREENWIDTH / (float)SCREENHEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        float angle = 20.0f;
        //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);
        // Calculate world space center
        /*glm::vec3 localCenter = calculateSphereCenter(myVertices);
        glm::vec4 worldCenter = model * glm::vec4(localCenter, 1.0f);
        glm::vec3 sphereCenter = glm::vec3(worldCenter);*/
        draw();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
        renderUI();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}


        // In your main loop, track when parameters change:
        static float lastNoise = noise;
        static float lastAmplitude = amplitude;
        static float lastFrequency = frequency;
        static int lastlatSegments = latSegments;
        static int lastlonSegments = lonSegments;
        static float lastRadius = radius;
        static float lastScale = imguiScale;
        //static float lastWaterLevel = waterLevel;
        static float lastLandLevel = landLevel;
        static glm::vec3 lastWaterColorBase = waterColorBase;
        static glm::vec3 lastWaterColorMix = waterColorMix;
        static glm::vec3 lastLandColorBase = landColorBase;
        static glm::vec3 lastLandColorMix = landColorMix;
        static glm::vec3 lastMountainPeakBase = mountainPeakBase;
        static glm::vec3 lastMountainPeakMix = mountainPeakMix;
        static float lastSeed = seed;
        static float lastBlendWidth = blendWidth;
        static float lastWaterThreshold = waterThreshold;
        static float tempSeed = seed;
        // ... track all parameters that affect generation

        // After ImGui updates
        if (noise != lastNoise || amplitude != lastAmplitude || 
            frequency != lastFrequency || lastlatSegments != latSegments || lastlonSegments != lonSegments || lastRadius != radius || lastScale != imguiScale ||
            lastLandLevel != landLevel || lastWaterColorBase != waterColorBase || lastWaterColorMix != waterColorMix || tempSeed != seed || 
            lastLandColorBase != landColorBase || lastLandColorMix != landColorMix || lastMountainPeakBase != mountainPeakBase || lastMountainPeakMix != mountainPeakMix ||
            lastSeed != seed || lastBlendWidth != blendWidth || lastWaterThreshold != waterThreshold) 
        {
            
            // Clear and regenerate
            myVertices.clear();
            myIndices.clear();
            planetGenerator::generateSphereVertices(myVertices, latSegments, lonSegments, 
                                  radius, noise, amplitude, frequency, variation);
            planetGenerator::generateSphereIndices(myIndices, latSegments, lonSegments);
            
            // Re-upload to GPU
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                myVertices.size() * sizeof(float),
                myVertices.data(),
                GL_DYNAMIC_DRAW);

            // INDEX DATA goes in EBO (GL_ELEMENT_ARRAY_BUFFER)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                myIndices.size() * sizeof(unsigned int),
                myIndices.data(),
                GL_DYNAMIC_DRAW);
            // Update tracked values
            lastNoise = noise;
            lastAmplitude = amplitude;
            lastFrequency = frequency;
            lastlatSegments = latSegments;
            lastlonSegments = lonSegments;
            lastRadius = radius;
            lastScale = imguiScale;
            //lastWaterLevel = waterLevel;
            lastLandLevel = landLevel;
            lastWaterColorBase = waterColorBase;
            lastWaterColorMix = waterColorMix;
            lastLandColorBase = landColorBase;
            lastLandColorMix = landColorMix;
            lastMountainPeakBase = mountainPeakBase;
            lastMountainPeakMix = mountainPeakMix;
            lastSeed = seed;
            lastBlendWidth = blendWidth;
            lastWaterThreshold = waterThreshold;

        }

		
		//END RENDERING CODE
		glfwSwapBuffers(window);
		glfwPollEvents();
        if (!dark)
        {
            ImGui::StyleColorsLight();
        }
        else
        {
            ImGui::StyleColorsDark();
        }
        if (shouldExit)
        {
            glfwSetWindowShouldClose(window, true);
        }
		if (play)
        {
            playMusic("C:\\projects\\MM2\\res\\song.wav");            
            play = false;
            cameraRotate = true;
        }
        if (cameraRotate)
        {
            theta += deltaTime * 0.5f;  // Rotate horizontally over time
            // Rotate camera around sphere
            camera.rotate(theta, 0.0f, glm::vec3(lookatx, lookaty, lookatz), cameraDistance);
            std::cout << "Theta: " << theta << ", Phi: " << phi << std::endl;
            std::cout << "Target: " << sphereCenter.x << ", " << sphereCenter.y << ", " << sphereCenter.z << std::endl;
            std::cout << "Camera Pos: " << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl;
            std::cout << "Camera Front: " << camera.Front.x << ", " << camera.Front.y << ", " << camera.Front.z << std::endl;
            std::cout << "---" << std::endl;

            if (frameCount % 60 == 0) {  // Print every 60 frames
                std::cout << "Camera Distance from target: " << glm::length(camera.Position - sphereCenter) << std::endl;
                std::cout << "Camera Position: " << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl;
                std::cout << "Sphere Center: " << sphereCenter.x << ", " << sphereCenter.y << ", " << sphereCenter.z << std::endl;
            }
            frameCount++;
        }
	}

	std::cout << "Cleaning up..." << std::endl;
	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	std::cout << "Application finished" << std::endl;
	return 0;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if (cursor)
    {
        return;
    }
    else if(!cursor)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    //camera.ProcessMouseMovement(xoffset, yoffset);
}

void renderUI()
{
    ShowMyMainMenuBar();
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    ImGui::Begin("Planet Generator!");
    {
        ImGui::ColorEdit4("Clear Color", col1);
        //ImGui::InputFloat("Input seed value(Default: 99,999)", &seed, 0.0f, 99999.0f);
        ImGui::InputFloat("Seed(Default: 99,999.0", &tempSeed, 0.0f, 99999.0f);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            planetGenerator.seed = tempSeed;
            seedChanged = true;
        }

        if (ImGui::Button("Apply Seed")) {
            planetGenerator.seed = tempSeed;
            seedChanged = true;
        }
        ImGui::SliderInt("Latitude Segments", &latSegments, 10, 1000);
        ImGui::SliderInt("Longitude Segments", &lonSegments, 10, 1000);
        ImGui::SliderFloat("Planet Radius", &radius, 1.0f, 10.0f);
        ImGui::SliderFloat("Noise", &noise, 0.0f, 1.0f);
        ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 2.0f);
        ImGui::SliderFloat("Frequency", &frequency, 0.0f, 100.0f);
        ImGui::SliderFloat("Scale of Variation", &imguiScale, 0.01f, 1.0f);
        ImGui::SliderFloat("Smooth step", &blendWidth, 0.001f, 1.0f);
        ImGui::SliderFloat("Water threshold", &waterThreshold, 0.1f, 1.0f);
        //ImGui::SliderFloat("Water Level", &waterLevel, 0.0f, 1.0f);
        ImGui::SliderFloat("Land Level", &landLevel, 0.0f, 1.0f);
        ImGui::ColorEdit3("Water color base", glm::value_ptr(waterColorBase));
        ImGui::ColorEdit3("Water color mix", glm::value_ptr(waterColorMix));
        ImGui::ColorEdit3("Land color base", glm::value_ptr(landColorBase));
        ImGui::ColorEdit3("Land color mix", glm::value_ptr(landColorMix));
        ImGui::ColorEdit3("Mountain peak color base", glm::value_ptr(mountainPeakBase));
        ImGui::ColorEdit3("Mountain peak color mix", glm::value_ptr(mountainPeakMix));
        ImGui::SliderFloat("Camera Distance", &cameraDistance, 4.0f, 25.0f);
        ImGui::SliderFloat("X", &lookatx, -4.0f, 4.0f);
        ImGui::SliderFloat("Y", &lookaty, -4.0f, 4.0f);
        ImGui::SliderFloat("Z", &lookatz, -4.0f, 4.0f);
        if (ImGui::Button("I LOST THE PLANET!!!"))
        {
            camera.Reset(glm::vec3(0.0f, 0.0f, 10.0f));
        }
        if (ImGui::Button("Play"))
        {
            play = true;
            std::cout << play << std::endl;
        }
        ImGui::InputFloat3("Camera Position", glm::value_ptr(camera.Position));
        if (showSavePopup) {
        ImGui::OpenPopup("Save Export");
        }

        if (ImGui::BeginPopupModal("Save Export", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char buf[256] = "planet";
            static char folder[512] = "./exports/";
            
            ImGui::InputText("Folder", folder, IM_ARRAYSIZE(folder));
            ImGui::SameLine();
            if (ImGui::Button("Browse...")) {
                nfdchar_t *outPath = NULL;
                nfdresult_t result = NFD_PickFolder(&outPath, NULL);
                if (result == NFD_OKAY) {
                    snprintf(folder, sizeof(folder), "%s/", outPath);
                    NFD_FreePath(outPath);
                }
            }
            
            ImGui::InputText("Filename", buf, IM_ARRAYSIZE(buf));
            ImGui::Text("Will save as: %s%s.ply", folder, buf);
            
            if (ImGui::Button("Save")) {
                std::filesystem::create_directories(folder);
                std::string filename = std::string(folder) + std::string(buf) + ".ply";
                exportToPLY(myVertices, myIndices, filename);
                showSavePopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                showSavePopup = false;
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }

    }
    ImGui::End();
}