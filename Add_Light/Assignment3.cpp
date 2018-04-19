/***
 Assignment-3: Shading via Lighting and Colors
 
 Name: Shoji, Emily
 
 Collaborators:
 
 Project Summary: generate objects using hierarchical modeling (based onassignment 2) and
 add shading (colors) to these objects by defining a light source (a3-dimensional vector),
 computing the surface normals, and generating the observed colors using Gouraud shading.
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

#include <math.h>
#include <vector>
#include <iostream>
using namespace std;



/**************************************************
 *              Object Model Class                *
 *                                                *
 *  Stores the points of the object as a vector   *
 *  along with the colors and normals for each    *
 *  point. Normals are computed from the points.  *
 *                                                *
 *************************************************/
class ObjectModel {
    vector<GLfloat> _points;
    vector<GLfloat> _normals;
    vector<GLfloat> _base_colors;
    vector<GLfloat> _colors;
public:
    ObjectModel() { };
    vector<GLfloat> get_points() { return _points; };
    vector<GLfloat> get_normals() { return _normals; };
    vector<GLfloat> get_base_colors() { return _base_colors; };
    vector<GLfloat> get_colors() { return _colors; };
    void set_points(vector<GLfloat> points) { _points = points; };
    void set_normals(vector<GLfloat> normals) { _normals = normals; };
    void set_base_colors(vector<GLfloat> base_colors) { _base_colors = base_colors; };
    void set_colors(vector<GLfloat> colors) { _colors = colors; };
};


/**************************************************
 *  Rectangular Prisms via Hierarchical Modeling  *
 *                                                *
 *  Using planes as building blocks, build a unit *
 *  cube with transformations that will serve as  *
 *  a primitive for modeling objects in the scene.*
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
        if ( (k+1) % 4 == 0){
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
        float c3 = 1;
        
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
 *           Generating Surface Normals           *
 *                                                *
 *  Generate the surface normals of the objects   *
 *  using the cross product between two vectors   *
 *  that lie on the surface (plane) of interest.  *
 *  Recall that the direction of the normal to a  *
 *  surface follows the Right Hand Rule.          *
 *                                                *
 *************************************************/

// Performs the cross product between two vectors
vector<GLfloat> cross_product(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> C;
    GLfloat x, y, z;
    x = A[1]*B[2] - A[2]*B[1];
    y = A[2]*B[0] - A[0]*B[2];
    z = A[0]*B[1] - A[1]*B[0];
    C.push_back(x);
    C.push_back(y);
    C.push_back(z);
    return C;
}

vector<GLfloat> subtract_vector(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> result;
    GLfloat x, y, z;
    x = A[0] - B[0];
    y = A[1] - B[1];
    z = A[2] - B[2];
    result.push_back(x);
    result.push_back(y);
    result.push_back(z);
    return result;
}

vector<GLfloat> to_unit(vector<GLfloat> n){
    vector<GLfloat> result;
    GLfloat magnitude = 0;
    for (int i = 0; i < n.size(); i++) {
        magnitude += pow(n[i], 2);
    }
    magnitude = sqrt(magnitude);
    
    for (int j = 0; j < n.size(); j++) {
        result.push_back(n[j]/magnitude);
    }
    return result;
}

// Generates the normals to each surface (plane)
vector<GLfloat> generate_normals(vector<GLfloat> points) {
    vector<vector<GLfloat>> normals;
    vector<GLfloat> q0, q1, q3, a, b, n;
    
    for(int i = 0; i < points.size(); i+=12){
        q0 = {points[i], points[i+1], points[i+2]};
        q1 = {points[i+3], points[i+4], points[i+5]};
        q3 = {points[i+9], points[i+10], points[i+11]};
        
        a = subtract_vector(q1, q0);
        b = subtract_vector(q3, q0);
        
        n = to_unit(cross_product(a, b));
        
        normals.push_back(n);
        normals.push_back(n);
        normals.push_back(n);
        normals.push_back(n);
    }
    
    // Note: each plane (quad) contains 4 points, choose the points
    // to generate your vectors such that the normals (given by the
    // cross product, point to the correct direction
    
    return vector_concat(normals);
}


