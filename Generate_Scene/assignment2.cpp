/***
 Assignment-2: Geometric Modeling of a Scene

 Name: Shoji, Emily

 Collaborators: Zapata, Alejandro

 Project Summary: Given a real world scene, replicate it using hierarchical modeling of the objects.
 Begin by building prisms from planes using 3D translations and rotations. These prisms
 are then used to form parts of objects.
***/

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <iostream>
#include <math.h>
#include <vector>
using namespace std;

/**************************************************
 *  Rectangular Prisms via Hierarchical Modeling  *
 *                                                *
 *  using planes as building blocks, build a unit *
 *  cube with transformations that will serve as  *
 *  a primitive for modeling objects in the scene *
 *                                                *
 *************************************************/

// Initializes a square plane of unit lengths
vector<GLfloat> init_plane() {
    vector<GLfloat> vertices = {
        +0.5,   +0.5,   +0.0,
        -0.5,   +0.5,   +0.0,
        -0.5,   -0.5,   +0.0,
        +0.5,   -0.5,   +0.0
    };
    return vertices;
}

// Converts a vector to an array
GLfloat* vector2array(vector<GLfloat> vec) {
    GLfloat* arr = new GLfloat[vec.size()];
    for (int i = 0; i < vec.size(); i++) {
        arr[i] = vec[i];
    }
    return arr;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_homogenous_coord(vector<GLfloat> cartesian_coords) {
    vector<GLfloat> result;
    for (int i = 0; i <= cartesian_coords.size(); i+=1){
        if ( (i != 0) && (i % 3 == 0)) {
            result.push_back(1.0);
        }
        if (i != cartesian_coords.size() ) {
            result.push_back(cartesian_coords[i]);
        }
    }
    
    return result;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_cartesian_coord(vector<GLfloat> homogenous_coords) {
    vector<GLfloat> result;
    for (int i = 0; i < homogenous_coords.size()-1; i++){
        if ( (i+1) % 4 != 0) {
            result.push_back(homogenous_coords[i]);
        }
    }
    // Remove the 1 in the 4th dimension to generate Cartesian coordinates
    
    return result;
}

// Definition of a translation matrix
vector<GLfloat> translation_matrix (float dx, float dy, float dz) {
    vector<GLfloat> translate_mat = {
        1.0, 0.0, 0.0, dx,
        0.0, 1.0, 0.0, dy,
        0.0, 0.0, 1.0, dz,
        0.0, 0.0, 0.0, 1.0
    };
    
    return translate_mat;
}

// Definition of a scaling matrix
vector<GLfloat> scaling_matrix (float sx, float sy, float sz) {
    vector<GLfloat> scale_mat = {
        sx, 0.0, 0.0, 0.0,
        0.0, sy, 0.0, 0.0,
        0.0, 0.0, sz, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    
    return scale_mat;
}

// Definition of a rotation matrix along the x-axis theta degrees
vector<GLfloat> rotation_matrix_x (float theta) {
    float theta_degrees = theta * (M_PI/180);
    vector<GLfloat> rotate_mat_x = {
        1.0, 0.0, 0.0, 0.0,
        0.0, cos(theta_degrees), -sin(theta_degrees), 0.0,
        0.0, sin(theta_degrees), cos(theta_degrees), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    
    return rotate_mat_x;
}


// Definition of a rotation matrix along the y-axis by theta degrees
vector<GLfloat> rotation_matrix_y (float theta) {
    float theta_degrees = theta * (M_PI/180);
    vector<GLfloat> rotate_mat_y = {
        cos(theta_degrees), 0.0, sin(theta_degrees), 0.0,
        0.0, 1.0, 0.0, 0.0,
        -sin(theta_degrees), 0.0, cos(theta_degrees),
        0.0, 0.0, 0.0, 0.0, 1.0
    };
    
    return rotate_mat_y;
}


// Definition of a rotation matrix along the z-axis by theta degrees
vector<GLfloat> rotation_matrix_z (float theta) {
    float theta_degrees = theta * (M_PI/180);
    vector<GLfloat> rotate_mat_z = {
        cos(theta_degrees), -sin(theta_degrees), 0.0, 0.0,
        sin(theta_degrees), cos(theta_degrees), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return rotate_mat_z;
}

void print_matrix (vector<GLfloat> A) {
    cout << "Matrix is:" << "\n";
    for (int k = 0; k < A.size(); k++) {
        cout << A[k] << ", ";
        if ( (k+1) % 3 == 0){
            cout << "\n";
        }
    }
    cout << "\n";
}

// Perform matrix multiplication for A B
vector<GLfloat> mat_mult(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> result;
    
    B = to_homogenous_coord(B);
    
    for (int i = 0; (unsigned)i < B.size(); i+=4) {
        float c0 = A[0]*B[i] +  A[1]*B[i+1] +  A[2]*B[i+2] +   A[3]*B[i+3];
        float c1 = A[4]*B[i] +  A[5]*B[i+1] +  A[6]*B[i+2] +   A[7]*B[i+3];
        float c2 = A[8]*B[i] +  A[9]*B[i+1] +  A[10]*B[i+2] +  A[11]*B[i+3];
        float c3 = A[12]*B[i] + A[13]*B[i+1] + A[14]*B[i+2] +  A[15]*B[i+3];
        
        result.push_back(c0);
        result.push_back(c1);
        result.push_back(c2);
        result.push_back(c3);
    }
    
    return to_cartesian_coord(result);
}

vector<GLfloat> vector_concat(vector<vector<GLfloat> > v) {
    vector<GLfloat> result;
    for (int i = 0; i < v.size(); i++) {
        result.insert(result.end(), v[i].begin(), v[i].end());
    }
    return result;
}

// Builds a unit cube centered at the origin
vector<GLfloat> build_cube() {
    vector<GLfloat> front = mat_mult(translation_matrix(0, 0, 0.5), init_plane());
    vector<GLfloat> back = mat_mult(translation_matrix(0, 0, -0.5), init_plane());
    vector<GLfloat> left = mat_mult(translation_matrix(0.5, 0, 0), mat_mult( rotation_matrix_y(-90.0), init_plane()));
    vector<GLfloat> right = mat_mult(translation_matrix(-0.5, 0, 0), mat_mult(rotation_matrix_y(90.0), init_plane()));
    vector<GLfloat> top = mat_mult(translation_matrix(0, -0.5, 0), mat_mult(rotation_matrix_x(-90.0), init_plane()));;
    vector<GLfloat> bottom = mat_mult(translation_matrix(0, 0.5, 0), mat_mult(rotation_matrix_x(90.0), init_plane()));;
    
    vector<vector<GLfloat>> result = {front, back, left, right, top, bottom};
    return vector_concat(result);
}

/**************************************************
 *            Camera and World Modeling           *
 *                                                *
 *  create a scene by applying transformations to *
 *  the objects built from planes and position    *
 *  the camera to view the scene by using setting *
 *  the projection viewing matrices               *
 *                                                *
 *************************************************/

float theta = 0.0;
int planes = 0;

void setup() {
    // Enable the vertex array functionality
    glEnableClientState(GL_VERTEX_ARRAY);
    // Enable the color array functionality (so we can specify a color for each vertex)
    glEnableClientState(GL_COLOR_ARRAY);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Set up some default base color
    glColor3f(0.5, 0.5, 0.5);
    // Set up white background
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

void init_camera() {
    // Camera parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, 1.0, 2.0, 10.0);
    // Position camera at (2, 3, 5), attention at (0, 0, 0), up at (0, 1, 0)
    gluLookAt(2.0, 3.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

vector<GLfloat> create_chair() {
    vector<GLfloat> chair_seat = mat_mult(translation_matrix(0, 1, 0.5), mat_mult(scaling_matrix(1, 0.2, 1), build_cube()));
    vector<GLfloat> seat_back = mat_mult(translation_matrix(0, 2, 0.025), mat_mult(rotation_matrix_x(90.0), mat_mult(scaling_matrix(1, 0.05, 1.8), build_cube())));
    vector<GLfloat> legFL = mat_mult(translation_matrix(-0.45, -0.1, 0.9), mat_mult(scaling_matrix(0.1, 2, 0.1), build_cube()));
    vector<GLfloat> legBL = mat_mult(translation_matrix(0.9, 0, 0), legFL);
    vector<GLfloat> legBR = mat_mult(translation_matrix(0, 0, -0.85), legFL);
    vector<GLfloat> legFR = mat_mult(translation_matrix(0, 0, -0.85), legBL);
    vector<vector<GLfloat>> chair_cubes = {chair_seat, seat_back, legFL, legBL, legBR, legFR};
    
    vector<GLfloat> chair = vector_concat(chair_cubes);
    chair = mat_mult(translation_matrix(0, 0, -1), mat_mult(scaling_matrix(0.5, 0.5, 0.5), chair));
    planes = (int)chair_cubes.size() * 6;
    return chair;
}

vector<GLfloat> create_desk() {
    vector<GLfloat> top;
    vector<GLfloat> leg1;
    vector<GLfloat> leg2;
    vector<GLfloat> leg3;
    vector<GLfloat> leg4;
    
    
    vector<vector<GLfloat>> desk = {top, leg1, leg2, leg3, leg4};
    return vector_concat(desk);
}

// Construct the scene using objects built from cubes/prisms
GLfloat* init_scene() {
    vector<GLfloat> posZ  = mat_mult(translation_matrix(0, 0, 1), mat_mult(scaling_matrix(0.5, 0.5, 0.5), build_cube()));
    vector<vector<GLfloat>> scene = {posZ, create_chair()};
    return vector2array(vector_concat(scene));
}

// Construct the color mapping of the scene
GLfloat* init_color() {
    vector<GLfloat> posz = {
        1.0,    0.0,    0.0,
        1.0,    0.0,    0.0,
        1.0,    0.0,    0.0,
        1.0,    0.0,    0.0,
        // Back plane
        0.0,    1.0,    0.0,
        0.0,    1.0,    0.0,
        0.0,    1.0,    0.0,
        0.0,    1.0,    0.0,
        // Right
        0.0,    0.0,    1.0,
        0.0,    0.0,    1.0,
        0.0,    0.0,    1.0,
        0.0,    0.0,    1.0,
        // Left
        1.0,    1.0,    0.0,
        1.0,    1.0,    0.0,
        1.0,    1.0,    0.0,
        1.0,    1.0,    0.0,
        // Top
        1.0,    0.0,    1.0,
        1.0,    0.0,    1.0,
        1.0,    0.0,    1.0,
        1.0,    0.0,    1.0,
        // Bottom
        0.0,    1.0,    1.0,
        0.0,    1.0,    1.0,
        0.0,    1.0,    1.0,
        0.0,    1.0,    1.0,
    };
    
    vector<GLfloat> seat = {
        1.0,    0.0,    0.0,
        1.0,    0.0,    0.0,
        1.0,    0.0,    0.0,
        1.0,    0.0,    0.0,
        // Back plane
        0.0,    1.0,    0.0,
        0.0,    1.0,    0.0,
        0.0,    1.0,    0.0,
        0.0,    1.0,    0.0,
        // Right
        0.0,    0.0,    1.0,
        0.0,    0.0,    1.0,
        0.0,    0.0,    1.0,
        0.0,    0.0,    1.0,
        // Left
        1.0,    1.0,    0.0,
        1.0,    1.0,    0.0,
        1.0,    1.0,    0.0,
        1.0,    1.0,    0.0,
        // Top
        1.0,    0.0,    1.0,
        1.0,    0.0,    1.0,
        1.0,    0.0,    1.0,
        1.0,    0.0,    1.0,
        // Bottom
        0.0,    1.0,    1.0,
        0.0,    1.0,    1.0,
        0.0,    1.0,    1.0,
        0.0,    1.0,    1.0,
    };
    
    vector<GLfloat> cube2 = {
        // Front plane
        0.0,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Back plane
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Right
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Left
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Top
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Bottom
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        
    };
    
    vector<GLfloat> leg = {
        // Front plane
        0.0,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Back plane
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Right
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Left
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Top
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        // Bottom
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        0.5,    0.5,    0.5,
        
    };

    
    vector<vector<GLfloat>> scene_colors = {posz, seat, cube2, leg, leg, leg, leg};
    return vector2array(vector_concat(scene_colors));
}

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // World model parameters
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glRotatef(theta, 0.0, 1.0, 0.0);
//    glRotatef(theta, 1.0, 0.0, 0.0);
    
    GLfloat* vertices = init_scene();
    GLfloat* colors = init_color();
    
    glVertexPointer(3,          // 3 components (x, y, z)
                    GL_FLOAT,   // Vertex type is GL_FLOAT
                    0,          // Start position in referenced memory
                    vertices);  // Pointer to memory location to read from
    
    //pass the color pointer
    glColorPointer(3,           // 3 components (r, g, b)
                   GL_FLOAT,    // Vertex type is GL_FLOAT
                   0,           // Start position in referenced memory
                   colors);     // Pointer to memory location to read from
    
    // Draw quad point planes: each 4 vertices
    glDrawArrays(GL_QUADS, 0, 4*(planes +6));
    
    glFlush();            //Finish rendering
    glutSwapBuffers();
    
    free(vertices);
    free(colors);
}

void idle_func() {
    theta = theta + 0.3;
    display_func();
}

int main (int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    // Create a window with rendering context and everything else we need
    glutCreateWindow("Vertex Arrays");

    setup();
    init_camera();

    // Set up our display function
    glutDisplayFunc(display_func);
    glutIdleFunc(idle_func);
    // Render our world
    glutMainLoop();
    return 0;
}

