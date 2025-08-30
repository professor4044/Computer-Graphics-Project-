#include <GL/glut.h>
#include <cmath>
#include <cstdlib> // For rand() and srand()
#include <iostream>
#include <ctime>    // For time()
#include <vector>    // For dynamic storage if needed, though not strictly required for this fixed-size example
#ifndef M_PI
#define M_PI 3.14159265358979323846f // Use float suffix for M_PI
#endif


// --- Global Variables for Main Scene (Honey Transformation Bee) ---
const float PI = 3.1416f; // Re-defined as it was in the original code

float cloudXPositions_main[3] = {-0.7f, 0.4f, -0.3f};
bool startBeeAnimation_main = false;
float beeX_main = 0.0f, beeY_main = -0.45f;
int beeHoverTime_main = 0;
int beeWingFlapState_main = 0;

// --- Global Variables for Second Animation Scene (Bee and Hive) ---
// Bee animation variables
bool startBeeAnimation_hive = false;
float beeX_hive = -0.5f, beeY_hive = 0.2f; // Initial bee position
float beeTargetX_hive, beeTargetY_hive;        // Current target for the bee
enum BeeState_hive {
    IDLE_HIVE,
    FLYING_TO_HONEY_HIVE,
    HOVERING_AT_HONEY_HIVE,
    FLYING_TO_HIVE_HIVE,
    LANDED_AT_HIVE_HIVE // This state will now mean the bee is "inside" the hive
};
BeeState_hive currentBeeState_hive = IDLE_HIVE;
int beeHoverTimer_hive = 0; // Milliseconds for hovering
const int HOVER_DURATION_HIVE = 1000; // 1 second hover time
float beeSpeed_hive = 0.008f; // Adjusted bee speed for flight
int beeWingFlapState_hive = 0; // For wing animation (0 or 1)

// Variables for controlling the "entering" effect
float entranceProgress_hive = 0.0f; // 0.0 (outside) to 1.0 (fully inside/invisible)
const float ENTER_DISTANCE_THRESHOLD_HIVE = 0.05f; // Distance from hive entrance to start "entering" effect
const float ENTER_MIN_SCALE_HIVE = 0.1f; // Minimum scale when "inside" hive
const float ENTER_MIN_ALPHA_HIVE = 0.0f; // Minimum alpha (transparency) when "inside" hive

// Target coordinates for honey and hive
const float HONEY_TARGET_X_HIVE = 0.2f;
const float HONEY_TARGET_Y_HIVE = 0.4f;
const float HIVE_ATTACH_X_HIVE = 0.6f + (-0.2f); // Hive attachment point X (from drawLeafyCanopyWithBranch)
const float HIVE_ATTACH_Y_HIVE = 0.28f + 0.1f - 0.11f; // Hive attachment point Y (from drawLeafyCanopyWithBranch)
const float HIVE_ENTRANCE_OFFSET_X_HIVE = 0.0f; // Center of hive entrance relative to hive origin
const float HIVE_ENTRANCE_OFFSET_Y_HIVE = -0.03f; // Center of hive entrance relative to hive origin

// Calculate the actual hive landing target for the bee
const float HIVE_LANDING_X_HIVE = HIVE_ATTACH_X_HIVE + HIVE_ENTRANCE_OFFSET_X_HIVE;
const float HIVE_LANDING_Y_HIVE = HIVE_ATTACH_Y_HIVE + HIVE_ENTRANCE_OFFSET_Y_HIVE; // Directly at the entrance center

// --- GLOBAL VARIABLES FOR STATIC LEAVES ---
struct Leaf {
    float x;
    float y;
    float radius;
    float red;
    float green;
    float blue;
};

const int NUM_CLUSTER_CENTERS = 12;
const int LEAVES_PER_CLUSTER_MIN = 12;
const int LEAVES_PER_CLUSTER_MAX = 15;
const int TOTAL_LEAVES = NUM_CLUSTER_CENTERS * LEAVES_PER_CLUSTER_MAX; // Max possible leaves
Leaf treeLeaves[TOTAL_LEAVES];
int actualNumLeaves = 0; // To keep track of how many leaves are actually generated

Leaf coverLeaves[5]; // For leaves covering the branch

// --- Scene Management ---
bool showInformation = true;
bool showInstruction = false;  // ✅ Add this
bool showBeeAnimationScene = false; // Controls which animation scene is active

//--- Global Variable for Shajid---
float truckX = -200.0f;
float wheelRotation = 0.0f;
float cloudX1 = 100.0f, cloudX2 = 300.0f, cloudX3 = 500.0f;
bool isMoving = false;
bool showTruckScene = false;  // NEW SCENE FLAG
bool truckMoving = false;

void drawText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

//--- Global Variable for Jubo
float jarX = 0.5f;   // Initial X position of the jar
float jarY = -0.325f;    // Initial Y position of the jar
int animate = 1;       // Control whether to animate
bool showJarScene = false;  // new global flag

// --- Common Drawing Utilities ---
void drawCircle(float cx, float cy, float r, int segments, float red, float green, float blue, float alpha = 1.0f) {
    glColor4f(red, green, blue, alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy); // Center of the circle (common vertex for all triangles in the fan)
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * static_cast<float>(i) / segments;
        float x_vertex = r * static_cast<float>(cos(theta));
        float y_vertex = r * static_cast<float>(sin(theta));
        glVertex2f(cx + x_vertex, cy + y_vertex); // Explicitly define each vertex on the circumference
    }
    glEnd();
}

