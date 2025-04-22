#include <GL/glut.h>
#include <cstring>
#include <cstdio>
#include <cmath>

// Game State
float dinoY = 0.0f;
bool isJumping = false;
float velocity = 0.0f;
float gravity = -0.5f;
float cactusX = 800.0f;
const float groundY = 90.0f;
bool isGameOver = false;
int score = 0;
float cloudX = 800.0f;
float legOffset = 0.0f;
bool legDirection = true;

// Background scrolling
float buildingsX = 0.0f;

// Colors
struct Color { float r, g, b; };
Color skyTop = { 0.6f, 0.8f, 1.0f };
Color skyBottom = { 0.9f, 0.95f, 1.0f };
Color sunColor = { 1.0f, 0.9f, 0.4f };

// Building structure
struct Building {
    float x;
    float width;
    float height;
    Color color;
};

Building buildings[3] = {
    {0, 200, 150, {0.5f, 0.4f, 0.4f}},
    {400, 250, 180, {0.4f, 0.5f, 0.4f}},
    {700, 180, 200, {0.4f, 0.4f, 0.5f}}
};

void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (int i = 0; i < strlen(text); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void drawGradientBackground() {
    glBegin(GL_QUADS);
    glColor3f(skyTop.r, skyTop.g, skyTop.b);
    glVertex2f(0, 600);
    glVertex2f(800, 600);
    glColor3f(skyBottom.r, skyBottom.g, skyBottom.b);
    glVertex2f(800, groundY);
    glVertex2f(0, groundY);
    glEnd();
}

void drawSun() {
    glColor3f(sunColor.r, sunColor.g, sunColor.b);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180;
        glVertex2f(700 + 30 * cos(angle), 500 + 30 * sin(angle));
    }
    glEnd();
}

void drawCloud(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180;
        glVertex2f(x + 20 * cos(angle), y + 10 * sin(angle));
    }
    glEnd();
}

void drawMovingClouds() {
    glPushMatrix();
    glTranslatef(cloudX, 500, 0);
    drawCloud(0, 0);
    drawCloud(40, 10);
    drawCloud(20, -10);
    glPopMatrix();
}

void drawBuilding(Building b) {
    float xPos = b.x + buildingsX;

    // Main building
    glColor3f(b.color.r, b.color.g, b.color.b);
    glBegin(GL_QUADS);
    glVertex2f(xPos, groundY);
    glVertex2f(xPos + b.width, groundY);
    glVertex2f(xPos + b.width, groundY + b.height);
    glVertex2f(xPos, groundY + b.height);
    glEnd();

    // Windows
    glColor3f(0.9f, 0.9f, 0.7f);
    int windowRows = b.height / 30;
    int windowCols = b.width / 25;

    for (int row = 0; row < windowRows; row++) {
        for (int col = 0; col < windowCols; col++) {
            if ((row + col) % 3 != 0) { // Simple window pattern
                glBegin(GL_QUADS);
                glVertex2f(xPos + 5 + col * 25, groundY + 5 + row * 30);
                glVertex2f(xPos + 15 + col * 25, groundY + 5 + row * 30);
                glVertex2f(xPos + 15 + col * 25, groundY + 20 + row * 30);
                glVertex2f(xPos + 5 + col * 25, groundY + 20 + row * 30);
                glEnd();
            }
        }
    }

    // Roof
    glColor3f(b.color.r * 0.8f, b.color.g * 0.8f, b.color.b * 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(xPos, groundY + b.height);
    glVertex2f(xPos + b.width, groundY + b.height);
    glVertex2f(xPos + b.width, groundY + b.height + 5);
    glVertex2f(xPos, groundY + b.height + 5);
    glEnd();
}

void drawBuildings() {
    for (int i = 0; i < 3; i++) {
        drawBuilding(buildings[i]);

        // Wrap around when buildings go off screen
        if (buildings[i].x + buildingsX < -buildings[i].width) {
            buildings[i].x = 800 + (rand() % 200);
        }
    }
}

void drawDetailedGround() {
    // Base ground
    glColor3f(0.4f, 0.3f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0, groundY);
    glVertex2f(800, groundY);
    glVertex2f(800, groundY - 15);
    glVertex2f(0, groundY - 15);
    glEnd();

    // Ground texture
    glColor3f(0.35f, 0.25f, 0.15f);
    glPointSize(3);
    glBegin(GL_POINTS);
    for (int i = 0; i < 200; i++) {
        glVertex2f(rand() % 800, groundY - 5);
    }
    glEnd();
}

void drawAnimatedDino() {
    // Body gradient
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.2f);
    glVertex2f(100, groundY + dinoY);
    glVertex2f(130, groundY + dinoY);
    glColor3f(0.4f, 0.4f, 0.4f);
    glVertex2f(130, groundY + 40 + dinoY);
    glVertex2f(100, groundY + 40 + dinoY);
    glEnd();

    // Head
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.3f, 0.3f);
    glVertex2f(130, groundY + 30 + dinoY);
    glVertex2f(150, groundY + 30 + dinoY);
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex2f(150, groundY + 50 + dinoY);
    glVertex2f(130, groundY + 50 + dinoY);
    glEnd();

    // Animated legs
    float legMove = legDirection ? legOffset : -legOffset;
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(105 + legMove, groundY + dinoY);
    glVertex2f(110 + legMove, groundY + dinoY);
    glVertex2f(110 + legMove, groundY - 10 + dinoY);
    glVertex2f(105 + legMove, groundY - 10 + dinoY);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(120 - legMove, groundY + dinoY);
    glVertex2f(125 - legMove, groundY + dinoY);
    glVertex2f(125 - legMove, groundY - 10 + dinoY);
    glVertex2f(120 - legMove, groundY - 10 + dinoY);
    glEnd();

    // Eye
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180;
        glVertex2f(145 + 2 * cos(angle), groundY + 45 + dinoY + 2 * sin(angle));
    }
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180;
        glVertex2f(145 + cos(angle), groundY + 45 + dinoY + sin(angle));
    }
    glEnd();

    // Tail
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_TRIANGLES);
    glVertex2f(100, groundY + 20 + dinoY);
    glVertex2f(90, groundY + 15 + dinoY);
    glVertex2f(100, groundY + 10 + dinoY);
    glEnd();
}

