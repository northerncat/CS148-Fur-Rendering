/* CS148 Summer 2014
 * Final Project
 * -------------
 * Name: Tsao-Wei (Brad) Huang
 *
 * Refer to the README file for descriptions on using the program.
 */

#include "stdlib.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include "main.h"

#include "GLUT/glut.h"
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#include "util.h"

/*************************************************************************************************/
/*************** These are the same code as I submitted for HW5 (and some in HW4). ***************/
/*************************************************************************************************/

// file locations
std::string vertexShader;
std::string fragmentShader;

SimpleShaderProgram *shader;

static const GLfloat WINDOW_WIDTH = 600.0;
static const GLfloat WINDOW_HEIGHT = 600.0;
static const GLfloat NEAR = 0.0;
static const GLfloat FAR = 6.0;

/* Three arrays of variables that manages the transformations that have been done before.
 * Variables are (in sequence):
 * trans: x translate, y translate, z translate.
 * scale: x scale, y scale, z scale.
 * rot: rotate around x axis, rotate around y axis and rotate around z axis.
 */
GLfloat trans[3];
GLfloat scale[3];
GLfloat rot[3];

// the starting position of the mouse dragging event
GLfloat startx;
GLfloat starty;

// The data structure storing all of the faces as Facef, a struct defined in util.h
std::vector<Trianglef> faces;

// setup the perspective camera of the scene
void setUpCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, NEAR, FAR);

    glEnable(GL_DEPTH_TEST);
}

// this function, modified from the example in the red book, 
// sets up several light sources in the scene
void setupLightSource() {

    GLfloat light0_position[] = { 0.0, 1.0, 0.0, 0.0 };
    GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat red_light[] = {0.6, 0.0, 0.0, 1.0};
    GLfloat cyan_light[] = {0.0, 1.0, 1.0, 1.0};
    GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    // a red light coming from the camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, white_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.2);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

}

// this function computes the normal of a given triangle using functions supported with Normalf
Normalf computeNormal(Trianglef &face) {
    Vertexf x0 = face.va, x1 = face.vb, x2 = face.vc;
    Normalf u = Normalf(x1.x - x0.x, x1.y - x0.y, x1.z - x0.z);
    Normalf v = Normalf(x2.x - x0.x, x2.y - x0.y, x2.z - x0.z);
    Normalf n = Normalf(u.dy*v.dz - u.dz*v.dy, u.dz*v.dx - u.dx*v.dz, u.dx*v.dy - u.dy*v.dx);
    n.Normalize();
    return n;
}

// this function checks if the given faces was specificed normal per vertex by checking
// against the default (0.0, 0.0, 0.0) Normalf
bool NoNormal(Trianglef &face) {
    if (face.nva.length() == 0) {
        return true;
    } else return false;
}

void ReshapeCallback(int w, int h) {

}

void MouseCallback(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        startx = x;
        starty = y;
    }
}

void MotionCallback(int x, int y) {
    trans[0] += (x - startx) / WINDOW_WIDTH;
    trans[1] += (starty - y) / WINDOW_HEIGHT;
}

//  call the display function again when idle
void IdleCallback() {
    glutPostRedisplay();
}

/*************************************************************************************************/
/*************** These are the same code as I submitted for HW5 (and some in HW4). ***************/
/*************************************************************************************************/




GLint furTexWidth = 600;
GLint furNLayer = 512;
GLfloat furLenth = 2.0;
GLfloat furDensity = 0.01;
GLint cTex = 1;
SimpleImage colorTex;
GLint fImage = 0;
SimpleImage furImage;

// Assign random positions in a SimpleImage as the position of the fur strands.
// The fur positions are set as cyan color so that the R value of the SimpleImage
// pixel can be used to check for fur existence.
void assignFur() {
    GLint furNumber = furDensity * furTexWidth * furTexWidth;
    RGBColor white = RGBColor(1.0, 1.0, 1.0);
    furImage = SimpleImage(furTexWidth, furTexWidth, white);
    for (int i = 0; i < furNumber; i++) {
        int x = clamp((float)rand()/(float)RAND_MAX * (float)furTexWidth, 0, furTexWidth-1);
        int y = clamp((float)rand()/(float)RAND_MAX * (float)furTexWidth, 0, furTexWidth-1);
        for (int px = x - 2; px <= x + 2; px++) {
            for (int py = y - 2; py <= y + 2; py++) {
                int setX = clamp(px, 0, furTexWidth-1);
                int setY = clamp(py, 0, furTexWidth-1);
                furImage.setPixel(setX, setY, RGBColor(0.0, 1.0, 1.0));
            }
        }
    }
}

// this function accepts a vertex, its normal and the index of a layer, 
// and transforms the vertex into the corresponding vertex of that layer
Vertexf NthLayer(Vertexf v, Normalf n, int nLayer) {
    GLfloat furStep = furLenth / furNLayer;
    return Vertexf(v.x+nLayer*furStep*n.dx, v.y+nLayer*furStep*n.dy, v.z+nLayer*furStep*n.dz);
}