void renderBitmapString(float x, float y, float z, void *font, const char *string) {
    const char *c;
    glRasterPos3f(x, y, z);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// --- Information Page Functions ---
void information(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(1.0, 0.0, 0.0);
    renderBitmapString(-0.5f, 0.91f, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "The Honey Trail: Animation of a Bee’s Voyage");
    renderBitmapString(-0.3, 0.84f, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "Computer Graphics [G]");
    renderBitmapString(-0.88, 0.78f, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "Group Member :");

    glLineWidth(4);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES); glVertex2f(-0.88, 0.75); glVertex2f(0.98, 0.75); glEnd();

    renderBitmapString(-0.7, 0.69, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "ID");
    glBegin(GL_LINES); glVertex2f(-0.82, 0.66); glVertex2f(-0.52, 0.66); glEnd();

    renderBitmapString(-0.05, 0.69, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Name");
    glBegin(GL_LINES); glVertex2f(-0.24, 0.66); glVertex2f(0.24, 0.66); glEnd();

    renderBitmapString(0.60, 0.69, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Contribution");
    glBegin(GL_LINES); glVertex2f(0.54, 0.66); glVertex2f(0.96, 0.66); glEnd();

    renderBitmapString(-0.84, 0.56, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "22-48468-3");
    renderBitmapString(-0.22, 0.56, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "MD. JAMIR SHEKH");
    renderBitmapString(0.74, 0.56, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "20");

    renderBitmapString(-0.84, 0.46, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "23-50157-1");
    renderBitmapString(-0.22, 0.46, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "MD. JONAYED HOSSAIN");
    renderBitmapString(0.74, 0.46, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "20");

    renderBitmapString(-0.84, 0.36, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "22-49344-3");
    renderBitmapString(-0.22, 0.36, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "FAHAD");
    renderBitmapString(0.74, 0.36, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "20");

    renderBitmapString(-0.84, 0.26, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "22-48624-3");
    renderBitmapString(-0.22, 0.26, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "MD. SHAJID HASAN HRIDOY");
    renderBitmapString(0.74, 0.26, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "20");

    renderBitmapString(-0.84, 0.16, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "22-47692-2");
    renderBitmapString(-0.22, 0.16, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "MD KAMRUZZAMN JUBO");
    renderBitmapString(0.74, 0.16, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "20");

    glBegin(GL_LINES); glVertex2f(-0.88, 0.10); glVertex2f(0.88, 0.10); glEnd();

    renderBitmapString(-0.1, 0.02, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Supervised by :");
    renderBitmapString(-0.24, -0.08, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "MAHFUJUR RAHMAN");
    renderBitmapString(-0.16, -0.16, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Assistant Professor");
    renderBitmapString(-0.30, -0.24, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Department of Computer Science");
    renderBitmapString(-0.32, -0.32, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "Faculty of Science and Technology");
    renderBitmapString(-0.48, -0.40, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "American International University-Bangladesh(AIUB)");

    glFlush();
}

//---Manual Instruction Page ---
void instructionPage() {
    const char* labels[] = {
        "1-5", "B", "C", "T", "J", "I", "M", "ESC"
    };

    const char* descriptions[] = {
        ": Switch between scenes",
        ": Bee transformation from flower",
        ": Bee transformation to Hive",
        ": Truck transformation",
        ": Honey Jar transformation",
        ": Show information screen",
        ": Show instruction manual",
        ": Exit the application"
    };

    glColor3f(0, 0, 0);
    float y = 0.8f;
    float gap = 0.08f;
    float startY = 0.8f;

    for (int i = 0; i < 8; ++i) {
        glRasterPos2f(-0.9f, startY - i * 0.1f);
        for (int j = 0; labels[i][j]; ++j)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, labels[i][j]);

        glRasterPos2f(-0.6f, startY - i * 0.1f); // aligned second column
        for (int j = 0; descriptions[i][j]; ++j)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, descriptions[i][j]);
    }

}

// --- Main Animation Scene (Original Honey Bee) Functions ---
void drawSky_main() {
    glColor3f(0.53f, 0.81f, 0.92f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
}

void drawCloud_main(float x, float y) {
    drawCircle(x, y, 0.1f, 50, 1.0f, 1.0f, 1.0f);
    drawCircle(x + 0.15f, y + 0.03f, 0.08f, 50, 1.0f, 1.0f, 1.0f);
    drawCircle(x - 0.15f, y + 0.03f, 0.08f, 50, 1.0f, 1.0f, 1.0f);
    drawCircle(x + 0.08f, y - 0.05f, 0.07f, 50, 1.0f, 1.0f, 1.0f);
    drawCircle(x - 0.08f, y - 0.05f, 0.07f, 50, 1.0f, 1.0f, 1.0f);
}

void drawGrass_main() {
    glColor3f(0.0f, 0.8f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 0.0f); glVertex2f(-1.0f, 0.0f);
    glEnd();
}

void drawMountain_main(float x, float y, float width, float height) {
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - width / 2, y);
    glVertex2f(x + width / 2, y);
    glVertex2f(x, y + height);
    glEnd();
}

void drawTree_main(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_QUADS);
    glVertex2f(-0.03f, -0.2f); glVertex2f(0.03f, -0.2f);
    glVertex2f(0.03f, 0.1f); glVertex2f(-0.03f, 0.1f);
    glEnd();
    glColor3f(0.0f, 0.5f, 0.0f);
    drawCircle(0.0f, 0.15f, 0.1f, 50, 0.0f, 0.5f, 0.0f);
    drawCircle(-0.07f, 0.1f, 0.09f, 50, 0.0f, 0.5f, 0.0f);
    drawCircle(0.07f, 0.1f, 0.09f, 50, 0.0f, 0.5f, 0.0f);
    drawCircle(0.0f, 0.2f, 0.1f, 50, 0.0f, 0.5f, 0.0f);
    glPopMatrix();
}

void drawFlower_main(float x, float y, float petalSize, float centerSize, float stemHeight, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glColor3f(0.0f, 0.6f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.01f, -stemHeight); glVertex2f(0.01f, -stemHeight);
    glVertex2f(0.01f, 0.02f); glVertex2f(-0.01f, 0.02f);
    glEnd();
    glColor3f(r, g, b);
    drawCircle(0.0f, 0.08f, petalSize, 50, r, g, b);
    drawCircle(0.08f, 0.04f, petalSize, 50, r, g, b);
    drawCircle(0.08f, -0.04f, petalSize, 50, r, g, b);
    drawCircle(0.0f, -0.08f, petalSize, 50, r, g, b);
    drawCircle(-0.08f, -0.04f, petalSize, 50, r, g, b);
    drawCircle(-0.08f, 0.04f, petalSize, 50, r, g, b);
    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircle(0.0f, 0.0f, centerSize, 50, 1.0f, 1.0f, 0.0f);
    glPopMatrix();
}

void drawRealisticBee_main(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    float body_seg_r = 0.025f;
    drawCircle(-0.04f, 0.0f, body_seg_r, 50, 1.0f, 0.85f, 0.0f);
    drawCircle(-0.02f, 0.0f, body_seg_r, 50, 0.0f, 0.0f, 0.0f);
    drawCircle(0.0f, 0.0f, body_seg_r, 50, 1.0f, 0.85f, 0.0f);
    drawCircle(0.02f, 0.0f, body_seg_r, 50, 0.0f, 0.0f, 0.0f);
    drawCircle(0.04f, 0.0f, body_seg_r, 50, 1.0f, 0.85f, 0.0f);
    drawCircle(0.065f, 0.01f, 0.02f, 50, 0.0f, 0.0f, 0.0f);
    drawCircle(0.075f, 0.015f, 0.005f, 20, 1.0f, 1.0f, 1.0f);
    drawCircle(0.076f, 0.016f, 0.002f, 10, 0.0f, 0.0f, 0.0f);
    glColor3f(0.3f, 0.15f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.06f, 0.025f); glVertex2f(0.075f, 0.04f);
    glVertex2f(0.055f, 0.02f); glVertex2f(0.07f, 0.035f);
    glEnd();
    glLineWidth(1.0f);
    glColor3f(0.3f, 0.15f, 0.05f);
    glBegin(GL_LINES);
    glVertex2f(0.03f, -0.02f); glVertex2f(0.04f, -0.04f);
    glVertex2f(0.04f, -0.04f); glVertex2f(0.03f, -0.05f);
    glVertex2f(0.0f, -0.025f); glVertex2f(0.0f, -0.045f);
    glVertex2f(0.0f, -0.045f); glVertex2f(-0.01f, -0.055f);
    glVertex2f(-0.03f, -0.02f); glVertex2f(-0.04f, -0.04f);
    glVertex2f(-0.04f, -0.04f); glVertex2f(-0.05f, -0.05f);
    glEnd();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (beeWingFlapState_main == 0) {
        drawCircle(-0.005f, 0.03f, 0.03f, 50, 1.0f, 1.0f, 1.0f, 0.6f);
        drawCircle(-0.025f, 0.025f, 0.025f, 50, 1.0f, 1.0f, 1.0f, 0.6f);
    } else {
        drawCircle(-0.005f, 0.035f, 0.028f, 50, 1.0f, 1.0f, 1.0f, 0.6f);
        drawCircle(-0.027f, 0.03f, 0.023f, 50, 1.0f, 1.0f, 1.0f, 0.6f);
    }

    glDisable(GL_BLEND);

    glColor3f(1.0f, 0.65f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.07f, -0.01f);
    int honey_segments = 20;
    float honey_base_r = 0.008f;
    float honey_base_cx = 0.07f;
    float honey_base_cy = -0.02f;
    for (int i = 0; i <= honey_segments; i++) {
        float theta = M_PI + (M_PI * 0.9f) * (float)i / (float)honey_segments;
        glVertex2f(honey_base_cx + honey_base_r * cos(theta), honey_base_cy + honey_base_r * sin(theta));
    }
    glEnd();
    glPopMatrix();
}

void updateClouds_main(int value) {
    for (int i = 0; i < 3; ++i) {
        cloudXPositions_main[i] += 0.001f;
        if (cloudXPositions_main[i] > 1.2f)
            cloudXPositions_main[i] = -1.2f;
    }
    if (!showInformation && !showBeeAnimationScene) { // Only redisplay if this scene is active
        glutPostRedisplay();
    }
    glutTimerFunc(30, updateClouds_main, 0);
}

void updateBee_main(int value) {
    if (startBeeAnimation_main) {
        if (beeHoverTime_main < 600)
            beeHoverTime_main += 30;
        else {
            beeX_main += 0.008f;
            beeY_main += 0.004f;
        }

        beeWingFlapState_main = (beeWingFlapState_main + 1) % 2;

        if (beeX_main > 1.2f || beeY_main > 1.2f) {
            startBeeAnimation_main = false;
            beeX_main = 0.0f;
            beeY_main = -0.45f;
            beeHoverTime_main = 0;
        }

        if (!showInformation && !showBeeAnimationScene) { // Only redisplay if this scene is active
             glutPostRedisplay();
        }
        glutTimerFunc(100, updateBee_main, 0);
    }
}

void drawSun_main() {
    drawCircle(0.7f, 0.7f, 0.1f, 50, 1.0f, 1.0f, 0.0f);
}

void displayMainAnimationScene() {
    drawSky_main();
    drawGrass_main();
    drawSun_main();
    for (int i = 0; i < 3; ++i) {
        drawCloud_main(cloudXPositions_main[i], (i == 0 ? 0.6f : (i == 1 ? 0.7f : 0.8f)));
    }

    drawMountain_main(-0.7f, 0.0f, 0.3f, 0.3f);
    drawMountain_main(-0.2f, 0.0f, 0.375f, 0.375f);
    drawMountain_main(0.4f, 0.0f, 0.225f, 0.2625f);

    drawTree_main(-0.6f, -0.1f);
    drawTree_main(0.7f, -0.25f);

    drawFlower_main(-0.6f, -0.6f, 0.05f, 0.025f, 0.2f, 1.0f, 0.0f, 0.0f);
    drawFlower_main(0.0f, -0.5f, 0.09f, 0.04f, 0.25f, 0.7f, 0.0f, 1.0f);
    drawFlower_main(0.6f, -0.6f, 0.05f, 0.025f, 0.2f, 1.0f, 0.5f, 0.0f);

    drawRealisticBee_main(beeX_main, beeY_main);
}


// --- Second Animation Scene (Bee and Hive) Functions ---
void drawRoundedRect(float x, float y, float width, float height, float radius, float r, float g, float b, float alpha = 1.0f) {
    glColor4f(r, g, b, alpha);
    int segments = 20; // For quarter circles

    // Main rectangular body parts
    glBegin(GL_QUADS);
        // Center rectangle
        glVertex2f(x + radius, y);
        glVertex2f(x + width - radius, y);
        glVertex2f(x + width - radius, y + height);
        glVertex2f(x + radius, y + height);

        // Left rectangle
        glVertex2f(x, y + radius);
        glVertex2f(x + radius, y + radius);
        glVertex2f(x + radius, y + height - radius);
        glVertex2f(x, y + height - radius);

        // Right rectangle
        glVertex2f(x + width - radius, y + radius);
        glVertex2f(x + width, y + radius);
        glVertex2f(x + width, y + height - radius);
        glVertex2f(x + width - radius, y + height - radius);
    glEnd();

    // Corner arcs using GL_TRIANGLE_FAN (explicit vertices)
    // Bottom-left corner
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + radius, y + radius); // Center of the arc
    for (int i = 0; i <= segments; i++) {
        float theta = M_PI + (M_PI / 2) * (float)i / (float)segments;
        float x_vertex = radius * cos(theta);
        float y_vertex = radius * sin(theta);
        glVertex2f(x + radius + x_vertex, y + radius + y_vertex);
    }
    glEnd();

    // Bottom-right corner
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + width - radius, y + radius); // Center of the arc
    for (int i = 0; i <= segments; i++) {
        float theta = (3 * M_PI / 2) + (M_PI / 2) * (float)i / (float)segments;
        float x_vertex = radius * cos(theta);
        float y_vertex = radius * sin(theta);
        glVertex2f(x + width - radius + x_vertex, y + radius + y_vertex);
    }
    glEnd();

    // Top-left corner
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + radius, y + height - radius); // Center of the arc
    for (int i = 0; i <= segments; i++) {
        float theta = (M_PI / 2) + (M_PI / 2) * (float)i / (float)segments;
        float x_vertex = radius * cos(theta);
        float y_vertex = radius * sin(theta);
        glVertex2f(x + radius + x_vertex, y + height - radius + y_vertex);
    }
    glEnd();

    // Top-right corner
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x + width - radius, y + height - radius); // Center of the arc
    for (int i = 0; i <= segments; i++) {
        float theta = (M_PI / 2) * (float)i / (float)segments;
        float x_vertex = radius * cos(theta);
        float y_vertex = radius * sin(theta);
        glVertex2f(x + width - radius + x_vertex, y + height - radius + y_vertex);
    }
    glEnd();
}

