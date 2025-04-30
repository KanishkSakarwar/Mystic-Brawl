#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp> // Include GLM for glm::vec3

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Player position
float playerX = 0.0f, playerY = 0.0f;
const float playerSpeed = 0.01f;

// Enemy positions
float enemyPositions[3][2] = {
    { 0.3f,  0.3f },
    { -0.5f, -0.2f },
    { 0.7f, -0.5f }
};

// Vertex Shader Source
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 offset;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x + offset.x, aPos.y + offset.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";

// Load texture from file
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Failed to load texture: " << path << "\n";
    }
    
    stbi_image_free(data);
    return textureID;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        playerY += playerSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        playerY -= playerSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        playerX -= playerSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        playerX += playerSpeed;
}

int main() {
    // GLFW initialization
    int score = 0;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mystic Brawl", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex data
    float playerVertices[] = {
        -0.1f,  0.1f, 0.0f,  0.0f, 1.0f,  // top left
        -0.1f, -0.1f, 0.0f,  0.0f, 0.0f,  // bottom left
         0.1f, -0.1f, 0.0f,  1.0f, 0.0f,  // bottom right
         0.1f,  0.1f, 0.0f,  1.0f, 1.0f   // top right
    };
    float quadVertices[] = {
        // positions        // tex coords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,  // top left
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // bottom left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // bottom right
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f   // top right
    };

    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    // Background VAO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Player VAO
    unsigned int playerVAO, playerVBO, playerEBO;
    glGenVertexArrays(1, &playerVAO);
    glGenBuffers(1, &playerVBO);
    glGenBuffers(1, &playerEBO);
    
    glBindVertexArray(playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load textures
    stbi_set_flip_vertically_on_load(true);
    unsigned int bgTexture = loadTexture("textures/grass.png");
    unsigned int playerTexture = loadTexture("textures/player.png");

    // Load enemy textures
    unsigned int enemyTextures[3];
    enemyTextures[0] = loadTexture("textures/enemy.png");
    enemyTextures[1] = loadTexture("textures/enemy.png");
    enemyTextures[2] = loadTexture("textures/enemy.png");


    // Render loop
    unsigned int bulletTexture = loadTexture("textures/bullet.png");
    // Set the window to full screen
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
    glViewport(0, 0, mode->width, mode->height);
    float st = 0;
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw background
        glUseProgram(shader);
        glUniform2f(glGetUniformLocation(shader, "offset"), 0.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, bgTexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Draw player
        glUniform2f(glGetUniformLocation(shader, "offset"), playerX, playerY);
        glBindTexture(GL_TEXTURE_2D, playerTexture);
        glBindVertexArray(playerVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // Additional player movement using arrow keys
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && playerY + 0.1f < 1.0f)
            playerY += playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && playerY - 0.1f > -1.0f)
            playerY -= playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && playerX - 0.1f > -1.0f)
            playerX -= playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && playerX + 0.1f < 1.0f)
            playerX += playerSpeed;

        // Draw player 2
        static float player2X = 0.5f, player2Y = 0.5f; // Initial position for player 2
        static bool player2ProjectileActive = false;
        static float player2ProjectileX = player2X, player2ProjectileY = player2Y;
        static bool player2SpacePressed = false;
        static bool player2AttackLeftActive = false;
        static bool player2AttackRightActive = false;
        static bool player2LeftPressed = false;
        static bool player2RightPressed = false;

        // Handle player 2 movement
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && player2Y + 0.1f < 1.0f)
            player2Y += playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && player2Y - 0.1f > -1.0f)
            player2Y -= playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && player2X - 0.1f > -1.0f)
            player2X -= playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && player2X + 0.1f < 1.0f)
            player2X += playerSpeed;

        // Draw player 2
        glUniform2f(glGetUniformLocation(shader, "offset"), player2X, player2Y);
        glBindTexture(GL_TEXTURE_2D, playerTexture);
        glBindVertexArray(playerVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // Handle player 2 left attack
        if (!player2AttackLeftActive && !player2AttackRightActive && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !player2LeftPressed) {
            player2LeftPressed = true;
            player2ProjectileX = player2X;
            player2ProjectileY = player2Y;
            player2AttackLeftActive = true;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
            player2LeftPressed = false;
        }

        if (player2AttackLeftActive) {
            player2ProjectileX -= 0.05f; // Move projectile to the left

            // Draw player 2 left attack projectile
            glUniform2f(glGetUniformLocation(shader, "offset"), player2ProjectileX, player2ProjectileY);
            glBindTexture(GL_TEXTURE_2D, bulletTexture); // Use bullet texture
            glBindVertexArray(playerVAO); // Reusing player VAO
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Deactivate projectile if it goes out of bounds
            if (player2ProjectileX < -1.0f) {
            player2AttackLeftActive = false;
            }
        }

        if (!player2AttackLeftActive && !player2AttackRightActive && glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !player2RightPressed) {
            player2RightPressed = true;
            player2ProjectileX = player2X;
            player2ProjectileY = player2Y;
            player2AttackRightActive = true;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
            player2RightPressed = false;
        }

        if (player2AttackRightActive) {
            player2ProjectileX += 0.05f; // Move projectile to the right

            // Draw player 2 right attack projectile
            glUniform2f(glGetUniformLocation(shader, "offset"), player2ProjectileX, player2ProjectileY);
            glBindTexture(GL_TEXTURE_2D, bulletTexture); // Use bullet texture
            glBindVertexArray(playerVAO); // Reusing player VAO
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Deactivate projectile if it goes out of bounds
            if (player2ProjectileX > 1.0f) {
            player2AttackRightActive = false;
            }
        }

        // Check for collisions between player 2 projectile and enemies
        if (player2ProjectileActive || player2AttackLeftActive || player2AttackRightActive) {
            for (int i = 0; i < 3; i++) {
            if (abs(player2ProjectileX - enemyPositions[i][0]) < 0.1f && abs(player2ProjectileY - enemyPositions[i][1]) < 0.1f) {
                score++;
                // Respawn enemy at a random position, ensuring it's not where the players are
                do {
                enemyPositions[i][0] = ((rand() % 200) - 100) / 100.0f; // Random value between -1.0 and 1.0
                enemyPositions[i][1] = ((rand() % 200) - 100) / 100.0f; // Random value between -1.0 and 1.0
                } while ((abs(enemyPositions[i][0] - playerX) < 0.2f && abs(enemyPositions[i][1] - playerY) < 0.2f) ||
                     (abs(enemyPositions[i][0] - player2X) < 0.2f && abs(enemyPositions[i][1] - player2Y) < 0.2f));
                
                player2ProjectileActive = false; // Deactivate upward projectile
                player2AttackLeftActive = false; // Deactivate left attack
                player2AttackRightActive = false; // Deactivate right attack
                break;
            }
            }
        }
        // Draw enemies
        for (int i = 0; i < 3; i++) {
            glUniform2f(glGetUniformLocation(shader, "offset"), enemyPositions[i][0], enemyPositions[i][1]);
            glBindTexture(GL_TEXTURE_2D, enemyTextures[i]);
            glBindVertexArray(playerVAO); // Reusing player VAO
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

    // Handle enemy movement
    static float enemyMoveDirections[3][2] = { {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} };
    static float lastDirectionChangeTime[3] = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < 3; i++) {
        float currentTime = glfwGetTime();

        // Change direction every 2 seconds
        if (currentTime - lastDirectionChangeTime[i] >= st) {
        lastDirectionChangeTime[i] = currentTime;
        float angle = ((rand() % 360) * 3.14159f) / 180.0f; // Random angle in radians
        enemyMoveDirections[i][0] = cos(angle) * 0.005f; // Speed in x direction
        enemyMoveDirections[i][1] = sin(angle) * 0.005f; // Speed in y direction
        }

        // Update enemy position
        enemyPositions[i][0] += enemyMoveDirections[i][0];
        enemyPositions[i][1] += enemyMoveDirections[i][1];

        // Reverse direction if enemy goes offscreen
        if (enemyPositions[i][0] < -1.0f || enemyPositions[i][0] > 1.0f) {
        enemyMoveDirections[i][0] = -enemyMoveDirections[i][0];
        }
        if (enemyPositions[i][1] < -1.0f || enemyPositions[i][1] > 1.0f) {
        enemyMoveDirections[i][1] = -enemyMoveDirections[i][1];
        }
    }
    st = 2.0;

        // Handle projectile logic
        static bool spacePressed = false;
        static float projectileX = playerX, projectileY = playerY;
        static bool projectileActive = false;
        static bool projact = false;
        static bool xpres = false;
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            std::cout << "Game Over" << std::endl <<"Enemies Killed: " << score << std::endl;
                glfwSetWindowShouldClose(window, true); // Close the window
                break;
        }
        if (!projact&&glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !spacePressed) {
            spacePressed = true;
            if (!projectileActive) {
            projectileX = playerX;
            projectileY = playerY;
            projectileActive = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
            spacePressed = false;
        }
         if (!projectileActive&&glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !spacePressed) {
            xpres = true;
            if (!projact) {
            projectileX = playerX;
            projectileY = playerY;
            projact = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE) {
            xpres = false;
        }

        if (projectileActive) {
            projectileX += 0.05f; // Move projectile to the right with increased speed

            // Draw projectile
            glUniform2f(glGetUniformLocation(shader, "offset"), projectileX, projectileY);
            glBindTexture(GL_TEXTURE_2D, bulletTexture); // Use bullet texture
            glBindVertexArray(playerVAO); // Reusing player VAO
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Deactivate projectile if it goes out of bounds
            if (projectileX > 1.0f) {
            projectileActive = false;
            }
        }

         if(projact){
            projectileX -= 0.05f; // Move projectile to the right with increased speed

            // Draw projectile
            glUniform2f(glGetUniformLocation(shader, "offset"), projectileX, projectileY);
            glBindTexture(GL_TEXTURE_2D, bulletTexture); // Use bullet texture
            glBindVertexArray(playerVAO); // Reusing player VAO
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Deactivate projectile if it goes out of bounds
            if (projectileX < -1.0f) {
            projact = false;
            }
        }

        // Handle enemy projectiles
        static float enemyProjectilePositions[3][2] = { {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} };
        static float enemyProjectileDirections[3][2] = { {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} };
        static bool enemyProjectileActive[3] = { false, false, false };
        static float lastEnemyShotTime[3] = { 0.0f, 0.0f, 0.0f };
        static unsigned int axeTexture = loadTexture("textures/attack.png");

        float currentTime = glfwGetTime();
        for (int i = 0; i < 3; i++) {
            // Shoot a projectile every 0.75 seconds (increased frequency)
            if (currentTime - lastEnemyShotTime[i] >= 0.75f) {
            lastEnemyShotTime[i] = currentTime;
            if (!enemyProjectileActive[i]) {
                enemyProjectilePositions[i][0] = enemyPositions[i][0];
                enemyProjectilePositions[i][1] = enemyPositions[i][1];
                float angle = ((rand() % 360) * 3.14159f) / 180.0f; // Random angle in radians
                enemyProjectileDirections[i][0] = cos(angle) * 0.02f; // Speed in x direction
                enemyProjectileDirections[i][1] = sin(angle) * 0.02f; // Speed in y direction
                enemyProjectileActive[i] = true;
            }
            }

            // Update and draw active projectiles
            if (enemyProjectileActive[i]) {
            enemyProjectilePositions[i][0] += enemyProjectileDirections[i][0];
            enemyProjectilePositions[i][1] += enemyProjectileDirections[i][1];

            // Draw projectile with smaller size
            glUniform2f(glGetUniformLocation(shader, "offset"), enemyProjectilePositions[i][0], enemyProjectilePositions[i][1]);
            glBindTexture(GL_TEXTURE_2D, axeTexture);
            glBindVertexArray(playerVAO); // Reusing player VAO
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Deactivate projectile if it goes out of bounds
            if (enemyProjectilePositions[i][0] < -1.0f || enemyProjectilePositions[i][0] > 1.0f ||
                enemyProjectilePositions[i][1] < -1.0f || enemyProjectilePositions[i][1] > 1.0f) {
                enemyProjectileActive[i] = false;
            }

            // Check for collision with player
            if (abs(enemyProjectilePositions[i][0] - playerX) < 0.03f && abs(enemyProjectilePositions[i][1] - playerY) < 0.03f) { // Smaller collision box
                std::cout << "Game Over" << std::endl << "Enemies Killed: " << score << std::endl;
                glfwSetWindowShouldClose(window, true); // Close the window
                break;
            }
            }
        }
        // Check for collisions between player and enemies
        for (int i = 0; i < 3; i++) {
            if (abs(playerX - enemyPositions[i][0]) < 0.1f && abs(playerY - enemyPositions[i][1]) < 0.1f) {
                std::cout << "Game Over" << std::endl <<"Enemies Killed: " << score << std::endl;
                glfwSetWindowShouldClose(window, true); // Close the window
                break;
            }
        }
        // Check for collisions between projectile and enemies
        if (projectileActive||projact) {
            for (int i = 0; i < 3; i++) {
            if (abs(projectileX - enemyPositions[i][0]) < 0.1f && abs(projectileY - enemyPositions[i][1]) < 0.1f) {
                score++;
                // Respawn enemy at a random position, ensuring it's not where the player is
                do {
                enemyPositions[i][0] = ((rand() % 200) - 100) / 100.0f; // Random value between -1.0 and 1.0
                enemyPositions[i][1] = ((rand() % 200) - 100) / 100.0f; // Random value between -1.0 and 1.0
                } while (abs(enemyPositions[i][0] - playerX) < 0.2f && abs(enemyPositions[i][1] - playerY) < 0.2f);
                
                projectileActive = false; // Deactivate projectile
                break;
            }
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
