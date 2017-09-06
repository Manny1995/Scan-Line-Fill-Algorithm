//
//  main.cpp
//  Homework1
//
//  Created by Immanuel Amirtharaj on 9/30/15.
//  Copyright © 2015 Immanuel Amirtharaj. All rights reserved.
//

// This program lets the user plot a set of vertices via left mouse click.  When the
// user is done plotting the vertices, the right mouse button is pressed and a scanline
// algorithm is used to fill the polygon created.

#ifdef _WIN32
#include <gl\glut.h>
#
#elif __APPLE__
#include <GLUT/glut.h>

#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>


#define width 600
#define height 600

#define undefined 10000000000000


using namespace std;

// This class stores the edge with the starting and ending points as well as the slope
class Edge
{
public:
    float x1;
    float x2;
    float y1;
    float y2;
    float slope;
    
};

// This class stores an intersection with the x and y value showing the point of intersection with
// the edge and the scanline
class Intersection
{
public:
    float x;
    float y;
    
};

// This class stores the x and y coordinates of the vertex of the polygon
class Vertex
{
    
public:
    float x;
    float y;
    
};

// decides when to draw the polygon and start filling
bool drawPolygon = false;
bool startFilling = false;


vector<Vertex>verticesVector;
vector<Edge>edgeVector;



//this function initializes the edges unordered
void initEdges()
{
    // Store every single edge of the polygon for future reference
    for(int i=0; i < verticesVector.size(); i++)
    {
        Edge newEdge;
        newEdge.x1 = verticesVector[i].x;
        newEdge.y1 = verticesVector[i].y;
       
        // If you're on the last point, connect it to the beginning of the polygon
        if (i == verticesVector.size() - 1)     //on last point
        {
            newEdge.x2 = verticesVector[0].x;
            newEdge.y2 = verticesVector[0].y;
        }
        
        // else just connect the edge to the next vertex
        else
        {
            newEdge.x2 = verticesVector[i+1].x;
            newEdge.y2 = verticesVector[i+1].y;
        }
        
        // calculate the slope of each edge to use for the scanline algorithm
        if (newEdge.x1 - newEdge.x2 != 0)
        {
            newEdge.slope = (newEdge.y1 - newEdge.y2)/(newEdge.x1-newEdge.x2);
        }
    
        // if the edge is vertical set it to an undefined value
        else
        {
            newEdge.slope = undefined;
        }
        
        
        // add the new edge to the vector
        edgeVector.push_back(newEdge);
    }
    
    
}