// Background
void drawSky_hive() {
    glColor3f(0.53f, 0.81f, 0.92f); // Light blue
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
    glEnd();
}

// Function to draw the sea
void drawSea_hive() {
    glColor3f(0.1f, 0.4f, 0.7f); // Deep blue for sea
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, -0.7f); // Sea level
        glVertex2f(-1.0f, -0.7f);
    glEnd();

    // Add subtle waves to the sea
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.3f, 0.6f, 0.9f, 0.2f); // Lighter blue with transparency
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.8f);
        glVertex2f(1.0f, -0.8f);
        glVertex2f(1.0f, -0.7f);
        glVertex2f(-1.0f, -0.7f);
    glEnd();
    glColor4f(0.3f, 0.6f, 0.9f, 0.1f); // Even lighter blue
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.9f);
        glVertex2f(1.0f, -0.9f);
        glVertex2f(1.0f, -0.8f);
        glVertex2f(-1.0f, -0.8f);
    glEnd();
    glDisable(GL_BLEND);
}

// Function to draw the beach
void drawBeach_hive() {
    glColor3f(0.96f, 0.87f, 0.70f); // Sandy color for beach
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.7f); // Beach starts where sea ends
        glVertex2f(1.0f, -0.7f);
        glVertex2f(1.0f, -0.5f); // Beach ends below grass
        glVertex2f(-1.0f, -0.5f);
    glEnd();

    // Add some texture to the beach
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < 50; ++i) {
        float x = -1.0f + static_cast<float>(rand()) / RAND_MAX * 2.0f; // Random x
        float y = -0.7f + static_cast<float>(rand()) / RAND_MAX * 0.2f;  // Random y within beach band
        float size = 0.005f + static_cast<float>(rand()) / RAND_MAX * 0.005f; // Vary size
        float shade = 0.8f + static_cast<float>(rand()) / RAND_MAX * 0.2f; // Vary shade
        drawCircle(x, y, size, 10, shade, shade * 0.9f, shade * 0.7f, 0.5f); // Small sandy dots
    }
    glDisable(GL_BLEND);
}

// Mountain
void drawMountain_hive() {
    glColor3f(0.4f, 0.4f, 0.4f); // dark grey
    glBegin(GL_TRIANGLES);
        // Adjusted Y-coordinates to start from the top of the beach
        glVertex2f(-1.0f, -0.5f);
        glVertex2f(-0.5f, 0.0f);
        glVertex2f(0.0f, -0.5f);
        glVertex2f(-0.2f, -0.5f);
        glVertex2f(0.3f, 0.1f);
        glVertex2f(0.8f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.9f, -0.1f);
        glVertex2f(1.2f, -0.5f); // Partially off-screen
    glEnd();
}

void drawCloud_hive(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f); // White
    drawCircle(x, y, 0.1f, 50, 1.0f, 1.0f, 1.0f);
    drawCircle(x + 0.1f, y + 0.05f, 0.08f, 50, 1.0f, 1.0f, 1.0f);
    drawCircle(x - 0.1f, y + 0.05f, 0.09f, 50, 1.0f, 1.0f, 1.0f);
    drawCircle(x, y + 0.08f, 0.07f, 50, 1.0f, 1.0f, 1.0f);
}

// New function to draw a cloud shadow
void drawCloudShadow_hive(float x, float y, float sunX, float sunY) {
    // Calculate shadow offset based on sun's position relative to the cloud
    float shadowOffsetX = (x - sunX) * 0.1f; // Adjust multiplier for desired offset
    // The shadow should appear on the grass, which is between Y = -0.5f and Y = -0.2f
    // We'll project it to the base of the grass or slightly above
    float grassLevel = -0.5f; // The bottom Y-coordinate of the grass
    float shadowOffsetY = (y - sunY) * 0.05f + grassLevel; // Project onto grassLevel

    // Ensure shadow appears only when cloud is above the ground level (grass)
    // And that the projected shadow's Y coordinate is within the grass area
    if (y > grassLevel && shadowOffsetY >= -0.5f && shadowOffsetY <= -0.2f) { // Added check for shadowOffsetY range
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.2f); // Dark gray with transparency

        // The shadow should be stretched and slightly offset
        float shadowScaleX = 1.5f; // Stretch horizontally
        float shadowScaleY = 0.5f; // Flatten vertically

        drawCircle(x + shadowOffsetX, shadowOffsetY, 0.1f * shadowScaleX, 50, 0.0f, 0.0f, 0.0f, 0.2f);
        drawCircle(x + 0.1f + shadowOffsetX, shadowOffsetY + 0.05f, 0.08f * shadowScaleX, 50, 0.0f, 0.0f, 0.0f, 0.2f);
        drawCircle(x - 0.1f + shadowOffsetX, shadowOffsetY + 0.05f, 0.09f * shadowScaleX, 50, 0.0f, 0.0f, 0.0f, 0.2f);
        drawCircle(x + shadowOffsetX, shadowOffsetY + 0.08f, 0.07f * shadowScaleX, 50, 0.0f, 0.0f, 0.0f, 0.2f);

        glDisable(GL_BLEND);
    }
}