void drawCactusWithDetails() {
    // Main body
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(cactusX, groundY);
    glVertex2f(cactusX + 20, groundY);
    glColor3f(0.0f, 0.7f, 0.0f);
    glVertex2f(cactusX + 20, groundY + 60);
    glVertex2f(cactusX, groundY + 60);
    glEnd();

    // Spikes
    glColor3f(0.0f, 0.3f, 0.0f);
    glLineWidth(2);
    for (int i = 0; i < 3; i++) {
        glBegin(GL_LINES);
        glVertex2f(cactusX - 5 + i * 10, groundY + 20 + i * 15);
        glVertex2f(cactusX - 8 + i * 10, groundY + 25 + i * 15);
        glEnd();
    }

    // Side arms
    glBegin(GL_QUADS);
    glVertex2f(cactusX - 10, groundY + 30);
    glVertex2f(cactusX, groundY + 30);
    glVertex2f(cactusX, groundY + 40);
    glVertex2f(cactusX - 10, groundY + 40);
    glEnd();
}

void checkCollision() {
    float dinoLeft = 100;
    float dinoRight = 150;
    float dinoBottom = groundY + dinoY;
    float dinoTop = groundY + 50 + dinoY;

    float cactusLeft = cactusX - 10;
    float cactusRight = cactusX + 30;
    float cactusBottom = groundY;
    float cactusTop = groundY + 60;

    if (dinoRight > cactusLeft && dinoLeft < cactusRight &&
        dinoTop > cactusBottom && dinoBottom < cactusTop) {
        isGameOver = true;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawGradientBackground();
    drawSun();
    drawMovingClouds();
    drawBuildings();
    drawDetailedGround();
    drawAnimatedDino();
    drawCactusWithDetails();

    // Score display
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(600, 550);
    glVertex2f(790, 550);
    glVertex2f(790, 590);
    glVertex2f(600, 590);
    glEnd();

    char scoreText[50];
    sprintf(scoreText, "SCORE: %d", score);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(scoreText, 610, 565);

    if (isGameOver) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 600);
        glVertex2f(0, 600);
        glEnd();

        glColor3f(1.0f, 0.3f, 0.3f);
        drawText("GAME OVER", 320, 320);
        drawText("Press R to Restart", 300, 280);
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!isGameOver) {
        // Cloud animation
        cloudX -= 1.5f;
        if (cloudX < -200) cloudX = 800.0f;

        // Building movement
        buildingsX -= 1.0f;

        // Leg animation
        if (legDirection) {
            legOffset += 0.5f;
            if (legOffset > 3.0f) legDirection = false;
        }
        else {
            legOffset -= 0.5f;
            if (legOffset < -3.0f) legDirection = true;
        }

        // Jump physics
        if (isJumping) {
            dinoY += velocity;
            velocity += gravity;
            if (dinoY <= 0.0f) {
                dinoY = 0.0f;
                isJumping = false;
            }
        }

        // Move cactus
        cactusX -= 5.0f;
        if (cactusX < -50) {
            cactusX = 800.0f;
            score += 10;
        }

        score += 1;
        checkCollision();
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void handleKeypress(unsigned char key, int x, int y) {
    if (key == ' ' && !isJumping && !isGameOver) {
        isJumping = true;
        velocity = 10.0f;
    }

    if (key == 'r' || key == 'R') {
        isGameOver = false;
        cactusX = 800.0f;
        dinoY = 0.0f;
        isJumping = false;
        score = 0;
        velocity = 0.0f;
        buildingsX = 0.0f;
    }
}

void init() {
    glClearColor(1, 1, 1, 1);
    gluOrtho2D(0, 800, 0, 600);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("T-Rex Dino Game");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeypress);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}