// This algorithm is a simple bubble sort.  I will be using this to to order the list of intersections
// from closest to farthest
void bubble_sort (vector<Intersection> &arr, int length)
{
    for (int i = 0; i < length; ++i)
        for (int j = 0; j < length - i - 1; ++j)
            if (arr[j].x > arr[j + 1].x)
            {
                Intersection temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
}


// This is the scan line algorithm where the filling is done
void scanLineFill()
{
    
   // int xPos = 0;
    int yPos = 0;
    
    // traverse the window from the top to the bottom and then fill horizontally accordingly
    for (int i = 0; i < height; i++)
    {
        // change the height to be compatible with the coordinate system
        yPos = height - i;
    
        vector<Intersection>orderedIntersections;
        
        // This loop will check to see if the scanline will hit any of the edges.
        for (int i = 0; i < edgeVector.size(); i++)
        {
            
            float lowerY;
            float upperY;
            
            // getting the bounds for the edges
            if (edgeVector[i].y1 > edgeVector[i].y2)
            {
                lowerY = edgeVector[i].y2;
                upperY =edgeVector[i].y1;
            }
            else
            {
                lowerY = edgeVector[i].y1;
                upperY = edgeVector[i].y2;
            }
            
            // comparing to see if the yPosition of the scan line is in bounds with the edge's y positions.
            // If so, then we know that the scanline will hit the edge
            if (yPos <= upperY && yPos > lowerY)
            {
                // now what we want to do is create an intersection and add the intersecting point and the
                // edge.  We will then add this intersection to a vector
                Intersection newIntersection;
                // if it is a horizontal line, add two intersections where the coordinates are the endpoints
                // of the line
                if (edgeVector[i].slope == 0)
                {
                    newIntersection.x = edgeVector[i].x1;
                    newIntersection.y = yPos;

                    Intersection newIntersection2;
                    newIntersection2.y = yPos;
                    newIntersection2.x = edgeVector[i].x2;
                    orderedIntersections.push_back(newIntersection2);


                }
                // if the edge is vertical, then all you need to put for the intersection is the
                // x coordinate for either of the endpoints
                if (edgeVector[i].slope == undefined)
                {
                    newIntersection.x = edgeVector[i].x1;
                    newIntersection.y = yPos;

                }
                // else just add the intersection of the first coordinate
                else
                {
                    newIntersection.x = ((yPos - edgeVector[i].y1)/edgeVector[i].slope) + edgeVector[i].x1;
                    newIntersection.y = yPos;

                }
                
                orderedIntersections.push_back(newIntersection);
                
            }
        }
        
        // simple bubble sort which will order the intersections based on x value
        bubble_sort(orderedIntersections, (int)orderedIntersections.size());
        

        // Case for two edges: Just draw a line between the two edges
        if (orderedIntersections.size() == 2)
        {
            glColor3f(1.0, 0.0, 0.0);
            glBegin(GL_LINES);
            glVertex2d(orderedIntersections[0].x, yPos);
            glVertex2d(orderedIntersections[1].x, yPos);
            glEnd();
        }
        
        // Case for more than two edges
        else if (orderedIntersections.size() > 2)
        {
            for (int k = 0; k < orderedIntersections.size()-1; k++)
            {
                // Determines whether we draw a line between the edges or not
                if (k%2 == 0)
                {
                    glColor3f(1.0, 0.0, 0.0);
                    glBegin(GL_LINES);
                    glVertex2d(orderedIntersections[k].x, yPos);
                    glVertex2d(orderedIntersections[k+1].x, yPos);
                    glEnd();
                }
            }
        }
    }

}

// mouse Pressed callback function
void mousePressed(int button, int state, int x, int y)
{
    // When the user left clicks, we should add the vertex into the vertex vector
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && drawPolygon == false)
    {
        Vertex newVertex;
        newVertex.x = x;
        newVertex.y = height - y;
        
        verticesVector.push_back(newVertex);
        glutPostRedisplay();

    }
    // When the user right clicks we should draw the polygon and start filling it
    else
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        drawPolygon = true;
        startFilling = true;
        glutPostRedisplay();
        
    }
}


// Resets and quits the program
void reset ()
{
    startFilling = false;
    drawPolygon = false;
    

    exit(0);
    
    
}

// Key Pressed callback function
void keyPressed(unsigned char c, int x, int y)
{
    if (c == 27) //escape key
    {
        reset();
        glutPostRedisplay();
        
    }
    
    
}

// the mouse clicked callback function
void mouseClicked (int x, int y)
{
    Vertex vertex;
    vertex.x = x;
    vertex.y = y;
    
    verticesVector.push_back(vertex);
    
    
    
}


// initializes the mouse and keyboard input
void initPeripherals()
{
    glutMouseFunc(mousePressed);
    glutKeyboardFunc(keyPressed);
    
}


// init function
void init(void) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(1.0,1.0,1.0,1.0);
    
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0,width,0.0,height);
    glMatrixMode(GL_MODELVIEW);
    
    initPeripherals();

    
}


// display the outline of the polygon
void displayPolygon()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBegin(GL_POLYGON);
    
    for(int i=0; i < verticesVector.size(); i++){
        glVertex3i( verticesVector[i].x, verticesVector[i].y, 0);
    }
    
    glEnd();
}

void display(void) {


    glPointSize(5.0f);
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POINTS);
    for(int i=0; i < verticesVector.size(); i++){
        
        glVertex2f( verticesVector[i].x, verticesVector[i].y);
        
        
    }
    
    glEnd();
    
    
    if (drawPolygon)
    {
        displayPolygon();
    }
    
    if (startFilling)
    {
        initEdges();
        scanLineFill();
    }
    
//  Flush data
    glFlush();
}

int main(int argc, char** argv) {
    
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(width,height);
    glutCreateWindow("Left click to draw vertices, right click to fill!");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;

}