void drawSun_hive() {
    float cx = 0.7f;
    float cy = 0.85f; // Moved sun higher
    float baseRadius = 0.1f;

    // Core of the sun (bright yellow)
    drawCircle(cx, cy, baseRadius, 100, 1.0f, 1.0f, 0.0f);

    // Multiple layers of glow for a softer, more realistic effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Inner glow (orange-yellow, more opaque)
    drawCircle(cx, cy, baseRadius * 1.2f, 100, 1.0f, 0.8f, 0.0f, 0.5f);
    // Outer glow (lighter orange, less opaque)
    drawCircle(cx, cy, baseRadius * 1.4f, 100, 1.0f, 0.7f, 0.0f, 0.3f);
    // Faintest outermost glow (pale yellow, very transparent)
    drawCircle(cx, cy, baseRadius * 1.6f, 100, 1.0f, 0.9f, 0.5f, 0.15f);

    glDisable(GL_BLEND);

    // Sun rays (more dynamic and varied)
    glColor3f(1.0f, 1.0f, 0.4f); // Lighter yellow for rays
    glLineWidth(1.5f);
    int numRays = 24; // More rays for a fuller look
    float innerRadius = baseRadius * 1.1f;
    float outerRadiusMin = baseRadius * 1.5f;
    float outerRadiusMax = baseRadius * 2.0f;

    glBegin(GL_LINES);
    for (int i = 0; i < numRays; ++i) {
        float angle = 2.0f * M_PI * static_cast<float>(i) / numRays;
        // Vary ray length slightly
        float currentOuterRadius = outerRadiusMin + (outerRadiusMax - outerRadiusMin) * (static_cast<float>(rand()) / RAND_MAX);

        float x1 = cx + innerRadius * std::cos(angle);
        float y1 = cy + innerRadius * std::sin(angle);
        float x2 = cx + currentOuterRadius * std::cos(angle);
        float y2 = cy + currentOuterRadius * std::sin(angle);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    }
    glEnd();
    glLineWidth(1.0f); // Reset line width
}

// Grass
void drawGrass_hive() {
    glColor3f(0.0f, 0.8f, 0.0f); // Green
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.5f); // Starts from beach top
        glVertex2f(1.0f, -0.5f);
        glVertex2f(1.0f, -0.2f); // Ends at tree trunk base
        glVertex2f(-1.0f, -0.2f);
    glEnd();
}

// Realistic Bee - Now accepts scale and alpha parameters
void drawRealisticBee_hive(float x, float y, float scale = 1.0f, float alpha = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f); // Apply scaling

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float body_seg_r = 0.025f;
    drawCircle(-0.04f, 0.0f, body_seg_r, 50, 1.0f, 0.85f, 0.0f, alpha);
    drawCircle(-0.02f, 0.0f, body_seg_r, 50, 0.0f, 0.0f, 0.0f, alpha);
    drawCircle(0.0f, 0.0f, body_seg_r, 50, 1.0f, 0.85f, 0.0f, alpha);
    drawCircle(0.02f, 0.0f, body_seg_r, 50, 0.0f, 0.0f, 0.0f, alpha);
    drawCircle(0.04f, 0.0f, body_seg_r, 50, 1.0f, 0.85f, 0.0f, alpha);

    // Bee's head
    drawCircle(0.065f, 0.01f, 0.02f, 50, 0.0f, 0.0f, 0.0f, alpha);

    // Eyes
    drawCircle(0.06f, 0.02f, 0.005f, 20, 1.0f, 1.0f, 1.0f, alpha); // Left eye
    drawCircle(0.07f, 0.02f, 0.005f, 20, 1.0f, 1.0f, 1.0f, alpha); // Right eye

    // Pupils
    drawCircle(0.061f, 0.021f, 0.002f, 10, 0.0f, 0.0f, 0.0f, alpha); // Left pupil
    drawCircle(0.071f, 0.021f, 0.002f, 10, 0.0f, 0.0f, 0.0f, alpha); // Right pupil


    glColor4f(0.3f, 0.15f, 0.05f, alpha); // Apply alpha to lines
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(0.06f, 0.025f); glVertex2f(0.075f, 0.04f);
        glVertex2f(0.055f, 0.02f); glVertex2f(0.07f, 0.035f);
    glEnd();
    glLineWidth(1.0f);

    glColor4f(0.3f, 0.15f, 0.05f, alpha); // Apply alpha to lines
    glBegin(GL_LINES);
        glVertex2f(0.03f, -0.02f); glVertex2f(0.04f, -0.04f);
        glVertex2f(0.04f, -0.04f); glVertex2f(0.03f, -0.05f);
        glVertex2f(0.0f, -0.025f); glVertex2f(0.0f, -0.045f);
        glVertex2f(0.0f, -0.045f); glVertex2f(-0.01f, -0.055f);
        glVertex2f(-0.03f, -0.02f); glVertex2f(-0.04f, -0.04f);
        glVertex2f(-0.04f, -0.04f); glVertex2f(-0.05f, -0.05f);
    glEnd();

    // Wings
    if (beeWingFlapState_hive == 0) { // Wings up
        drawCircle(-0.005f, 0.03f, 0.03f, 50, 1.0f, 1.0f, 1.0f, 0.6f * alpha);
        drawCircle(-0.025f, 0.025f, 0.025f, 50, 1.0f, 1.0f, 1.0f, 0.6f * alpha);
    } else { // Wings slightly down (simulating flap)
        drawCircle(-0.005f, 0.035f, 0.028f, 50, 1.0f, 1.0f, 1.0f, 0.6f * alpha);
        drawCircle(-0.027f, 0.03f, 0.023f, 50, 1.0f, 1.0f, 1.0f, 0.6f * alpha);
    }

    // Honey drop - this remains part of the bee's drawing
    // Only draw the honey drop if the bee is NOT fully "inside" the hive (alpha is very low)
    if (alpha > 0.05f) { // If bee is still visible, draw honey
        glColor4f(1.0f, 0.65f, 0.0f, alpha); // Apply alpha to honey
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0.07f, -0.01f);
            int honey_segments = 20;
            float honey_base_r = 0.008f;
            float honey_base_cx = 0.07f;
            float honey_base_cy = -0.02f;
            for (int i = 0; i <= honey_segments; i++) {
                float theta = M_PI + (M_PI * 0.9f) * (float)i / (float)honey_segments;
                glVertex2f(honey_base_cx + honey_base_r * cos(theta), honey_base_cy + honey_base_r * sin(theta));
            }
        glEnd();
    }

    glDisable(GL_BLEND); // Disable blending after drawing bee
    glPopMatrix();
}

// Function to draw the tree trunk (larger again)
void drawTreeTrunk_hive() {
    glColor3f(0.55f, 0.27f, 0.07f); // Brown
    glBegin(GL_QUADS);
        glVertex2f(0.6f, -0.2f);
        glVertex2f(0.68f, -0.2f); // Wider base
        glVertex2f(0.68f, 0.25f);    // Straightened top-right x
        glVertex2f(0.60f, 0.25f);    // Straightened top-left x
    glEnd();
}

// Function to initialize leaf properties (called once)
void initializeTreeLeaves_hive() {
    float clusterCenters[NUM_CLUSTER_CENTERS][2] = {
        {0.0f, 0.0f}, {0.08f, 0.02f}, {-0.08f, 0.01f},
        {0.15f, 0.08f}, {-0.15f, 0.07f},
        {0.05f, 0.15f}, {-0.05f, 0.14f},
        {0.20f, 0.00f}, {-0.20f, -0.01f},
        {0.10f, 0.18f}, {-0.10f, 0.17f},
        {0.0f, 0.20f}
    };

    actualNumLeaves = 0;
    for (int c = 0; c < NUM_CLUSTER_CENTERS; c++) {
        float baseX = clusterCenters[c][0];
        float baseY = clusterCenters[c][1];
        int leavesCount = LEAVES_PER_CLUSTER_MIN + rand() % (LEAVES_PER_CLUSTER_MAX - LEAVES_PER_CLUSTER_MIN + 1);

        for (int i = 0; i < leavesCount; i++) {
            if (actualNumLeaves >= TOTAL_LEAVES) break; // Prevent buffer overflow

            float angle = 2.0f * M_PI * i / leavesCount;
            float currentRadius = 0.04f + 0.015f * (float)rand() / RAND_MAX;
            float xOffset = baseX + currentRadius * std::cos(angle) * (0.9f + 0.1f * (float)rand() / RAND_MAX);
            float yOffset = baseY + currentRadius * std::sin(angle) * (0.9f + 0.1f * (float)rand() / RAND_MAX);

            treeLeaves[actualNumLeaves].x = xOffset;
            treeLeaves[actualNumLeaves].y = yOffset;
            treeLeaves[actualNumLeaves].radius = 0.035f + 0.008f * (float)rand() / RAND_MAX;
            treeLeaves[actualNumLeaves].green = 0.3f + 0.45f * (float)rand() / RAND_MAX;
            treeLeaves[actualNumLeaves].red = 0.0f; // All leaves are green
            treeLeaves[actualNumLeaves].blue = 0.0f; // All leaves are green
            actualNumLeaves++;
        }
    }

    // Initialize leaves partially covering branch
    float tempCoverLeaves[5][3] = {
        {-0.09f, 0.03f, 0.03f},
        {-0.14f, 0.06f, 0.025f},
        {-0.13f, 0.02f, 0.028f},
        {-0.17f, 0.08f, 0.02f},
        {-0.2f, 0.1f, 0.015f}
    };

    for (int i = 0; i < 5; ++i) {
        coverLeaves[i].x = tempCoverLeaves[i][0];
        coverLeaves[i].y = tempCoverLeaves[i][1];
        coverLeaves[i].radius = tempCoverLeaves[i][2];
        coverLeaves[i].green = 0.3f + 0.35f * (float)rand()/RAND_MAX;
        coverLeaves[i].red = 0.0f;
        coverLeaves[i].blue = 0.0f;
    }
}

