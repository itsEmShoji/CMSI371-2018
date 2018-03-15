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
    gluLookAt(0.1, 1.5, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

vector<GLfloat> create_smallTable() {
    vector<GLfloat> top = mat_mult(translation_matrix(0, 0.5, 0.45), mat_mult(scaling_matrix(1, 0.2, 1), build_cube()));
    vector<GLfloat> legFL = mat_mult(translation_matrix(-0.45, 0, 0.9), mat_mult(scaling_matrix(0.05, 1, 0.05), build_cube()));
    vector<GLfloat> legBL = mat_mult(translation_matrix(0.9, 0, 0), legFL);
    vector<GLfloat> legBR = mat_mult(translation_matrix(0, 0, -0.85), legFL);
    vector<GLfloat> legFR = mat_mult(translation_matrix(0, 0, -0.85), legBL);
    vector<GLfloat> legconnectR = mat_mult(translation_matrix(0, -0.52, 0.45), mat_mult(scaling_matrix(1, 1, 0.85), mat_mult(rotation_matrix_x(-90.0),legFR)));
    vector<GLfloat> legconnectL = mat_mult(translation_matrix(-0.9, 0, 0), legconnectR);
    vector<vector<GLfloat>> smallTable_parts = {top, legFL, legBL, legBR, legFR, legconnectR, legconnectL};
    
    vector<GLfloat> smallTable = vector_concat(smallTable_parts);
    smallTable = mat_mult(translation_matrix(-1, -0.2, -1), mat_mult(scaling_matrix(0.5, 0.5, 0.5), smallTable));
    planes = (int)smallTable_parts.size() * 6;
    return smallTable;
}

vector<GLfloat> create_coffeeTable() {
    vector<GLfloat> top = mat_mult(translation_matrix(0.3, 0.5, 0.55), mat_mult(scaling_matrix(1.55, 0.2, 1), build_cube()));
    vector<GLfloat> legFL = mat_mult(translation_matrix(-0.45, -0.1, 1), mat_mult(scaling_matrix(0.05, 1, 0.05), build_cube()));
    vector<GLfloat> legBL = mat_mult(translation_matrix(1.5, 0, 0), legFL);
    vector<GLfloat> legBR = mat_mult(translation_matrix(0, 0, -0.85), legFL);
    vector<GLfloat> legFR = mat_mult(translation_matrix(0, 0, -0.85), legBL);
    vector<GLfloat> legconnectR = mat_mult(translation_matrix(0, -0.65, 0.5), mat_mult(scaling_matrix(1, 0.55, 0.85), mat_mult(rotation_matrix_x(-90.0),legFR)));
    vector<GLfloat> legconnectL = mat_mult(translation_matrix(-1.5, 0, 0), legconnectR);
    vector<GLfloat> legconnectF = mat_mult(translation_matrix(-0.7, 0, 0.56), mat_mult(scaling_matrix(1.75, 1, 1), mat_mult(rotation_matrix_y(90.0), legconnectL)));
    vector<GLfloat> legconnectB = mat_mult(translation_matrix(0, 0, -0.85),legconnectF);
    vector<vector<GLfloat>> coffeeTable_parts = {top, legFL, legBL, legBR, legFR, legconnectR, legconnectL, legconnectB ,legconnectF};
    
    vector<GLfloat> coffeeTable = vector_concat(coffeeTable_parts);
    coffeeTable = mat_mult(translation_matrix(0, 0, 0.5), mat_mult(scaling_matrix(0.5, 0.5, 0.5), coffeeTable));
    
    planes = planes + (int)coffeeTable_parts.size() * 6;
    return coffeeTable;
}

vector<GLfloat> create_carpet() {
    vector<GLfloat> carpet = mat_mult(translation_matrix(0.2, -0.29, 0.8), mat_mult(scaling_matrix(1.5, 0.01, 0.75), build_cube()));
    planes = planes + 6;
    return carpet;
}

vector<GLfloat> create_couch() {
    vector<GLfloat> base = mat_mult(translation_matrix(0.3, -0.2, -0.8), mat_mult(scaling_matrix(1.6, 0.25, 0.75), build_cube()));
    vector<GLfloat> back = mat_mult(translation_matrix(0, 0.5, -1.2), mat_mult(scaling_matrix(1, 0.5, 0.5), mat_mult(rotation_matrix_x(-90.0), base)));
    vector<GLfloat> cushionR = mat_mult(translation_matrix(0.5, 0.1, -0.09), mat_mult(scaling_matrix(0.45, 0.5, 0.8), base));
    vector<GLfloat> cushionL = mat_mult(translation_matrix(-0.65, 0, 0), cushionR);
    vector<GLfloat> armrestR = mat_mult(translation_matrix(1.05, -0.15, 0), mat_mult(rotation_matrix_z(90.0), mat_mult(scaling_matrix(0.25, 0.65, 1), cushionR)));
    vector<GLfloat> armrestL = mat_mult(translation_matrix(-1.5, 0, 0), armrestR);
    vector<GLfloat> pillowR = mat_mult(translation_matrix(0.38, 0.75, -1.06), mat_mult(scaling_matrix(0.6, 0.65, 0.5), mat_mult(rotation_matrix_x(-100.0), cushionR)));
    vector<GLfloat> pillowM = mat_mult(translation_matrix(-0.48, 0, 0), pillowR);
    vector<GLfloat> pillowL = mat_mult(translation_matrix(-0.95, 0, 0), pillowR);
    vector<GLfloat> pillowSR = mat_mult(translation_matrix(0.6, 0, 0.2), mat_mult(scaling_matrix(0.8, 0.8, 1.1), pillowM));
    vector<GLfloat> pillowSL = mat_mult(translation_matrix(-0.35, 0, 0.3), mat_mult(rotation_matrix_y(25), pillowSR));
    vector<GLfloat> pillowSL2 = mat_mult(translation_matrix(-0.3, 0, 0), pillowSL);
    
    vector<vector<GLfloat>> couch_parts = {base, back, cushionR, cushionL, armrestR, armrestL, pillowR, pillowM, pillowL, pillowSR, pillowSL, pillowSL2};
    vector<GLfloat> couch = vector_concat(couch_parts);
    planes = planes + (int)couch_parts.size() * 6;
    return couch;
}

vector<GLfloat> create_ottoman() {
    vector<GLfloat> base = mat_mult(translation_matrix(0, 0, 2), mat_mult(scaling_matrix(0.5, 0.3, 0.5), build_cube()));
    vector<GLfloat> top = mat_mult(translation_matrix(0, 0.24, 2), mat_mult(scaling_matrix(0.5, 0.15, 0.5), build_cube()));
    
    vector<vector<GLfloat>> ottoman_parts = {base, top};
    vector<GLfloat> ottoman = vector_concat(ottoman_parts);
    ottoman = mat_mult(translation_matrix(0.8, -0.2, -0.5), ottoman);
    planes = planes + (int)ottoman_parts.size() * 6;
    return ottoman;
}

// Construct the scene using objects built from cubes/prisms
GLfloat* init_scene() {
    vector<vector<GLfloat>> scene = {create_ottoman(), create_smallTable(), create_coffeeTable(), create_carpet(), create_couch()};
    return vector2array(vector_concat(scene));
}

// Construct the color mapping of the scene
GLfloat* init_color() {
    vector<GLfloat> SteelBlue = {
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        // Back plane
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        // Right
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        // Left
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        // Top
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        // Bottom
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
        0.275, 0.510, 0.706,
    };

    
    vector<GLfloat> lightSteelBlue = {
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        //back
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        //right
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        //left
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        //top
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        //bottom
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        
    };
    
    vector<GLfloat> goldenRod = {
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        // Back plane
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        // Right
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        // Left
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        // Top
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        // Bottom
        00.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
        0.722, 0.525, 0.043,
    };
    
    vector<GLfloat> grey = {
        // Front plane
        0.5,    0.5,    0.5,
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
    
    vector<GLfloat> black = {
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
        .000, 0.000, 0.000,
    };
    
    vector<GLfloat> royalBlue = {
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
        0.255, 0.412, 0.882,
    };
    
    vector<GLfloat> dodgerBlue = {
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
        0.118, 0.565, 1.000,
    };
    
    vector<GLfloat> clBlue = {
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
        0.392, 0.584, 0.929,
    };
    
    vector<vector<GLfloat>> scene_colors = {
        //ottoman
        royalBlue, SteelBlue,
        //small table
        goldenRod, black, black, black, black, black, black,
        //coffee table
        goldenRod, black, black, black, black, black, black, black, black,
        //carpet
        grey,
        //couch
        royalBlue, royalBlue, SteelBlue, SteelBlue, royalBlue, royalBlue, dodgerBlue, clBlue, lightSteelBlue, lightSteelBlue, dodgerBlue, clBlue};
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
    glDrawArrays(GL_QUADS, 0, 4*(planes+12));
    
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

