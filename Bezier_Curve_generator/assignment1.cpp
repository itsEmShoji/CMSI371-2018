/***
 assignment1.cpp
 Assignment-1: Cartoonify
 
 Name: Shoji, Emily
 
 Collaborators: Zapata, Alejandro; Azinge, Eddie;
 
 Project Summary: This project implements an iterative version of the Bezier curve algorithm, which by calculating midpoints of
 given control points and connecting them, is able to plot curves.
 
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

#include <vector>
#include <iostream>

using namespace std;

class Vertex {
    GLfloat x, y;
    public:
    Vertex(GLfloat, GLfloat);
    GLfloat get_y() { return y; };
    GLfloat get_x() { return x; };
};

Vertex::Vertex(GLfloat X, GLfloat Y) {
    x = X;
    y = Y;
}

void setup() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

vector<Vertex> generate_midpoints(vector<Vertex> points) {
    vector<Vertex> midpoints;    // Generate points for a given Bezier curve iteration
    
    for (int i = 0; i < points.size()-1; i+=1) {
        Vertex v_0 = points[i];
        Vertex v_1 = points[i+1];
        
        GLfloat x = (1.0/2.0)*v_0.get_x() + (1.0/2.0)*v_1.get_x();
        GLfloat y = (1.0/2.0)*v_0.get_y() + (1.0/2.0)*v_1.get_y();
        
        midpoints.push_back(Vertex(x,y));
    }
    return midpoints;
}

vector<Vertex> generate_points(vector<Vertex> control_points) {
    vector<Vertex> midpoints = generate_midpoints(control_points);
    vector<Vertex> mid_midpoints = generate_midpoints(midpoints);
    vector<Vertex> points;
    points.push_back(control_points[0]);
    points.push_back(midpoints[0]);
    
    for (int i = 0; i < mid_midpoints.size(); i++) {
        points.push_back(mid_midpoints[i]);
        points.push_back(midpoints[i+1]);
    }
    
    points.push_back(control_points[control_points.size()-1]);
    return points;
}

void draw_curve(vector<Vertex> control_points, int n_iter) {
    // Draw a Bezier curve based on the given control points
    while (n_iter > 0) {
        control_points = generate_points(control_points);
        n_iter -= 1;
    }

    glLineWidth(2.0);
    glBegin(GL_LINES);
    for (int i=0; i < control_points.size()-1; i++) {
        glVertex2f(control_points[i].get_x(), control_points[i].get_y());
        glVertex2f(control_points[i+1].get_x(), control_points[i+1].get_y());
    }
    glEnd();
}

void generate_face() {
    vector<Vertex> face_side1;
    face_side1.push_back(Vertex(-0.4f, 0.3f));
    face_side1.push_back(Vertex(-0.4f, 0.1f));
    face_side1.push_back(Vertex(-0.3f, -0.2f));
    face_side1.push_back(Vertex(-0.2f, -0.5f));
    face_side1.push_back(Vertex(-0.01f, -0.65f));
    face_side1.push_back(Vertex(0.0f, -0.6f));
    face_side1.push_back(Vertex(0.2f, -0.65f));
    face_side1.push_back(Vertex(0.31f, -0.57f));
    face_side1.push_back(Vertex(0.3f, -0.59f));
    face_side1.push_back(Vertex(0.51f, -0.3f));
    face_side1.push_back(Vertex(0.5f, 0.0f));
    face_side1.push_back(Vertex(0.5f, 0.3f));
    face_side1.push_back(Vertex(0.49f, 0.4f));
    draw_curve(face_side1, 10);
    
    vector<Vertex> head;
    head.push_back(Vertex(-0.4f, 0.3f));
    head.push_back(Vertex(-0.38f, 0.6f));
    head.push_back(Vertex(-0.35f, 0.7f));
    head.push_back(Vertex(-0.3f, 0.76f));
    head.push_back(Vertex(-0.28f, 0.78f));
    head.push_back(Vertex(-0.24f, 0.8f));
    head.push_back(Vertex(-0.2f, 0.85f));
    head.push_back(Vertex(0.0f, 0.88f));
    head.push_back(Vertex(0.25f, 0.88f));
    head.push_back(Vertex(0.32f, 0.8f));
    head.push_back(Vertex(0.5f, 0.7f));
    head.push_back(Vertex(0.49f, 0.4f));
    draw_curve(head, 10);
    
    vector<Vertex> right_ear;
    right_ear.push_back(Vertex(0.5f, 0.1f));
    right_ear.push_back(Vertex(0.51f, 0.0f));
    right_ear.push_back(Vertex(0.57f, 0.0f));
    right_ear.push_back(Vertex(0.58f, 0.1f));
    right_ear.push_back(Vertex(0.6f, 0.2f));
    right_ear.push_back(Vertex(0.6f, 0.3f));
    right_ear.push_back(Vertex(0.59f, 0.4f));
    right_ear.push_back(Vertex(0.52f, 0.4f));
    right_ear.push_back(Vertex(0.53f, 0.4f));
    right_ear.push_back(Vertex(0.49f, 0.35f));
    draw_curve(right_ear, 10);
    
    vector<Vertex> left_ear;
    left_ear.push_back(Vertex(-0.4f, 0.25f));
    left_ear.push_back(Vertex(-0.41f, 0.3f));
    left_ear.push_back(Vertex(-0.39f, 0.33f));
    left_ear.push_back(Vertex(-0.48f, 0.3f));
    left_ear.push_back(Vertex(-0.47f, 0.2f));
    left_ear.push_back(Vertex(-0.46f, 0.1f));
    left_ear.push_back(Vertex(-0.43f, 0.07f));
    left_ear.push_back(Vertex(-0.42f, 0.0f));
    left_ear.push_back(Vertex(-0.36f, -0.07f));
    left_ear.push_back(Vertex(-0.36f, -0.02f));
    draw_curve(left_ear, 10);
    
    vector<Vertex> hair;
    hair.push_back(Vertex(0.5f, 0.35f));
    hair.push_back(Vertex(0.4f, 0.57f));
    hair.push_back(Vertex(0.3f, 0.47f));
    hair.push_back(Vertex(0.2f, 0.37f));
    hair.push_back(Vertex(0.1f, 0.37f));
    hair.push_back(Vertex(0.0f, 0.17f));
    hair.push_back(Vertex(-0.1f, 0.37f));
    hair.push_back(Vertex(-0.2f, 0.27f));
    hair.push_back(Vertex(-0.3f, 0.25f));
    hair.push_back(Vertex(-0.4f, 0.25f));
    draw_curve(hair, 10);
    
    vector<Vertex> glasses_1;
    glasses_1.push_back(Vertex(0.5f, 0.35f));
    glasses_1.push_back(Vertex(0.5f, 0.35f));
    glasses_1.push_back(Vertex(0.3f, 0.35f));
    glasses_1.push_back(Vertex(0.2f, 0.3f));
    glasses_1.push_back(Vertex(0.1f, 0.25f));
    glasses_1.push_back(Vertex(0.0f, 0.18f));
    glasses_1.push_back(Vertex(-0.08f, 0.25f));
    glasses_1.push_back(Vertex(-0.1f, 0.25f));
    glasses_1.push_back(Vertex(-0.17f, 0.27f));
    glasses_1.push_back(Vertex(-0.36f, 0.27f));
    glasses_1.push_back(Vertex(-0.4f, 0.25f));
    draw_curve(glasses_1, 10);
    
    vector<Vertex> glasses_left_lens;
    glasses_left_lens.push_back(Vertex(-0.4f, 0.25f));
    glasses_left_lens.push_back(Vertex(-0.36f, 0.1f));
    glasses_left_lens.push_back(Vertex(-0.35f, 0.0f));
    glasses_left_lens.push_back(Vertex(-0.33f, -0.1f));
    glasses_left_lens.push_back(Vertex(-0.3f, -0.1f));
    glasses_left_lens.push_back(Vertex(-0.2f, -0.1f));
    glasses_left_lens.push_back(Vertex(-0.01f, -0.1f));
    glasses_left_lens.push_back(Vertex(0.0f, 0.0f));
    glasses_left_lens.push_back(Vertex(0.01f, 0.1f));
    glasses_left_lens.push_back(Vertex(0.0f, 0.21f));
    draw_curve(glasses_left_lens, 10);
    
    vector<Vertex> glasses_right_lens;
    glasses_right_lens.push_back(Vertex(0.1f, 0.25f));
    glasses_right_lens.push_back(Vertex(0.1f, 0.1f));
    glasses_right_lens.push_back(Vertex(0.12f, 0.0f));
    glasses_right_lens.push_back(Vertex(0.2f, -0.06f));
    glasses_right_lens.push_back(Vertex(0.25f, -0.04f));
    glasses_right_lens.push_back(Vertex(0.35f, -0.03f));
    glasses_right_lens.push_back(Vertex(0.45f, -0.03f));
    glasses_right_lens.push_back(Vertex(0.48f, 0.1f));
    glasses_right_lens.push_back(Vertex(0.48f, 0.2f));
    glasses_right_lens.push_back(Vertex(0.5f, 0.35f));
    draw_curve(glasses_right_lens, 10);
    
    vector<Vertex> nose;
    nose.push_back(Vertex(0.0f, 0.0f));
    nose.push_back(Vertex(-0.03f, -0.1f));
    nose.push_back(Vertex(0.05f, -0.12f));
    nose.push_back(Vertex(0.1f, -0.12f));
    nose.push_back(Vertex(0.15f, -0.07f));
    nose.push_back(Vertex(0.16f, -0.03f));
    nose.push_back(Vertex(0.15f, 0.0f));
    draw_curve(nose, 10);
    
    vector<Vertex> top_lip;
    top_lip.push_back(Vertex(-0.02f, -0.25f));
    top_lip.push_back(Vertex(0.0f, -0.2f));
    top_lip.push_back(Vertex(0.04f, -0.2f));
    top_lip.push_back(Vertex(0.06f, -0.16f));
    top_lip.push_back(Vertex(0.1f, -0.3f));
    top_lip.push_back(Vertex(0.13f, -0.16f));
    top_lip.push_back(Vertex(0.14f, -0.15f));
    top_lip.push_back(Vertex(0.2f, -0.2f));
    top_lip.push_back(Vertex(0.24f, -0.21f));
    top_lip.push_back(Vertex(0.3f, -0.18f));
    draw_curve(top_lip, 10);
    
    vector<Vertex> bottom_lip;
    bottom_lip.push_back(Vertex(-0.02f, -0.25f));
    bottom_lip.push_back(Vertex(-0.01f, -0.27f));
    bottom_lip.push_back(Vertex(0.1f, -0.3f));
    bottom_lip.push_back(Vertex(0.3f, -0.18f));
    draw_curve(bottom_lip, 10);
    
    vector<Vertex> bottom_lip2;
    bottom_lip2.push_back(Vertex(-0.02f, -0.25f));
    bottom_lip2.push_back(Vertex(0.0f, -0.35f));
    bottom_lip2.push_back(Vertex(0.15f, -0.35f));
    bottom_lip2.push_back(Vertex(0.3f, -0.18f));
    draw_curve(bottom_lip2, 10);
    
    
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set our color to black (R, G, B)
    glColor3f(0.0f, 0.0f, 0.0f);
    // Draw cartoon
    generate_face();
    glutSwapBuffers();
}

    
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(1000, 1000); // Set your own window size
    glutCreateWindow("Assignment 1");
    setup();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