// Structured leaves and thin smooth branch
void drawLeafyCanopyWithBranch_hive() {
    glPushMatrix();
    // Adjusted y-translation to connect with the trunk
    glTranslatef(0.6f, 0.28f, 0.0f);

    // Draw structured leaves as layered clusters instead of random scatter
    for (int i = 0; i < actualNumLeaves; i++) {
        drawCircle(treeLeaves[i].x, treeLeaves[i].y, treeLeaves[i].radius, 25,
                    treeLeaves[i].red, treeLeaves[i].green, treeLeaves[i].blue);
    }

    // Draw thin smooth abstract branch curve using GL_LINE_STRIP and polygon for thickness
    glColor3f(0.45f, 0.22f, 0.05f); // Brown

    // Define branch curve control points for a smooth Bezier-like curve approximation
    float branchPts[6][2] = {
        {-0.02f, -0.07f},
        {-0.05f, -0.04f},
        {-0.1f, 0.00f},
        {-0.15f, 0.05f},
        {-0.18f, 0.08f},
        {-0.2f, 0.10f}
    };

    // We'll draw the branch as a thin polygon strip around these points
    const float branchThickness = 0.012f;
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i < 6; i++) {
        float dx, dy, length;
        if (i < 5) {
            dx = branchPts[i+1][1] - branchPts[i][1];
            dy = -(branchPts[i+1][0] - branchPts[i][0]);
        } else {
            dx = branchPts[i][1] - branchPts[i-1][1];
            dy = -(branchPts[i][0] - branchPts[i-1][0]);
        }
        length = sqrt(dx*dx + dy*dy);
        if (length == 0) length = 1;
        dx = dx / length * branchThickness;
        dy = dy / length * branchThickness;

        glVertex2f(branchPts[i][0] + dx, branchPts[i][1] + dy);
        glVertex2f(branchPts[i][0] - dx, branchPts[i][1] - dy);
    }
    glEnd();

    // Add subtle bark texture lines on branch
    glColor3f(0.38f, 0.18f, 0.03f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(-0.07f, -0.02f); glVertex2f(-0.08f, 0.00f);
        glVertex2f(-0.11f, 0.02f);  glVertex2f(-0.12f, 0.04f);
        glVertex2f(-0.15f, 0.06f);  glVertex2f(-0.16f, 0.07f);
        glVertex2f(-0.18f, 0.09f);  glVertex2f(-0.19f, 0.1f);
    glEnd();
    glLineWidth(1.0f);

    // Draw leaves partially covering branch to integrate it naturally
    for (int i=0; i<5; ++i) {
        drawCircle(coverLeaves[i].x, coverLeaves[i].y, coverLeaves[i].radius, 25,
                    coverLeaves[i].red, coverLeaves[i].green, coverLeaves[i].blue);
    }

    // Hive attachment point on branch
    glColor3f(0.4f, 0.2f, 0.05f);
    drawCircle(-0.2f, 0.1f, 0.012f, 15, 0.4f, 0.2f, 0.05f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.21f, 0.11f);
        glVertex2f(-0.19f, 0.11f);
        glVertex2f(-0.2f, 0.13f);
    glEnd();

    glPopMatrix();
}

// Hive
void drawHive_hive(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    glColor3f(0.5f, 0.3f, 0.1f); // Brown stem
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(0.0f, 0.08f);
        glVertex2f(0.0f, 0.11f);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(1.0f, 0.84f, 0.0f); // Gold yellow hive
    float base_width = 0.15f;
    float overall_height = 0.15f;
    float segment_overlap = 0.015f;
    int num_segments = 5;
    for(int i = 0; i < num_segments; ++i) {
        float current_y = -overall_height / 2.0f + (overall_height / num_segments) * i;
        float segment_factor = 1.0f - (std::abs(i - (num_segments - 1) / 2.0f) / (num_segments / 2.0f));
        float current_width = base_width * (0.6f + 0.4f * segment_factor);
        float current_height = overall_height / num_segments + segment_overlap;
        float current_radius = current_height * 0.5f;
        drawRoundedRect(-current_width / 2.0f, current_y, current_width, current_height, current_radius, 1.0f, 0.84f, 0.0f);
    }

    glColor3f(0.8f, 0.6f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, -0.06f);
        glVertex2f(-base_width/2.0f + 0.005f, -0.03f);
        glVertex2f(base_width/2.0f - 0.005f, -0.03f);
    glEnd();

    // Hive entrance
    drawCircle(0.0f, -0.03f, 0.03f, 50, 0.3f, 0.2f, 0.1f);

    glPopMatrix();
}

void drawCartoonTreeWithHiveBranch_hive() {
    drawTreeTrunk_hive();
    drawLeafyCanopyWithBranch_hive();
}

// Update bee position for animation
void updateBee_hive(int value) {
    if (!startBeeAnimation_hive) {
        return; // Only update if animation is active
    }

    float dx, dy, dist;

    switch (currentBeeState_hive) {
        case IDLE_HIVE:
            // Should not be in IDLE if startBeeAnimation is true
            break;

        case FLYING_TO_HONEY_HIVE:
            beeTargetX_hive = HONEY_TARGET_X_HIVE;
            beeTargetY_hive = HONEY_TARGET_Y_HIVE;
            dx = beeTargetX_hive - beeX_hive;
            dy = beeTargetY_hive - beeY_hive;
            dist = sqrtf(dx * dx + dy * dy);

            if (dist < beeSpeed_hive) {
                beeX_hive = beeTargetX_hive;
                beeY_hive = beeTargetY_hive;
                currentBeeState_hive = HOVERING_AT_HONEY_HIVE;
                beeHoverTimer_hive = 0;
            } else {
                beeX_hive += dx / dist * beeSpeed_hive;
                beeY_hive += dy / dist * beeSpeed_hive;
            }
            break;

        case HOVERING_AT_HONEY_HIVE:
            beeHoverTimer_hive += 100; // Increase timer by the timerFunc interval
            if (beeHoverTimer_hive >= HOVER_DURATION_HIVE) {
                currentBeeState_hive = FLYING_TO_HIVE_HIVE;
            }
            break;

        case FLYING_TO_HIVE_HIVE:
            beeTargetX_hive = HIVE_LANDING_X_HIVE;
            beeTargetY_hive = HIVE_LANDING_Y_HIVE;
            dx = beeTargetX_hive - beeX_hive;
            dy = beeTargetY_hive - beeY_hive;
            dist = sqrtf(dx * dx + dy * dy);

            // If bee is close enough to the hive entrance, start "entering" effect
            if (dist < ENTER_DISTANCE_THRESHOLD_HIVE) {
                // Calculate entrance progress based on remaining distance
                entranceProgress_hive = 1.0f - (dist / ENTER_DISTANCE_THRESHOLD_HIVE);
                // Ensure progress is within 0-1 range
                if (entranceProgress_hive > 1.0f) entranceProgress_hive = 1.0f;
                if (entranceProgress_hive < 0.0f) entranceProgress_hive = 0.0f;

                // Move bee slightly further into the entrance
                beeX_hive += dx / dist * beeSpeed_hive; // Continue moving towards the center
                beeY_hive += dy / dist * beeSpeed_hive;

                if (dist < beeSpeed_hive * 0.5f) { // If very close to center, consider it "landed"
                    beeX_hive = beeTargetX_hive;
                    beeY_hive = beeTargetY_hive;
                    currentBeeState_hive = LANDED_AT_HIVE_HIVE; // Bee is now "inside" the hive
                    startBeeAnimation_hive = false; // Animation stops when landed inside
                    entranceProgress_hive = 1.0f; // Ensure full progress when landed
                }
            } else {
                beeX_hive += dx / dist * beeSpeed_hive;
                beeY_hive += dy / dist * beeSpeed_hive;
                entranceProgress_hive = 0.0f; // Reset progress if not near entrance
            }
            break;

        case LANDED_AT_HIVE_HIVE:
            // Bee is landed/inside, do nothing until 'C' is pressed again
            // Ensure full "invisibility" when landed
            entranceProgress_hive = 1.0f;
            break;
    }

    // Wing flap toggle (always happens when bee is not IDLE or LANDED_AT_HIVE and still visible)
    // Only flap wings if the bee is not fully transparent.
    if (currentBeeState_hive != IDLE_HIVE && (currentBeeState_hive != LANDED_AT_HIVE_HIVE || entranceProgress_hive < 1.0f)) {
        beeWingFlapState_hive = (beeWingFlapState_hive + 1) % 2;
    }

    if (!showInformation && showBeeAnimationScene) { // Only redisplay if this scene is active
        glutPostRedisplay();
    }

    // Keep timer running during hover and "entering" hive states.
    // No need to run timer if completely landed inside (entranceProgress == 1.0f).
    if (startBeeAnimation_hive || currentBeeState_hive == HOVERING_AT_HONEY_HIVE || (currentBeeState_hive == FLYING_TO_HIVE_HIVE && entranceProgress_hive < 1.0f)) {
        glutTimerFunc(100, updateBee_hive, 0); // Update every 100ms for wing flap and position updates
    }
}