/**************************************************
 *         Shading via Lighting and Color         *
 *                                                *
 *  Generate the set of colors for each face of   *
 *  the planes that compose the objects in the    *
 *  scene. Based on the light source and surface  *
 *  normals, render the colors of the objects by  *
 *  applying the shading equation.                *
 *                                                *
 *************************************************/

// Initializes the base color of a plane to a single color
vector<GLfloat> init_base_color(GLfloat r, GLfloat g, GLfloat b) {
    vector<GLfloat> base_color = {
        r,   g,   b,
        r,   g,   b,
        r,   g,   b,
        r,   g,   b
    };
    return base_color;
}

// Initializes the base color of a plane by specifying the color of each point
vector<GLfloat> init_base_color(GLfloat r0, GLfloat g0, GLfloat b0, GLfloat r1, GLfloat g1, GLfloat b1,
                                GLfloat r2, GLfloat g2, GLfloat b2, GLfloat r3, GLfloat g3, GLfloat b3) {
    // This enables OpenGL to use interpolation for (Gouraud) shading the plane
    vector<GLfloat> base_color = {
        r0,   g0,   b0,
        r1,   g1,   b1,
        r2,   g2,   b2,
        r3,   g3,   b3
    };
    return base_color;
}

// Generates the colors of a set of surfaces based on the light source,
// surface normals, and base color of the surface
ObjectModel apply_shading(ObjectModel object_model, vector<GLfloat> light_source, vector<GLfloat> camera) {
    vector<GLfloat> colors;
    
    object_model.set_colors(colors);
    return object_model;
}

// Allows for ambience (a vector of 3 values), diffusion (vector of 3 x n values) and specular (vector of 3 x n values)
// as input to the shading equation
ObjectModel apply_shading(ObjectModel object_model, vector<GLfloat> light_source, vector<GLfloat> camera,
                          vector<GLfloat> amb, vector<GLfloat> diff, vector<GLfloat> spec) {
    vector<GLfloat> colors;
    
    object_model.set_colors(colors);
    return object_model;
}

// Performs the dot product between two vectors
GLfloat dot_product(vector<GLfloat> A, vector<GLfloat> B) {
    GLfloat result = 0;
    for (int i = 0; i < A.size(); i++) {
        result += A[i]*B[i];
    }
    return result;
}

/**************************************************
 *            Camera and World Modeling           *
 *                                                *
 *  Create a scene by applying transformations to *
 *  the objects built from planes and position    *
 *  the camera to view the scene by using setting *
 *  the projection viewing matrices               *
 *                                                *
 *************************************************/
float theta = 0.0;
int planes = 0;

ObjectModel create_small_table() {
    vector<GLfloat> top = mat_mult(translation_matrix(0, 0.5, 0.45), mat_mult(scaling_matrix(1, 0.2, 1), build_cube()));
    vector<GLfloat> legFL = mat_mult(translation_matrix(-0.45, 0, 0.9), mat_mult(scaling_matrix(0.05, 1, 0.05), build_cube()));
    vector<GLfloat> legBL = mat_mult(translation_matrix(0.9, 0, 0), legFL);
    vector<GLfloat> legBR = mat_mult(translation_matrix(0, 0, -0.85), legFL);
    vector<GLfloat> legFR = mat_mult(translation_matrix(0, 0, -0.85), legBL);
    vector<GLfloat> legconnectR = mat_mult(translation_matrix(0, -0.52, 0.45), mat_mult(scaling_matrix(1, 1, 0.85), mat_mult(rotation_matrix_x(-90.0),legFR)));
    vector<GLfloat> legconnectL = mat_mult(translation_matrix(-0.9, 0, 0), legconnectR);
    vector<vector<GLfloat>> small_table_parts = {top, legFL, legBL, legBR, legFR, legconnectR, legconnectL};
    
    vector<GLfloat> small_table_pts = vector_concat(small_table_parts);
    small_table_pts = mat_mult(translation_matrix(-1, -0.2, -1), mat_mult(scaling_matrix(0.5, 0.5, 0.5), small_table_pts));
    
    ObjectModel small_table;
    small_table.set_points(small_table_pts);
    
    return small_table;
}