// This function accepts a triangle and a layer number, and tells OpenGL the vertices and 
// their corresponding normal and texture coordinates to be drawn.
// However, texture parametrization should be improved in order for the variable triangle
// to be rendered correctly..
void drawGeometry(Trianglef face, int layer) {

    // get vector from a to b and from a to c, and normalize by the x and y coordinate to generate
    // vectors for texture coordinates
    Normalf vec_ab = Normalf(face.vb.x - face.va.x, face.vb.y - face.va.y, face.vb.z - face.va.z);
    Normalf vec_ac = Normalf(face.vc.x - face.va.x, face.vc.y - face.va.y, face.vc.z - face.va.z);
    vec_ab.Normalize2D();
    vec_ac.Normalize2D();

    if (NoNormal(face)) {
        // compute normal if no normals are specified
        Normalf n = computeNormal(face);
        // transform the vertices into the corresponding layer
        Vertexf pos_a = NthLayer(face.va, n, layer);
        Vertexf pos_b = NthLayer(face.vb, n, layer);
        Vertexf pos_c = NthLayer(face.vc, n, layer);
        // input normal, texture coordinates, and vertex position
        glNormal3f(n.dx, n.dy, n.dz);       // normal
        glMultiTexCoord2f(0, 0, 0);         // texture coordinate for the fur texture
        glMultiTexCoord2f(1, 0, 0);         // texture coordinate for the color texture
        glVertex3f(face.va.x, face.va.y, face.va.z);

        glMultiTexCoord2f(0, vec_ab.dx, vec_ab.dy);     // texture coordinate for the fur texture
        glMultiTexCoord2f(1, vec_ab.dx, vec_ab.dy);     // texture coordinate for the color texture
        glVertex3f(face.vb.x, face.vb.y, face.vb.z);

        glMultiTexCoord2f(0, vec_ac.dx, vec_ac.dy);     // texture coordinate for the fur texture
        glMultiTexCoord2f(1, vec_ac.dx, vec_ac.dy);     // texture coordinate for the color texture
        glVertex3f(face.vc.x, face.vc.y, face.vc.z);
    } else {
        // if normals are specified, use to interpolate throughout the triangle
        // normal specified for vertex a
        glNormal3f(face.nva.dx, face.nva.dy, face.nva.dz);
        // transform vertex a to the corresponding layer
        Vertexf pos_a = NthLayer(face.va, face.nva, layer);
        glMultiTexCoord2f(0, 0, 0);     // texture coordinate for the fur texture
        glMultiTexCoord2f(1, 0, 0);     // texture coordinate for the color texture
        glVertex3f(pos_a.x, pos_a.y, pos_a.z);

        // normal specified for vertex b
        glNormal3f(face.nvb.dx, face.nvb.dy, face.nvb.dz);
        // transform vertex c to the corresponding layer
        Vertexf pos_b = NthLayer(face.vb, face.nvb, layer);
        glMultiTexCoord2f(0, vec_ab.dx, vec_ab.dy);     // texture coordinate for the fur texture
        glMultiTexCoord2f(1, vec_ab.dx, vec_ab.dy);     // texture coordinate for the color texture
        glVertex3f(pos_b.x, pos_b.y, pos_b.z);

        // normal specified for vertex c
        glNormal3f(face.nvc.dx, face.nvc.dy, face.nvc.dz);         
        // transform vertex c to the corresponding layer
        Vertexf pos_c = NthLayer(face.vc, face.nvc, layer);
        glMultiTexCoord2f(0, vec_ac.dx, vec_ac.dy);     // texture coordinate for the fur texture
        glMultiTexCoord2f(1, vec_ac.dx, vec_ac.dy);     // texture coordinate for the color texture
        glVertex3f(pos_c.x, pos_c.y, pos_c.z);
    }
}

// This function binds the fur texture to texture with ID 0, and the color texture to the texture with ID 1.
// The fur texture is generated from the assignFur function, while the color texture is read from the 
// bigtiger.png file. The two textures are then bound to the samplers in the shaders.
void bindTexture() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, furTexWidth, furTexWidth, 0, GL_RGB, GL_FLOAT, furImage.GetData());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    shader->SetTexture("furImage", fImage);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cTex);
    colorTex.LoadPNG("meshes/bigtiger.png");
    GLint w = colorTex.GetWidth();
    GLint h = colorTex.GetHeight();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, colorTex.GetData());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    shader->SetTexture("colorTexture", cTex);
}

void DisplayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // setup light source again to make light source stationary to viewpoint
    setupLightSource();

    glRotatef(rot[0], 1.0, 0.0, 0.0);
    glRotatef(rot[1], 0.0, 1.0, 0.0);
    glRotatef(rot[2], 0.0, 0.0, 1.0);
    glScalef(scale[0], scale[1], scale[2]);
    glTranslatef(trans[0], trans[1], trans[2]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    shader->Bind();
    bindTexture();

    glBegin(GL_TRIANGLES);

        /****************  This code block is the code for drawing fur on a mesh.  ***************/
        // for (int i = 0; i < faces.size(); i++) {
        //     for (int l = 0; l < furNLayer; l++) {
        //         drawGeometry(faces.at(i), l);
        //     }
        // }
        /****************  This code block is the code for drawing fur on a mesh.  ***************/


        /****************  This code block is the code for drawing a tile of fur.  ***************/
        for (int i = 0; i < furNLayer; i++) {
            // printf("i/furNLayer = %f\n", (float)i/(float)furNLayer);
            glFogCoordf((float)i/(float)furNLayer);    
            Vertexf a = Vertexf(-1.0, -1.0, -0.0);      // bottom left vertex
            Vertexf b = Vertexf(1.0, 1.0, -0.0);        // top right vertex
            Vertexf c = Vertexf(-1.0, 1.0, -0.0);       // top left vertex
            Vertexf d = Vertexf(1.0, -1.0, -0.0);       // bottom right vertex

            Trianglef tri = Trianglef(a, c, b);
            Normalf n = computeNormal(tri);

            glMultiTexCoord2f(0, 0, 0);
            glMultiTexCoord2f(1, 0, 0);
            glVertex3f(NthLayer(a, n, i).x, NthLayer(a, n, i).y, NthLayer(a, n, i).z);

            glMultiTexCoord2f(0, 0, 1);
            glMultiTexCoord2f(1, 0, 1);
            glVertex3f(NthLayer(c, n, i).x, NthLayer(c, n, i).y, NthLayer(c, n, i).z);

            glMultiTexCoord2f(0, 1, 1);
            glMultiTexCoord2f(1, 1, 1);
            glVertex3f(NthLayer(b, n, i).x, NthLayer(b, n, i).y, NthLayer(b, n, i).z);


            tri = Trianglef(a, b, d);
            n = computeNormal(tri);

            glMultiTexCoord2f(0, 0, 0);
            glMultiTexCoord2f(1, 0, 0);
            glVertex3f(NthLayer(a, n, i).x, NthLayer(a, n, i).y, NthLayer(a, n, i).z);

            glMultiTexCoord2f(0, 1, 1);
            glMultiTexCoord2f(1, 1, 1);
            glVertex3f(NthLayer(b, n, i).x, NthLayer(b, n, i).y, NthLayer(b, n, i).z);

            glMultiTexCoord2f(0, 1, 0);
            glMultiTexCoord2f(1, 1, 0);
            glVertex3f(NthLayer(d, n, i).x, NthLayer(d, n, i).y, NthLayer(d, n, i).z);
        } 
        /****************  This code block is the code for drawing a tile of fur.  ***************/
    
    glEnd();
    shader->UnBind();

    glFlush();
}

void SpecialCallback(int key, int x, int y) {
    switch(key) {

        case GLUT_KEY_UP:   // rotate along x axis
            rot[0] += 0.5;
            break;

        case GLUT_KEY_DOWN: // rotate along x axis
            rot[0] -= 0.5;
            break;

        case GLUT_KEY_LEFT:     // rotate along y axis
            rot[1] += 0.5;
            break;

        case GLUT_KEY_RIGHT:    // rotate along y axis
            rot[1] -= 0.5;
            break;

        default:    
            break;
    }
}

void KeyboardCallback(unsigned char key, int x, int y) {
    switch(key) {
        case '=': // zoom in
            for (int i = 0; i < 3; i++) {
                scale[i] *= 1.01;
            }
            break;
        case '-': // zoom out
            for (int i = 0; i < 3; i++) {
                scale[i] *= 0.99;
            }
            break;
        case 'm': // increase fur density
            furDensity *= 1.01;
            assignFur();
            break;
        case 'n': // decrease fur density
            furDensity /= 1.01;
            assignFur();
            break;
        case 'l': // increase fur length
            furLenth *= 1.05;
            assignFur();
            break;
        case 'k': // decrease fur length
            furLenth /= 1.05;
            assignFur();
            break;
        case 't': // make fur thinner through sampling on high resolution textures
            furTexWidth *= 1.05;
            assignFur();
            break;
        case 'y': // make fur thicker through reducing texture resolution
            furTexWidth /= 1.05;
            assignFur();
            break;
        default:
            return;
    }
}

// initializes transformation variables
void init() {
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    assignFur();

    shader = new SimpleShaderProgram();
    shader->LoadVertexShader(vertexShader);
    shader->LoadFragmentShader(fragmentShader);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 3; i++) {
        trans[i] = 0.0;
        scale[i] = 1.0;
        rot[i] = 0.0;
    }
    startx = 0.0;
    starty = 0.0;

}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow ("CS148 Final Project");

    if (argc >= 0) {
        vertexShader   = string(argv[2]);
        fragmentShader = string(argv[3]);
        init();
        setUpCamera();
        setupLightSource();
        openOBJ(string(argv[1]), faces);
    }
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutSpecialFunc(SpecialCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
    glutIdleFunc(IdleCallback);
    
    glutMainLoop();
    return 0;
}