void updateClouds_hive(int value) {
    for (int i = 0; i < 3; ++i) {
        cloudXPositions_main[i] += 0.001f; // Reusing cloud positions from main scene
        if (cloudXPositions_main[i] > 1.2f) { // Reset position if out of bounds
            cloudXPositions_main[i] = -1.2f; // Reappear from the left
        }
    }
    if (!showInformation && showBeeAnimationScene) { // Only redisplay if this scene is active
        glutPostRedisplay();
    }
    glutTimerFunc(30, updateClouds_hive, 0); // Update every 30ms for smoother animation
}

void displaySecondAnimationScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawSky_hive();
    drawSea_hive();  // Draw sea behind everything
    drawBeach_hive(); // Draw beach behind grass and mountain
    drawMountain_hive(); // Mountain base adjusted to start from beach top
    drawGrass_hive(); // Grass base adjusted to start from beach top

    // Draw the sun first, so clouds will appear on top of it
    drawSun_hive();

    // Store sun's coordinates for shadow calculation
    float sun_cx = 0.7f;
    float sun_cy = 0.85f; // Updated sun position

    // Draw cloud shadows first, so they appear beneath the clouds
    for (int i = 0; i < 3; ++i) {
        // Pass the appropriate Y-coordinate for the cloud
        float cloudY = (i == 0 ? 0.7f : (i == 1 ? 0.8f : 0.75f));
        drawCloudShadow_hive(cloudXPositions_main[i], cloudY, sun_cx, sun_cy);
    }

    // Draw clouds at their animated positions
    for (int i = 0; i < 3; ++i) {
        drawCloud_hive(cloudXPositions_main[i], i == 0 ? 0.7f : (i == 1 ? 0.8f : 0.75f));
    }

    // Draw the tree (trunk and leaves)
    drawCartoonTreeWithHiveBranch_hive();

    // Hive hang position relative to branch attachment point and canopy translation
    float hiveHangX = HIVE_ATTACH_X_HIVE;
    float hiveHangY = HIVE_ATTACH_Y_HIVE;
    drawHive_hive(hiveHangX, hiveHangY);

    // Calculate current scale and alpha for the bee, based on entranceProgress
    float currentScale = 1.0f - (entranceProgress_hive * (1.0f - ENTER_MIN_SCALE_HIVE));
    float currentAlpha = 1.0f - (entranceProgress_hive * (1.0f - ENTER_MIN_ALPHA_HIVE));

    // Ensure scale and alpha are within their valid ranges
    if (currentScale < ENTER_MIN_SCALE_HIVE) currentScale = ENTER_MIN_SCALE_HIVE;
    if (currentAlpha < ENTER_MIN_ALPHA_HIVE) currentAlpha = ENTER_MIN_ALPHA_HIVE;

    // Only draw the bee if it's not fully "inside" (i.e., its alpha is not near zero)
    if (currentAlpha > 0.01f) { // Draw if mostly visible
        drawRealisticBee_hive(beeX_hive, beeY_hive, currentScale, currentAlpha);
    }
}

//--- Shajid Animation Scene---
void drawCircleF(float x, float y, float radius, int segments = 32) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

void drawWheelF(float x, float y, float radius) {
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircleF(x, y, radius);

    glColor3f(0.8f, 0.8f, 0.8f);
    drawCircleF(x, y, radius * 0.7f);

    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < 8; i++) {
        float angle = wheelRotation + (i * M_PI / 4);
        float x1 = x + (radius * 0.3f) * cos(angle);
        float y1 = y + (radius * 0.3f) * sin(angle);
        float x2 = x + (radius * 0.6f) * cos(angle);
        float y2 = y + (radius * 0.6f) * sin(angle);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    }
    glEnd();

    glColor3f(0.4f, 0.4f, 0.4f);
    drawCircleF(x, y, radius * 0.2f);
}

void drawCloudF(float x, float y, float size) {
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircleF(x, y, size);
    drawCircleF(x + size * 0.8f, y, size * 0.8f);
    drawCircleF(x + size * 1.6f, y, size);
    drawCircleF(x + size * 2.2f, y, size * 0.7f);
    drawCircleF(x + size * 0.4f, y + size * 0.5f, size * 0.6f);
    drawCircleF(x + size * 1.2f, y + size * 0.5f, size * 0.7f);
}

void drawTreeF(float x, float y) {
    glColor3f(0.4f, 0.2f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y);
    glVertex2f(x + 10, y);
    glVertex2f(x + 10, y + 60);
    glVertex2f(x - 10, y + 60);
    glEnd();

    glColor3f(0.0f, 0.5f, 0.0f); // Deep green
    drawCircleF(x, y + 80, 35);
    drawCircleF(x - 20, y + 65, 25);
    drawCircleF(x + 20, y + 65, 25);
    drawCircleF(x, y + 55, 30);
}