ObjectModel create_coffee_table() {
    vector<GLfloat> top = mat_mult(translation_matrix(0.3, 0.5, 0.55), mat_mult(scaling_matrix(1.55, 0.2, 1), build_cube()));
    vector<GLfloat> legFL = mat_mult(translation_matrix(-0.45, -0.1, 1), mat_mult(scaling_matrix(0.05, 1, 0.05), build_cube()));
    vector<GLfloat> legBL = mat_mult(translation_matrix(1.5, 0, 0), legFL);
    vector<GLfloat> legBR = mat_mult(translation_matrix(0, 0, -0.85), legFL);
    vector<GLfloat> legFR = mat_mult(translation_matrix(0, 0, -0.85), legBL);
    vector<GLfloat> legconnectR = mat_mult(translation_matrix(0, -0.65, 0.5), mat_mult(scaling_matrix(1, 0.55, 0.85), mat_mult(rotation_matrix_x(-90.0),legFR)));
    vector<GLfloat> legconnectL = mat_mult(translation_matrix(-1.5, 0, 0), legconnectR);
    vector<GLfloat> legconnectF = mat_mult(translation_matrix(-0.7, 0, 0.56), mat_mult(scaling_matrix(1.75, 1, 1), mat_mult(rotation_matrix_y(90.0), legconnectL)));
    vector<GLfloat> legconnectB = mat_mult(translation_matrix(0, 0, -0.85),legconnectF);
    vector<vector<GLfloat>> coffee_table_parts = {top, legFL, legBL, legBR, legFR, legconnectR, legconnectL, legconnectB ,legconnectF};
    
    vector<GLfloat> coffee_table_pts = vector_concat(coffee_table_parts);
    coffee_table_pts = mat_mult(translation_matrix(0, 0, 0.5), mat_mult(scaling_matrix(0.5, 0.5, 0.5), coffee_table_pts));
    
    ObjectModel coffee_table;
    coffee_table.set_points(coffee_table_pts);
    
    return coffee_table;
}

ObjectModel create_carpet() {
    vector<GLfloat> carpet_pts = mat_mult(translation_matrix(0.2, -0.29, 0.8), mat_mult(scaling_matrix(1.5, 0.01, 0.75), build_cube()));
    
    ObjectModel carpet;
    carpet.set_points(carpet_pts);
    
    return carpet;
}

ObjectModel create_couch() {
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
    vector<GLfloat> couch_pts = vector_concat(couch_parts);
    
    ObjectModel couch;
    couch.set_points(couch_pts);
    
    return couch;
}

ObjectModel create_ottoman() {
    vector<GLfloat> base = mat_mult(translation_matrix(0, 0, 2), mat_mult(scaling_matrix(0.5, 0.3, 0.5), build_cube()));
    vector<GLfloat> top = mat_mult(translation_matrix(0, 0.24, 2), mat_mult(scaling_matrix(0.5, 0.15, 0.5), build_cube()));
    
    vector<vector<GLfloat>> ottoman_parts = {base, top};
    vector<GLfloat> ottoman_pts = vector_concat(ottoman_parts);
    ottoman_pts = mat_mult(translation_matrix(0.8, -0.2, -0.5), ottoman_pts);
    ObjectModel ottoman_model;
    ottoman_model.set_points(ottoman_pts);
    
    return ottoman_model;
}

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
    gluLookAt(0.2, 1, 4, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Construct the scene using objects built from cubes/prisms
GLfloat* init_scene() {
    ObjectModel coffee_table, ottoman, couch, carpet, small_table;
    coffee_table = create_coffee_table();
    ottoman = create_ottoman();
    couch = create_couch();
    carpet = create_carpet();
    small_table = create_small_table();
    
    vector<vector<GLfloat>> points = {
            ottoman.get_points(),
            small_table.get_points(),
            coffee_table.get_points(),
            carpet.get_points(),
            couch.get_points()
    };
    
    vector<GLfloat> result_vector = vector_concat(points);
    planes = (int)result_vector.size()/3;
    
    return vector2array(vector_concat(points));
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
    glDrawArrays(GL_QUADS, 0, planes);
    
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
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    // Create a window with rendering context and everything else we need
    glutCreateWindow("Assignment 3");
    
    setup();
    init_camera();
    
    // Set up our display function
    glutDisplayFunc(display_func);
    glutIdleFunc(idle_func);
    // Render our world
    glutMainLoop();
    
    // Remember to call "delete" on your dynmically allocated arrays
    // such that you don't suffer from memory leaks. e.g.
    // delete arr;
    
    return 0;
}