void drawHoneyTruck() {
    float baseX = truckX;  // pixel value, used with gluOrtho2D(-400,400,...)
    float baseY = 100.0f;  // reasonable road height


    // Main truck body - Beige/Cream color like in image
    glColor3f(0.95f, 0.9f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(baseX, baseY);
    glVertex2f(baseX + 160, baseY);
    glVertex2f(baseX + 160, baseY + 70);
    glVertex2f(baseX, baseY + 70);
    glEnd();

    // Truck cab - Blue color like in image
    glColor3f(0.2f, 0.3f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(baseX + 160, baseY);
    glVertex2f(baseX + 220, baseY);
    glVertex2f(baseX + 220, baseY + 90);
    glVertex2f(baseX + 160, baseY + 70);
    glEnd();

    // Truck body outline
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(baseX, baseY);
    glVertex2f(baseX + 160, baseY);
    glVertex2f(baseX + 160, baseY + 70);
    glVertex2f(baseX + 220, baseY + 90);
    glVertex2f(baseX + 220, baseY);
    glVertex2f(baseX, baseY);
    glEnd();

    // Cab outline
    glBegin(GL_LINE_LOOP);
    glVertex2f(baseX + 160, baseY);
    glVertex2f(baseX + 220, baseY);
    glVertex2f(baseX + 220, baseY + 90);
    glVertex2f(baseX + 160, baseY + 70);
    glEnd();

    // Front windshield - Light blue
    glColor3f(0.7f, 0.8f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(baseX + 185, baseY + 75);
    glVertex2f(baseX + 215, baseY + 85);
    glVertex2f(baseX + 215, baseY + 65);
    glVertex2f(baseX + 185, baseY + 55);
    glEnd();

    // Vertical line on truck body
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(baseX + 200, baseY);
    glVertex2f(baseX + 200, baseY + 70);
    glEnd();

    // Door handle
    glColor3f(0.5f, 0.5f, 0.5f);
    drawCircleF(baseX + 205, baseY + 35, 3);

    // Headlight
    glColor3f(1.0f, 1.0f, 0.9f);
    drawCircleF(baseX + 220, baseY + 25, 8);

    // Headlight outline
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * M_PI * i / 32;
        glVertex2f(baseX + 220 + 8 * cos(angle), baseY + 25 + 8 * sin(angle));
    }
    glEnd();

    // Wheels
    drawWheelF(baseX + 40, baseY - 20, 25);
    drawWheelF(baseX + 180, baseY - 20, 25);

    // HONEY text - Dark color for better visibility
    glColor3f(0.3f, 0.3f, 0.3f);
    drawText(baseX + 50, baseY + 35, "HONEY", GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawBackgroundF() {
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.8f, 1.0f);
    glVertex2f(-400, 200);
    glVertex2f(400, 200);
    glColor3f(0.8f, 0.9f, 1.0f);
    glVertex2f(400, 400);
    glVertex2f(-400, 400);
    glEnd();

    glColor3f(0.4f, 0.7f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-400, -200);
    glVertex2f(400, -200);
    glVertex2f(400, 200);
    glVertex2f(-400, 200);
    glEnd();

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-400, 0);
    glVertex2f(400, 0);
    glVertex2f(400, 100);
    glVertex2f(-400, 100);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    for (int i = -400; i < 400; i += 60) {
        glBegin(GL_QUADS);
        glVertex2f(i, 45);
        glVertex2f(i + 30, 45);
        glVertex2f(i + 30, 55);
        glVertex2f(i, 55);
        glEnd();
    }

    drawCloudF(cloudX1, 320, 20);
    drawCloudF(cloudX2, 350, 25);
    drawCloudF(cloudX3, 330, 18);

    drawTreeF(-300, 100);
    drawTreeF(-150, 100);
    drawTreeF(250, 100);
    drawTreeF(350, 100);

    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircleF(-250, 350, 30);
    glColor3f(1.0f, 0.8f, 0.0f);
    drawCircleF(-250, 350, 25);
}

void updateTruck(int value) {
    if (truckMoving) {
        truckX += 2.0f;
        if (truckX > 600) truckX = -300;
        wheelRotation += 0.3f;
        if (wheelRotation > 2 * M_PI) wheelRotation = 0;
    }
    glutPostRedisplay();
    glutTimerFunc(50, updateTruck, 0);
}


//--- Jubo Animation Scene ---
void displayJuboAnimationScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    glColor3ub(0, 0, 0);

    glLineWidth(1);

    //Shelf
    glBegin(GL_QUADS);
    glColor3ub(150, 150, 150);
    glVertex2f(1.0f,0.025f);
    glVertex2f(-1.0f,0.025f);
    glVertex2f(-1.0f,-0.025f);
    glVertex2f(1.0f,-0.025f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3ub(150, 150, 150);
    glVertex2f(1.0f,0.375f);
    glVertex2f(-1.0f,0.375f);
    glVertex2f(-1.0f,0.325f);
    glVertex2f(1.0f,0.325f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3ub(150, 150, 150);
    glVertex2f(1.0f,0.725f);
    glVertex2f(-1.0f,0.725f);
    glVertex2f(-1.0f,0.675f);
    glVertex2f(1.0f,0.675f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3ub(150, 150, 150);
    glVertex2f(1.0f,-0.375f);
    glVertex2f(-1.0f,-0.375f);
    glVertex2f(-1.0f,-0.325f);
    glVertex2f(1.0f,-0.325f);
    glEnd();
    //Honey Jar
    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.780f,0.25f);
    glVertex2f(-0.925f,0.25f);
    glVertex2f(-0.925f,0.025f);
    glVertex2f(-0.780f,0.025f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.750f,0.25f);  // Bottom point (x = 0.1)
    glVertex2f(-0.955f,0.25f);  // Top point (x = 0.1)

    glEnd();
    renderBitmapString(-0.919f, 0.125f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.455f,0.25f);
    glVertex2f(-0.600f,0.25f);
    glVertex2f(-0.600f,0.025f);
    glVertex2f(-0.455f,0.025f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.425f,0.25f);  // Bottom point (x = 0.1)
    glVertex2f(-0.630f,0.25f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(-0.594f, 0.125f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.130f,0.25f);
    glVertex2f(-0.275f,0.25f);
    glVertex2f(-0.275f,0.025f);
    glVertex2f(-0.130f,0.025f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.1f,0.25f);  // Bottom point (x = 0.1)
    glVertex2f(-0.305f,0.25f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(-0.269f, 0.125f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.195f,0.25f);
    glVertex2f(0.05f,0.25f);
    glVertex2f(0.05f,0.025f);
    glVertex2f(0.195f,0.025f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.225f,0.25f);  // Bottom point (x = 0.1)
    glVertex2f(0.02f,0.25f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(0.056f, 0.125f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.52f,0.25f);
    glVertex2f(0.375f,0.25f);
    glVertex2f(0.375f,0.025f);
    glVertex2f(0.52f,0.025f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.55f,0.25f);  // Bottom point (x = 0.1)
    glVertex2f(0.345f,0.25f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(0.381f, 0.125f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.845f,0.25f);
    glVertex2f(0.7f,0.25f);
    glVertex2f(0.7f,0.025f);
    glVertex2f(0.845f,0.025f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.875f,0.25f);  // Bottom point (x = 0.1)
    glVertex2f(0.67f,0.25f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(0.706f, 0.125f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.780f,0.6f);
    glVertex2f(-0.925f,0.6f);
    glVertex2f(-0.925f,0.375f);
    glVertex2f(-0.780f,0.375f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.750f,0.6f);  // Bottom point (x = 0.1)
    glVertex2f(-0.955f,0.6f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(-0.919f, 0.476f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.455f,0.6f);
    glVertex2f(-0.600f,0.6f);
    glVertex2f(-0.600f,0.375f);
    glVertex2f(-0.455f,0.375f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.425f,0.6f);  // Bottom point (x = 0.1)
    glVertex2f(-0.630f,0.6f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(-0.594f, 0.476f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.130f,0.6f);
    glVertex2f(-0.275f,0.6f);
    glVertex2f(-0.275f,0.375f);
    glVertex2f(-0.130f,0.375f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.1f,0.6f);  // Bottom point (x = 0.1)
    glVertex2f(-0.305f,0.6f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(-0.269f, 0.476f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.195f,0.6f);
    glVertex2f(0.05f,0.6f);
    glVertex2f(0.05f,0.375f);
    glVertex2f(0.195f,0.375f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.225f,0.6f);  // Bottom point (x = 0.1)
    glVertex2f(0.02f,0.6f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(0.056f, 0.476f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.52f,0.6f);
    glVertex2f(0.375f,0.6f);
    glVertex2f(0.375f,0.375f);
    glVertex2f(0.52f,0.375f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.55f,0.6f);  // Bottom point (x = 0.1)
    glVertex2f(0.345f,0.6f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(0.381f, 0.476f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.845f,0.6f);
    glVertex2f(0.7f,0.6f);
    glVertex2f(0.7f,0.375f);
    glVertex2f(0.845f,0.375f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.875f,0.6f);  // Bottom point (x = 0.1)
    glVertex2f(0.67f,0.6f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(0.706f, 0.476f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.780f,-0.1f);
    glVertex2f(-0.925f,-0.1f);
    glVertex2f(-0.925f,-0.325f);
    glVertex2f(-0.780f,-0.325f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.750f,-0.1f);  // Bottom point (x = 0.1)
    glVertex2f(-0.955f,-0.1f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(-0.919f, -0.23f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");


    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.455f,-0.1f);
    glVertex2f(-0.600f,-0.1f);
    glVertex2f(-0.600f,-0.325f);
    glVertex2f(-0.455f,-0.325f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.425f,-0.1f);  // Bottom point (x = 0.1)
    glVertex2f(-0.630f,-0.1f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(-0.594f, -0.23f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.130f,-0.1f);
    glVertex2f(-0.275f,-0.1f);
    glVertex2f(-0.275f,-0.325f);
    glVertex2f(-0.130f,-0.325f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.1f,-0.1f);  // Bottom point (x = 0.1)
    glVertex2f(-0.305f,-0.1f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(-0.269f, -0.23f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.195f,-0.1f);
    glVertex2f(0.05f,-0.1f);
    glVertex2f(0.05f,-0.325f);
    glVertex2f(0.195f,-0.325f);
    glEnd();

    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.225f,-0.1f);  // Bottom point (x = 0.1)
    glVertex2f(0.02f,-0.1f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(0.056f, -0.23f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    glPushMatrix();
    glTranslatef(jarX + 0.075f, jarY + 0.1125f, 0.0f); // move jar from initial pos

    // Draw animated jar
    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(-0.06f, 0.1125f);
    glVertex2f(-0.2f, 0.1125f);
    glVertex2f(-0.2f, -0.1125f);
    glVertex2f(-0.06f, -0.1125f);
    glEnd();

    glColor3ub(255, 115, 0); // Label line
    glLineWidth(3.5f);
    glBegin(GL_LINES);
    glVertex2f(-0.045f, 0.1125f);
    glVertex2f(-0.218f, 0.1125f);
    glEnd();

    renderBitmapString(-0.196f, 0.0f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");
    glPopMatrix();

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.845f,-0.1f);
    glVertex2f(0.7f,-0.1f);
    glVertex2f(0.7f,-0.325f);
    glVertex2f(0.845f,-0.325f);
    glEnd();
    glColor3ub(255, 115, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.875f,-0.1f);  // Bottom point (x = 0.1)
    glVertex2f(0.67f,-0.1f);  // Top point (x = 0.1)
    glEnd();
    renderBitmapString(0.706f, -0.23f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    //Bord
    glColor3ub(0, 0, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.15f,1.0f);  // Bottom point (x = 0.1)
    glVertex2f(0.15f,0.925f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(0, 0, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.15f,1.0f);  // Bottom point (x = 0.1)
    glVertex2f(-0.15f,0.925f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(0, 0, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.25f,0.925f);  // Bottom point (x = 0.1)
    glVertex2f(-0.25f,0.925f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(0, 0, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.25f,0.800f);  // Bottom point (x = 0.1)
    glVertex2f(-0.25f,0.800f);  // Top point (x = 0.1)
    glEnd();

    glBegin(GL_QUADS);
    glColor3ub(255, 173, 43);
    glVertex2f(0.25f,0.925f);
    glVertex2f(-0.25f,0.925f);
    glVertex2f(-0.25f,0.800f);
    glVertex2f(0.25f,0.800f);
    glEnd();
    glColor3ub(0, 0, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(-0.246f,0.925f);  // Bottom point (x = 0.1)
    glVertex2f(-0.246f,0.800f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(0, 0, 0); // Black color
    glLineWidth(3.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.246f,0.925f);  // Bottom point (x = 0.1)
    glVertex2f(0.246f,0.800f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(255, 255, 255);         // White using byte values
    renderBitmapString(-0.07f, 0.849f, 0.0f, GLUT_BITMAP_HELVETICA_18, "Honey");

    // Cart
    glColor3ub(50, 100, 160);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.125f, -0.5f);
    glVertex2f(0.175f, -0.8f);
    glVertex2f(0.55f, -0.8f);
    glEnd();

    glColor3ub(50, 100, 160);
    glLineWidth(1.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.575f,-0.475f);  // Bottom point (x = 0.1)
    glVertex2f(0.5f,-0.5f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(50, 100, 160);
    glLineWidth(1.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.575f,-0.475f);  // Bottom point (x = 0.1)
    glVertex2f(0.55f,-0.8f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(50, 100, 160);
    glLineWidth(1.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.509f,-0.56f);  // Bottom point (x = 0.1)
    glVertex2f(0.134f,-0.56f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(50, 100, 160);
    glLineWidth(1.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.527f,-0.66f);  // Bottom point (x = 0.1)
    glVertex2f(0.152f,-0.66f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(50, 100, 160);
    glLineWidth(1.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.544f,-0.76f);  // Bottom point (x = 0.1)
    glVertex2f(0.169f,-0.76f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(50, 100, 160);
    glLineWidth(1.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.225f,-0.5f);  // Bottom point (x = 0.1)
    glVertex2f(0.225f,-0.8f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(50, 100, 160);
    glLineWidth(1.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.3375f,-0.5f);  // Bottom point (x = 0.1)
    glVertex2f(0.3375f,-0.8f);  // Top point (x = 0.1)
    glEnd();

    glColor3ub(50, 100, 160);
    glLineWidth(1.5f); // Optional: make it thicker
    glBegin(GL_LINES);
    glVertex2f(0.45f,-0.5f);  // Bottom point (x = 0.1)
    glVertex2f(0.45f,-0.8f);  // Top point (x = 0.1)
    glEnd();

    glFlush(); // Render now
}

void updateJarAnimation(int value) {
    if (animate) {
        if (jarX < 0.32f) {
            jarX += 0.01f;  // Move right
        }
        if (jarY < -0.7f) {
            jarY += 0.01f;  // Move up
        } else if (jarX >= 0.32f) {
            animate = 0;  // Stop animation
        }
    }

    glutPostRedisplay();              // Redraw scene
    glutTimerFunc(20, updateJarAnimation, 0);     // Repeat every 20ms
}

// --- Keyboard and Display Logic ---
void keyboard(unsigned char key, int x, int y) {
    if (key == 'i' || key == 'I') {
        showInformation = true;
        showInstruction = false;
        showBeeAnimationScene = false;
        showJarScene = false;
        startBeeAnimation_main = false;
        startBeeAnimation_hive = false;
        glutPostRedisplay();
    }
    else if (key == 'm' || key == 'M') {
    showInstruction = true;
    showInformation = false;
    showBeeAnimationScene = false;
    showJarScene = false;
    showTruckScene = false;

    // Stop all ongoing animations
    startBeeAnimation_main = false;
    startBeeAnimation_hive = false;
    truckMoving = false;
    animate = 0;

    glutPostRedisplay();
}
    else if (key == '1') {
        showInformation = false;
        showInstruction = false;
        showBeeAnimationScene = false;
        showJarScene = false;
        glutPostRedisplay();
    }
    else if (key == 'b' || key == 'B') {
        if (!showInformation && !showBeeAnimationScene && !showJarScene) {
            if (!startBeeAnimation_main) {
                startBeeAnimation_main = true;
                beeHoverTime_main = 0;
                glutTimerFunc(30, updateBee_main, 0);
            }
        }
        glutPostRedisplay();
    }
    else if (key == '2') {
        showInformation = false;
        showInstruction = false;
        showBeeAnimationScene = true;
        showJarScene = false;
        startBeeAnimation_main = false;
        startBeeAnimation_hive = false;
        beeX_hive = -0.5f;
        beeY_hive = 0.2f;
        currentBeeState_hive = IDLE_HIVE;
        entranceProgress_hive = 0.0f;
        glutPostRedisplay();
    }
    else if (key == 'c' || key == 'C') {
        if (!showInformation && showBeeAnimationScene) {
            if (!startBeeAnimation_hive && currentBeeState_hive == IDLE_HIVE) {
                startBeeAnimation_hive = true;
                currentBeeState_hive = FLYING_TO_HONEY_HIVE;
                entranceProgress_hive = 0.0f;
                glutTimerFunc(100, updateBee_hive, 0);
            } else if (currentBeeState_hive == LANDED_AT_HIVE_HIVE) {
                startBeeAnimation_hive = false;
                beeX_hive = -0.5f;
                beeY_hive = 0.2f;
                currentBeeState_hive = IDLE_HIVE;
                beeHoverTimer_hive = 0;
                entranceProgress_hive = 0.0f;
                glutPostRedisplay();
            }
        }
    }
    else if (key == '5') {
        // Show jar scene (supermarket)
        showJarScene = true;
        showInformation = false;
        showInstruction = false;
        showBeeAnimationScene = false;
        startBeeAnimation_main = false;
        startBeeAnimation_hive = false;
        jarX = 0.5f;
        jarY = -0.33f;
        animate = 0; // jar not moving until 'a' is pressed
        glutPostRedisplay();
    }
    else if (key == 'j' || key == 'J') {
        if (showJarScene) {
            jarX = 0.4f;    // Set starting point for animation
            jarY = -0.8f;
            animate = 1;   // start animation only if in jar scene
        }
    }
    else if (key == '4') {  // Press '4' to switch to truck scene
    showInformation = false;
    showInstruction = false;
    showBeeAnimationScene = false;
    showJarScene = false;
    showTruckScene = true;

    // Reset truck position and start animation
    truckX = -200.0f;
    wheelRotation = 0.0f;
    truckMoving = false;

    // Stop other animations
    startBeeAnimation_main = false;
    startBeeAnimation_hive = false;

    glutPostRedisplay();
}
else if (key == 't' || key == 'T') {  // Press 't' to toggle truck movement only if truck scene active
    if (showTruckScene) {
        truckMoving = !truckMoving;
    }
}
else if (key == 27) {  // ESC key
    exit(0); // Cleanly exit application
}

}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (showInformation) {
        information();
    }
    else if (showInstruction) {
        instructionPage();
    }
    else if (showBeeAnimationScene) {
        displaySecondAnimationScene();
    }
    else if (showJarScene) {
        displayJuboAnimationScene();
    }
else if (showTruckScene) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-400, 400, -400, 400);  // Use pixel-based coordinates
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    drawBackgroundF();
    drawHoneyTruck();

    glPopMatrix();           // Pop modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();           // Pop projection
    glMatrixMode(GL_MODELVIEW);
}


    else {
        displayMainAnimationScene();
    }

    glutSwapBuffers();
}


void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    srand(time(0)); // Initialize random seed for varied elements

    jarX = 0.5f;
    jarY = -0.325f;
    animate = 1;

    // Call the function to initialize leaf properties ONCE
    initializeTreeLeaves_hive(); // This only needs to be called once for the hive scene elements
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Use GLUT_DOUBLE for smoother animations
    glutInitWindowSize(800, 800);
    glutCreateWindow("The Honey Trail: Animation of a Bees Voyage");
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    // Start cloud animation for both scenes, but they only trigger redisplay if their scene is active
    glutTimerFunc(30, updateClouds_main, 0);
    glutTimerFunc(30, updateClouds_hive, 0); // Both cloud update functions will run, but display only if their scene is active
    glutTimerFunc(0, updateJarAnimation, 0);
    // In main() after other glutTimerFunc calls
    glutTimerFunc(0, updateTruck, 0);

    glutMainLoop();
    return 0;
}
