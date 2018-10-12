//  Course: CPS 465
//  Assignment 4
//  Name: Qirui Li
//  Description: Load a obj file into OpenGL, and use shiflt + mouse drag to translate the object
//               use mouse drag vertically to rotate the object about y axis, use mouse drag horizontally
//               to rotate the object about x axis. Press 'z' or 'Z' then drag the mouse to rotate the object
//               about z axis. Press 'c' or 'C' then drag mouse with rotate the object with is center location.
//               press 'l' will load the low_bunny_obj, press 'h' will switch to load the high_bunny_obj.
//               press 'm' to diaplay the object with real 3D look. Press 0 to disapper all the color. Press 1 to only
//               display the diffuse term and ambinet term. Press 2 to display diffuse, ambinet and phong term.
//               preess 'G' or 'g' to display the bunny made of glod. Press 'a' or 'A' to display the bunny made of China
//
//
//

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif


#define X .525731112119133606
#define Z .850650808352039932

using namespace std;

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;

double size = 1;

// define a structor about the Vertex
struct Vertex
{
    double x; // x location
    double y; // y location
    double z; // z location
    float normal[3]; // normal vector
    vector<int> common_triangle; // to store the index of triangle, which contains this vertex
    
    Vertex(double x_coordinate, double y_coordinate, double z_coordinate) // constructor
    {
        x = x_coordinate;
        y = y_coordinate;
        z = z_coordinate;
    }
    
    Vertex()
    {
        
    }
    
    Vertex& operator = (const Vertex &v) // overload the "=" operator
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    
    
    
    const Vertex operator + (const Vertex& right) const // overload the "+" operator, to + a vector
    {
        Vertex result;
        result.x = x + right.x;
        result.y = y + right.y;
        result.z = z + right.z;
        return result;
    }
    
    const Vertex operator - (const Vertex& right) const  //overload the "-" operator, to - a vector
    {
        Vertex result;
        result.x = x - right.x;
        result.y = y - right.y;
        result.z = z - right.z;
        return result;
    }
    
    const Vertex operator * (const float right) const //Reload * operator to times a number
    {
        Vertex result;
        result.x = x * right;
        result.y = y * right;
        result.z = z * right;
        return result;
    }
    
    float dotProduct(const Vertex& right) const  //For normalized vector dot product use, use after call normoalization()
    {
        // right is already normalize
        float result;
        result = x * right.x + y * right.y + z * right.z;
        return result;
    }
    
    Vertex cross(const Vertex& right)  // function to calculate the cross product of two vectors then return a vector, use after call normolization()
    {
        Vertex result;
        result.x = y * right.z - z * right.y;
        result.y = z * right.x - x * right.z;
        result.z = x * right.y - y * right.x;
        return result;
    }
    
    bool equal(const Vertex& right) //function to determine are two vertex same
    {
        if (x == right.x && y == right.y && z == right.z)
            return true;
        else
            return false;
    }

    
    const void normalization() // function to normalize the vector
    {
        float sum = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        x = x / sum;
        y = y / sum;
        z = z / sum;
    }
    
    void printPoint()
    {
        cout << "x == " << x << " y == " << y << " z == " << z << endl;
    }
};

// define a structor which represent one surface of the object with three vertexs, v1,v2,v3
struct Triangle
{
    int v1;
    int v2;
    int v3;
    Vertex triangle_normal; // the normal vector represented for the whole triangle
    
    Triangle(int v1_index, int v2_index, int v3_index)
    {
        v1 = v1_index;
        v2 = v2_index;
        v3 = v3_index;
    }
    
    Triangle()
    {
        
    }
};

// define a structor which used to calculate the translation and rotation matrix
struct Mat
{
    double data[4][4];
    
    Mat() // default constructor
    {
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {

                data[x][y] = 0;
            }
        }
    }
    
    //constructor for the Mat
    Mat(double array[4][4])
    {
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                if (array[x][y] == -0)
                {
                    array[x][y] = 0;
                }
                data[x][y] = array[x][y];
            }
        }
    }
    
    //overload * operator,function to calculate a matrix times a matrix 4 by 4 matrix time 4 by 4 matrix
    const Mat operator *(const Mat& right) const
    {
        Mat result;
        for (int x = 0 ; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                for (int z = 0; z < 4; z++)
                {
                    double a = data[x][z] * right.data[z][y];
                    result.data[x][y] = result.data[x][y] + a;
                }
            }
        }
        return result;
    }
    
    
    //overload * operator function to calculate a vertex time a 4 * 4 matrix
    const Vertex operator *(const Vertex& v) const
    {
        Vertex result;
        double data1[4] = {v.x, v.y, v.z, 1};
        for (int x = 0; x < 4; x++)
        {
            double sum = 0;
            for (int y = 0; y < 4; y++)
            {
                sum += data[x][y] * data1[y];
            }
            if (x == 0)
                result.x = sum;
            else if (x == 1)
                result.y = sum;
            else if (x == 2)
                result.z = sum;
        }
        return result;
    }
    
    // overload = operator
    Mat& operator = (const Mat &m)
    {
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                data[x][y] = m.data[x][y];
            }
        }
        return *this;
    }
    
    
    void printMat()
    {
        for(int x = 0; x < 4; x++)
        {
            for(int y = 0; y < 4; y++)
            {
                cout << this->data[x][y] << " ";
            }
            cout << endl;
        }
    }
};


vector<Vertex> vertice_list; // a vector to contain all the vertice
vector<Triangle> triangle_list; // a vector to contain all the triangle

int i;
static GLdouble ex = .0, ey = 0.0, ez = 5.0, upx = .0, upy = 1.0, upz = 0.0, ox = -0.03, oy = 0.27, oz;

//light data, surface color, space color(Ca), specular color(Cs), surface color(Cr)
float light_color[] = {0.9, 0.9, 0.9}; // light color
float light_pos[] = {0.0, 0.0, 5.0}; // light position
float space_color[] = {0.2, 0.2, 0.2}; // ambient, Ca
float specular_color[] = {0.8, 0.8, 0.8}; // phong, Cs
float surface_color[] = {0.8, 0.8, 0.8}; // surface color Cr
float color_to_draw[3]; // final color to illuminate vertex, used in glColor()
float eye_position[] = {float(ex), float(ey), float(ez)}; // eye position vector
float phong_exponent = 76; // phong exponent
bool need_sharding = true;
bool only_diffuse_ambinet = false;
bool diffue_ambinet_phong = true;




/*light data*/
//GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  /* Grey diffuse light. */
//GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; /*Specular light*/
//GLfloat light_ambient[] = { .1, .1, .1, 1.0 }; /*Ambient color */
GLfloat light_position[] = { 0.0, 0.0, 5.0, 1.0 };  /* Infinite light location. */
GLfloat angle = 100;
GLfloat light_direction[] = { .0, .0, -1.0 };
GLfloat light_x = .0f;
GLfloat light_y = .0f;
GLfloat light_z = 5.0f;
int phong = 0;



/*material data*/
GLfloat material_color1[] = { 0.1, 0.7, .0, 1.0 }; //ambient 1
GLfloat material_color2[] = { 1.0, 0.0, .0, 1.0 }; //ambient 2
GLfloat material_color3[] = { .0, 1.0, 0.0, 1.0 }; //specular
GLfloat material_ambient[] = { .5, .5, .5, 1.0 };
GLfloat low_sh[] = {5.0};
GLfloat material_specular[] = { .2, .2, .2, 1. };

/* Define data */
int mouse_down = 0; //Represent the left mouse key is clicked down
int change_mode = 0; //0 means change eye position; 1 means change focus position
int current_x = 0, current_y = 0;


/*Projection matrix mode*/
int projection = 0; //0 - parallel; 1 - perspective

/*Shading Mode*/
int shading = 1;

bool real3DLook = false; // flag to the real 3D look
string fileName = "bunny_high.obj";
queue<Vertex> point_queue; // container to contain the mouse location, always has size == 2
const double degree = M_PI / 180;
bool needToRotateWithCenter = false; // flag to rotate with center
bool needToRotateWithZ = false; // flag to rotate with z axis
double center_pt_x, center_pt_y, center_pt_z; // variable to store the center location of the object


//function to find each vertex appear in which triangle, used for calculate the normal for each vertex
void findCommonTriangle()
{
    // traverse vertice_list to calculate the normal for each vertex
    for (int x = 0 ; x < vertice_list.size(); x++)
    {
        for (int y = 0 ; y < triangle_list.size(); y++)
        {
            //cout << "In x == " << x << " In y == " << y << endl;
            if (vertice_list[x].equal(vertice_list[triangle_list[y].v1]) || vertice_list[x].equal(vertice_list[triangle_list[y].v2]) || vertice_list[x].equal(vertice_list[triangle_list[y].v3]))
            {
                vertice_list[x].common_triangle.push_back(y);
            }
        }
    }
}



// function to read the file then initial the vertice_list and triangle_list
void readFile(string fileName)
{
    ifstream fin;
    fin.open(fileName);
    string line;
    vertice_list.clear();
    triangle_list.clear();
    if (fin.fail())
    {
        cout << "Fail to open the file" << endl;
    }
    string letter, num1, num2, num3;
    while(getline(fin, line))
    {
        //fin >> line;
        istringstream is(line);
        is >> letter >> num1 >> num2 >> num3;
        if (letter == "v")
        {
            vertice_list.push_back(Vertex(atof(num1.c_str()), atof(num2.c_str()),atof(num3.c_str())));
        }
        if (letter == "f")
        {
            triangle_list.push_back(Triangle(atoi(num1.c_str()) - 1, atoi(num2.c_str()) - 1, atoi(num3.c_str()) - 1));
        }
    }
    findCommonTriangle();
}

void initialize()
{
    /* Use depth buffering for hidden surface elimination. */
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, (GLint)WINDOW_WIDTH, (GLint)WINDOW_HEIGHT);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &angle);
    printf("oz %.f\n", oz);
}

// function to calculate the normal
Vertex calculateNormal_Vector(Vertex v1, Vertex v2, Vertex v3)
{
    Vertex normal_vector;
    Vertex difference1 = (v2 - v1); // (v3 - v1);
    Vertex difference2 = (v3 - v1);
    normal_vector = difference1.cross(difference2);
    return normal_vector;
}

/**
void calculateNormal()
{
    //traverse triangle_list to calculate the normal for each triangle
    for (int x = 0 ; x < triangle_list.size(); x++)
    {
        triangle_list[x].triangle_normal = calculateNormal_Vector(vertice_list[triangle_list[x].v1], vertice_list[triangle_list[x].v2], vertice_list[triangle_list[x].v3]);
    }
    
    // traverse vertice_list to calculate the normal for each vertex
    for (int x = 0 ; x < vertice_list.size(); x++)
    {
        int num = 0.0;
        Vertex sum_normal = Vertex(0.0, 0.0, 0.0);

        for (int y = 0 ; y < triangle_list.size(); y++)
        {
            //cout << "In x == " << x << " In y == " << y << endl;
            if (vertice_list[x].equal(vertice_list[triangle_list[y].v1]) || vertice_list[x].equal(vertice_list[triangle_list[y].v2]) || vertice_list[x].equal(vertice_list[triangle_list[y].v3]))
            {
                num++;
                sum_normal = sum_normal + triangle_list[y].triangle_normal;
                
            }
        }
        
        cout << "sum_normal == " << endl;
        sum_normal.printPoint();
        cout << "num == " << num << endl;
        vertice_list[x].normal[0] = sum_normal.x / num;
        vertice_list[x].normal[1] = sum_normal.y / num;
        vertice_list[x].normal[2] = sum_normal.z / num;
    }
}
*/

// function to calculate normal of each vertex
void calculateNormalForEachVertex()
{
    //traverse triangle_list to calculate the normal for each triangle based on three vertex of each triangle
    for (int x = 0 ; x < triangle_list.size(); x++)
    {
        triangle_list[x].triangle_normal = calculateNormal_Vector(vertice_list[triangle_list[x].v1], vertice_list[triangle_list[x].v2], vertice_list[triangle_list[x].v3]);
    }
    
    // traverse the vertice_list to calculate the normal for each vertex, get the average
    for (int x = 0; x < vertice_list.size(); x++)
    {
        Vertex sum_normal = Vertex(0.0, 0.0, 0.0);
        
        for(int y = 0; y < vertice_list[x].common_triangle.size(); y++)
        {
            sum_normal = sum_normal + triangle_list[vertice_list[x].common_triangle.at(y)].triangle_normal;
        }
        vertice_list[x].normal[0] = sum_normal.x / vertice_list[x].common_triangle.size();
        vertice_list[x].normal[1] = sum_normal.y / vertice_list[x].common_triangle.size();
        vertice_list[x].normal[2] = sum_normal.z / vertice_list[x].common_triangle.size();
    }
    
}


// get_ambinet_color based on the surface color and the space color
void get_ambient_color(float* ambinet_color)
{
    float temp[3];
    for(int x = 0; x < 3; x++)
    {
        temp[x] = space_color[x];
        temp[x] = temp[x] / 10;
    }
    //traverse the array for the R,G,B color
    for(int x = 0; x < 3; x++)
    {
        ambinet_color[x] = temp[x] * surface_color[x];
    }
}

// get_diffuse_color based on the diffuse color and the normal for that vertex and that vertex
void get_diffuse_color(float* diffuse_color, Vertex normal, Vertex v)
{
    Vertex light_vector = Vertex(light_pos[0] - v.x, light_pos[1] - v.y, light_pos[2] - v.z); //compute light vector
    light_vector.normalization();
    normal.normalization();
    float value = normal.dotProduct(light_vector);
    if (value < 0)
        value = 0;
    //traverse the array for the R,G,B color
    for(int x = 0; x < 3; x++)
    {
        diffuse_color[x] = surface_color[x] * light_color[x] * value;
    }
}

// get_phong_color based on the phong color and the normal for that vertex and that vertex
void get_phong_color(float* phong_color, Vertex normal, Vertex v)
{
    Vertex light_vector = Vertex(light_pos[0] - v.x, light_pos[1] - v.y, light_pos[2] - v.z); // compute light vector
    Vertex eye_lookat = Vertex(eye_position[0] - v.x, eye_position[1] - v.y, eye_position[2] - v.z); // compute view vector
    light_vector.normalization();
    eye_lookat.normalization();
    normal.normalization();
    Vertex reflect_vector = normal * (normal.dotProduct(light_vector)) * 2 - light_vector; // compute reflect vector
    reflect_vector.normalization();
    float value = eye_lookat.dotProduct(reflect_vector);
    if (value < 0)
        value = 0;
    for(int x = 0; x < 3; x++)
    {
        phong_color[x] = light_color[x] * specular_color[x] * pow(value, phong_exponent);
    }
}

// function to get the color based on diffuse color, ambonet color and phong color
void getColor(Vertex v, Vertex normal)
{
    //call get_diffuse_color()
    //call get_ambinet_color()
    //call get_phong_color()
    // for each time need to clear the color_to_draw
    color_to_draw[0] = 0;
    color_to_draw[1] = 0;
    color_to_draw[2] = 0;
    float ambinet_color[3];
    float diffuse_color[3];
    float phong_color[3];
    get_ambient_color(ambinet_color);
    get_diffuse_color(diffuse_color, normal, v);
    get_phong_color(phong_color, normal, v);
    if (diffue_ambinet_phong && !only_diffuse_ambinet)
    {
        color_to_draw[0] = ambinet_color[0] + diffuse_color[0] + phong_color[0]; //red color
        color_to_draw[1] = ambinet_color[1] + diffuse_color[1] + phong_color[1]; //blue color
        color_to_draw[2] = ambinet_color[2] + diffuse_color[2] + phong_color[2]; //green color
    }
    else if (!diffue_ambinet_phong && only_diffuse_ambinet)
    {
        color_to_draw[0] = ambinet_color[0] + diffuse_color[0];
        color_to_draw[1] = ambinet_color[1] + diffuse_color[1];
        color_to_draw[2] = ambinet_color[2] + diffuse_color[2];
    }
}

//function to displat the object
void drawTriangles()
{
    //calculateNormal();
    calculateNormalForEachVertex(); // get each vertex's normal vector
    for (int x = 0 ; x < triangle_list.size(); x++)
    {
        
        if (real3DLook)
        {
            //Vertex normal = calculateNormal_Vector(vertice_list[triangle_list[x].v1], vertice_list[triangle_list[x].v2], vertice_list[triangle_list[x].v3]);
            glBegin(GL_LINE_LOOP); // display the object without surface
            if (need_sharding)
            {
                getColor(vertice_list[triangle_list[x].v1], Vertex(vertice_list[triangle_list[x].v1].normal[0], vertice_list[triangle_list[x].v1].normal[1], vertice_list[triangle_list[x].v1].normal[2])); // get the final color to draw
                //getColor(vertice_list[triangle_list[x].v1], normal);
                glColor3f(color_to_draw[0], color_to_draw[1], color_to_draw[2]); // specify the color based on the color_to_draw array
                glVertex3f(vertice_list[triangle_list[x].v1].x, vertice_list[triangle_list[x].v1].y, vertice_list[triangle_list[x].v1].z);
                
                getColor(vertice_list[triangle_list[x].v2], Vertex(vertice_list[triangle_list[x].v2].normal[0], vertice_list[triangle_list[x].v2].normal[1], vertice_list[triangle_list[x].v2].normal[2]));
                //getColor(vertice_list[triangle_list[x].v2],normal);
                glColor3f(color_to_draw[0], color_to_draw[1], color_to_draw[2]);
                glVertex3f(vertice_list[triangle_list[x].v2].x, vertice_list[triangle_list[x].v2].y, vertice_list[triangle_list[x].v2].z);
                
                getColor(vertice_list[triangle_list[x].v3], Vertex(vertice_list[triangle_list[x].v3].normal[0], vertice_list[triangle_list[x].v3].normal[1], vertice_list[triangle_list[x].v3].normal[2]));
                //getColor(vertice_list[triangle_list[x].v3], normal);
                glColor3f(color_to_draw[0], color_to_draw[1], color_to_draw[2]);
                glVertex3f(vertice_list[triangle_list[x].v3].x, vertice_list[triangle_list[x].v3].y, vertice_list[triangle_list[x].v3].z);
            }
            else
            {
                glVertex3f(vertice_list[triangle_list[x].v1].x, vertice_list[triangle_list[x].v1].y, vertice_list[triangle_list[x].v1].z);
                glVertex3f(vertice_list[triangle_list[x].v2].x, vertice_list[triangle_list[x].v2].y, vertice_list[triangle_list[x].v2].z);
                glVertex3f(vertice_list[triangle_list[x].v3].x, vertice_list[triangle_list[x].v3].y, vertice_list[triangle_list[x].v3].z);
            }
            glEnd();
        }
        else
        {
            // for each triangle to calculate the normal vector
            //Vertex normal = calculateNormal_Vector(vertice_list[triangle_list[x].v1], vertice_list[triangle_list[x].v2], vertice_list[triangle_list[x].v3]);
            glBegin(GL_TRIANGLES); // display the object with surface
            if (need_sharding)
            {
                // for each vertex call getColor() to specify its RGB color
               getColor(vertice_list[triangle_list[x].v1], Vertex(vertice_list[triangle_list[x].v1].normal[0], vertice_list[triangle_list[x].v1].normal[1], vertice_list[triangle_list[x].v1].normal[2])); // get the final color to draw
                //getColor(vertice_list[triangle_list[x].v1], normal);
                glColor3f(color_to_draw[0], color_to_draw[1], color_to_draw[2]);// specify the color based on the color_to_draw array
                glVertex3f(vertice_list[triangle_list[x].v1].x, vertice_list[triangle_list[x].v1].y, vertice_list[triangle_list[x].v1].z);
                
                getColor(vertice_list[triangle_list[x].v2], Vertex(vertice_list[triangle_list[x].v2].normal[0], vertice_list[triangle_list[x].v2].normal[1], vertice_list[triangle_list[x].v2].normal[2]));
                //getColor(vertice_list[triangle_list[x].v2], normal);
                glColor3f(color_to_draw[0], color_to_draw[1], color_to_draw[2]);
                glVertex3f(vertice_list[triangle_list[x].v2].x, vertice_list[triangle_list[x].v2].y, vertice_list[triangle_list[x].v2].z);
                
                getColor(vertice_list[triangle_list[x].v3], Vertex(vertice_list[triangle_list[x].v3].normal[0], vertice_list[triangle_list[x].v3].normal[1], vertice_list[triangle_list[x].v3].normal[2]));
                //getColor(vertice_list[triangle_list[x].v3], normal);
                glColor3f(color_to_draw[0], color_to_draw[1], color_to_draw[2]);
                glVertex3f(vertice_list[triangle_list[x].v3].x, vertice_list[triangle_list[x].v3].y, vertice_list[triangle_list[x].v3].z);
            }
            else
            {
                glVertex3f(vertice_list[triangle_list[x].v1].x, vertice_list[triangle_list[x].v1].y, vertice_list[triangle_list[x].v1].z);
                glVertex3f(vertice_list[triangle_list[x].v2].x, vertice_list[triangle_list[x].v2].y, vertice_list[triangle_list[x].v2].z);
                glVertex3f(vertice_list[triangle_list[x].v3].x, vertice_list[triangle_list[x].v3].y, vertice_list[triangle_list[x].v3].z);
            }
            glEnd();
        }
    }
}

void onDisplay()
{
    //cout << "ex == " << ox << " ey == " << oy << " ez == " << oz << endl;
    //for each iteration, clear the canvas
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    /*glMatrixMode(GL_PROJECTION);*/
    glMatrixMode(GL_PROJECTION); //define the camera matrix model
    //
    glLoadIdentity(); //generate a matrix
    
    
    if (projection == 0) //parallel projection
    {
        
        glOrtho(-0.5, 0.5, -0.5 , 0.5, -100, 100);//geneterat intrinsics
        //Mp = I * [6/640, 0, 0, +3 * ratio_x;]
        //         [0,    6/480, 0, -3 * ratio_y]
        //         [0,    0,     1,     0]
        
        
    }
    else  //perspective
        gluPerspective(60.0,  (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, .1, 100.0);
    
    /* About Shading */
//    if(shading == 1)
//    {
//        light_position[0] = light_x;
//        light_position[1] = light_y;
//        light_position[2] = light_z;
//        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//        glMaterialfv(GL_FRONT, GL_AMBIENT, material_color3);
//        glEnable(GL_LIGHT0);
//        glEnable(GL_LIGHTING);
//
//        if(phong == 1)
//        {
//
//            glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.2);
//            glMaterialfv(GL_FRONT, GL_AMBIENT, material_color1);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, material_color3);
//            glMaterialfv(GL_FRONT, GL_SHININESS, low_sh);
//        }else{
//        }
//
//
//    }
//    else{
//        glDisable(GL_LIGHT0);
//        glDisable(GL_LIGHTING);
//    }
    
    
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //generate a matrix M_m = I
    
    gluLookAt(ex, ey, ez, ox, oy, ox, upx, upy, upz);
    //gluLookAt(0, 0, 5, 0, 1, 0, 0, 1, 0);
    //ex, ey, ez - camera center position
    //ox, oy, oz - camera orientation, specify the point your k
    // eye is looking at.
    //upx, upy, upz - positive y direction
    //static GLdouble ex = .0, ey = 0.0, ez = 5.0, upx = .0, upy = 1.0, upz = .0, ox = 0.0, oy = 0.0, oz;
    
    
    
    //glMaterialfv(GL_FRONT, GL_DIFFUSE, material_color1);
    glPushMatrix();
    glTranslated(0.0, 0.0, 0.0); //generate a matrix
    glColor3f(0.5, 0.5, 0.5);
    
    drawTriangles();
    
    //glutSolidTeapot(size); // define the object size
    //glEnd();
    glutSwapBuffers();//display the buffer

}




//****************************************************************************
//Name: onMouse
//Description: The mouse event callback
//Function calls: buttonPushed, glutPostResdisplay
//Preconditions: x,y is a valid point
//Postconditions: The mouse event is handled
//Parameters: GLint x, y, the mouse position
//                button, the mouse buttons used
//                state, the properties of the mouse
//                GLUT_UP, GLUT_DOWN, GLUT_LEFT_BUTTON
//                GLUT_RIGHT_BUTTON,
//Returns: Nothing
void onMouse(int button, int state, int x, int y)
{
    
    GLint specialKey = glutGetModifiers();
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                mouse_down = 1;
                current_x = x;
                current_y = y;
                
                if (specialKey == GLUT_ACTIVE_SHIFT)
                {
                    //should do the translation
                    needToRotateWithCenter = false;
                    change_mode = 1;
                    Vertex begin_point = Vertex(current_x, current_y, 0.0);
                    point_queue.push(begin_point);
                }
                else if (needToRotateWithCenter)
                {
                    // should rotate with center point of the object
                    Vertex begin_point = Vertex(current_x, current_y, 0.0);
                    point_queue.push(begin_point);
                    change_mode = 0;
                }
                else
                {
                    //should do the rotation ratate about x axis, y axis or z axis
                    needToRotateWithCenter = false;
                    change_mode = 0;
                    Vertex begin_point = Vertex(current_x, current_y, 0.0);
                    point_queue.push(begin_point);
                }
            }
            else if (state == GLUT_UP)
            {
                mouse_down = 0;
                // empty the queue
                needToRotateWithCenter = false;
                needToRotateWithZ = false;
                while( !point_queue.empty()) // clear the point_queue for next operation
                    point_queue.pop();
            }
            break;
            
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
                
                break;
        default:
            break;
            
    }
    
}

void onResize(GLint w, GLint h)
{
    //glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective (65.0, (GLfloat) w/(GLfloat) h, 1.0, 100.0);
    
}


void onIdle()
{
    
}

//function to find the object center point for rotate with center location of the object
void findCenterPoint()
{
    double sum_x = 0;
    double sum_y = 0;
    double sum_z = 0;
    center_pt_x = 0;
    center_pt_y = 0;
    center_pt_z = 0;
    for (int x = 0; x < vertice_list.size(); x++)
    {
        sum_x += vertice_list[x].x;
        sum_y += vertice_list[x].y;
        sum_z += vertice_list[x].z;
    }
    center_pt_x = sum_x / vertice_list.size();
    center_pt_y = sum_y / vertice_list.size();
    center_pt_z = sum_z / vertice_list.size();
}

// function to update the vertice_list when do the translation
void updateVertexForTranslation(Mat translation_mat)
{
    for(int x = 0; x < vertice_list.size(); x++)
    {
        Vertex v = translation_mat * vertice_list[x]; // update each vertex
        vertice_list[x] = v;
    }
}

// function to update the vertice_list when rotate with x,y or z axis
void updateVertexForRotation(Mat x_rotation_mat, Mat y_rotation_mat, Mat z_rotation_mat)
{
    Mat transform_mat = z_rotation_mat * y_rotation_mat * x_rotation_mat; // calculate the transform matrix
    
    for(int x = 0; x < vertice_list.size(); x++)
    {
        Vertex v = transform_mat * vertice_list[x];
        vertice_list[x] = v;
    }

}

// function to update the vertice_list when rotate with center location of the object
void updateVertexForRotationWithCenter(Mat x_rotation_mat, Mat y_rotation_mat, Mat z_rotation_mat, Mat translateToOri, Mat translateToCenter)
{
    // calculate the transform matrix, first translate the object's center to the original point then do the rotation then translate back
    Mat transform_mat = translateToCenter * z_rotation_mat * y_rotation_mat * x_rotation_mat * translateToOri;
    for(int x = 0; x < vertice_list.size(); x++)
    {
        Vertex v = transform_mat * vertice_list[x];
        vertice_list[x] = v;
    }
}

void onMouseMotion(int x, int y)
{
    if (mouse_down == 1)
    {
        if (change_mode == 0)
        {
            
            // do the rotation
            Vertex move_point = Vertex(double(x), double(y), 0);
            point_queue.push(move_point);
            
            if (point_queue.size() == 3)
            {
                point_queue.pop();
            }

            Vertex begin_pt = point_queue.front();
            Vertex end_pt = point_queue.back();
            
            double angle_x = (end_pt.y - begin_pt.y) / 2; // to calculate the angle rotate about x axis
            double angle_y = (end_pt.x - begin_pt.x) / 2; // to calculate the angle rotate about y axis
            double angle_z = 0;
            
            if (needToRotateWithZ) // to calculate the angle when rotate about z axis
            {
                angle_x = 0;
                angle_y = 0;
                angle_z = -(end_pt.x - begin_pt.x) / 2;
            }
            
            // initial the x rotation matrix
            double array_x[4][4] = {{1,0,0,0},
                                   {0,cos(angle_x * degree),-sin(angle_x * degree),0},
                                   {0,sin(angle_x * degree),cos(angle_x * degree),0},
                                   {0,0,0,1}};
            Mat x_rotation_matrix = Mat(array_x);
            
            // initial the y rotation matrix
            double array_y[4][4] = {{cos(angle_y * degree),0,sin(angle_y * degree),0},
                                    {0,1,0,0},
                                    {-sin(angle_y * degree),0,cos(angle_y * degree),0},
                                    {0,0,0,1}};
            Mat y_rotation_matrix = Mat(array_y);
            
            // initial the z rotation matrix
            double array_z[4][4] = {{cos(angle_z * degree),-sin(angle_z * degree),0,0},{sin(angle_z * degree),cos(angle_z * degree),0,0},{0,0,1,0},{0,0,0,1}};
            Mat z_rotation_matrix = Mat(array_z);
            
            // when rotate with center location of the object
            if (needToRotateWithCenter)
            {
                
                findCenterPoint(); // find the center point
                
                // to find the matrix to back to Ori_pt
                double array1[4][4] = {{1,0,0,-center_pt_x},{0,1,0,-center_pt_y},{0,0,1,-center_pt_z},{0,0,0,1}};
                Mat translateToOri = Mat(array1);
                
                // to find the matrix to back to center
                double array2[4][4] = {{1,0,0,center_pt_x},{0,1,0,center_pt_y},{0,0,1,center_pt_z},{0,0,0,1}};
                Mat translateToCenter = Mat(array2);

                updateVertexForRotationWithCenter(x_rotation_matrix, y_rotation_matrix, z_rotation_matrix, translateToOri, translateToCenter);
            }
            else
            {
                // when rotate about x,y or z axis
                updateVertexForRotation(x_rotation_matrix, y_rotation_matrix, z_rotation_matrix);
            }
            
            glutPostRedisplay();
        }
        else
        {
            // do the translation
            Vertex move_point = Vertex(double(x), double(y), 0);
            point_queue.push(move_point);
            if (point_queue.size() == 3)
            {
                point_queue.pop();
            }
            Vertex begin_pt = point_queue.front();
            Vertex end_pt = point_queue.back();
            // find the translation matrix
            double distance_x = (end_pt.x - begin_pt.x) / 500;
            double distance_y = (end_pt.y - begin_pt.y) / 500;
            double array[4][4] = {{1,0,0,distance_x},{0,1,0,-distance_y},{0,0,1,0},{0,0,0,1}};
            Mat translation_mat = Mat(array);
            updateVertexForTranslation(translation_mat);
            glutPostRedisplay();
            
        }
        current_x = x;
        current_y = y;
    }
    glutPostRedisplay();
}

void onKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        //bool only_diffuse_ambinet = false;
        //bool diffue_ambinet_phong = false;
        case 27:
            exit(1);
            break;
        case 48:  // number 0
            need_sharding = false;
            only_diffuse_ambinet = false;
            diffue_ambinet_phong = false;
            glutPostRedisplay();
            break;
        case 49: // number 1
            need_sharding = true;
            only_diffuse_ambinet = true;
            diffue_ambinet_phong = false;
            glutPostRedisplay();
            break;
        case 50: // number 2
            need_sharding = true;
            only_diffuse_ambinet = false;
            diffue_ambinet_phong = true;
            glutPostRedisplay();
            break;
        case 'p':
            projection = (projection == 0) ? 1 : 0;
            glutPostRedisplay();
            break;
//        case 'z':
//            ez += 0.2;
//            glutPostRedisplay();
//            printf("Z: %.3f\n", ez);
//            break;
        case 'x':
            ez -= 0.2;
            glutPostRedisplay();
            printf("Z: %.3f\n", ez);
            break;
            
        case 's':
            shading = (shading == 0)?1:0;
            glutPostRedisplay();
            break;
            
        case 'o':
            phong = (phong == 0)?1:0;
            glutPostRedisplay();
            break;
            
        case 'i':
            cout << "before increase szie == " << size << endl;
            size += 0.2;
            cout << "after increase szie == " << size << endl;
            glutPostRedisplay();
            break;
        case 'd':
            cout << "before decrease szie == " << size << endl;
            size -= 0.2;
            cout << "after decrease szie == " << size << endl;
            glutPostRedisplay();
            break;
        case 'm':
            if (real3DLook)
            {
                real3DLook = false;
                glutPostRedisplay();
            }
            else
            {
                real3DLook = true;
                glutPostRedisplay();
            }
            break;
        case 'M':
            if (real3DLook)
            {
                real3DLook = false;
                glutPostRedisplay();
            }
            else
            {
                real3DLook = true;
                glutPostRedisplay();
            }
            break;
        case 'l':
            fileName = "bunny_low.obj";
            readFile(fileName);
            glutPostRedisplay();
            break;
        case 'h':
            fileName = "bunny_high.obj";
            readFile(fileName);
            glutPostRedisplay();
            break;
        case 'c':
            needToRotateWithCenter = true;
            //cout << "need to translate with center" << endl;
            break;
        case 'C':
            needToRotateWithCenter = true;
            break;
        case 'z':
            if (needToRotateWithZ)
                needToRotateWithZ = false;
            else
                needToRotateWithZ = true;
            break;
        case 'Z':
            if (needToRotateWithZ)
                needToRotateWithZ = false;
            else
                needToRotateWithZ = true;
            break;
        // Press 'g' or 'G' to display the buuny as glod
        case 'g':
            phong_exponent = 0.56;
            surface_color[0] = 0.9;
            surface_color[1] = 0.7;
            surface_color[2] = 0.1;
            glutPostRedisplay();
            break;
        case 'G':
            phong_exponent = 0.56;
            surface_color[0] = 0.9;
            surface_color[1] = 0.7;
            surface_color[2] = 0.1;
            glutPostRedisplay();
            break;
        // press 'a' or 'A' to display the bunny as China
        case 'a':
            phong_exponent = 76;
            surface_color[0] = 0.8;
            surface_color[1] = 0.8;
            surface_color[2] = 0.8;
            glutPostRedisplay();
            break;
        case 'A':
            phong_exponent = 76;
            surface_color[0] = 0.8;
            surface_color[1] = 0.8;
            surface_color[2] = 0.8;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

void processSpecialKeys(int key, int x, int y)
{
    switch (key)
    {
        case 27 :      break;
        case 100 :
            light_x -= 1.2;
            glutPostRedisplay();
            
            break;
        case 102:
            light_x += 1.2;
            glutPostRedisplay();
            break;
        case 101   :
            light_y += 1.2;
            glutPostRedisplay();
            break;
        case 103 :
            light_y -= 1.2;
            glutPostRedisplay();
            break;
    }
}


int main(int argc, char** argv)
{
    readFile(fileName);
    /**********************Part 1: Always keep the same ************/
    //Initialization functions
    glutInit(&argc, argv); // do the basic set up for OpenGL
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //GLUT_DOUBLE -> is for two buffer that make two buffers make animation run smoother
    //GLUT_RGB -> is to render a colored object
    //GLUT_DEPTH -> is to enable closer object block farther object
    
    
    
    glutInitWindowSize(640, 480); // specify the window size rows, columns
    glutInitWindowPosition(0, WINDOW_HEIGHT / 2); // the location of the window when display
    glutCreateWindow("Bunny"); //The window's title
    
    
    /*************** Part 2 ***********************/
    initialize(); // This function is oprional that depends on the application
    // This name "initialize()" can be changed
    // The job of this function is just to initialize a few parameters according to your application
    
    
    /************** Part 3: Define call back functions ************/
    glutDisplayFunc(onDisplay);  // your job is define "onDisplay" function
    glutKeyboardFunc(onKeyboard); // your job is define "onKeyBoard" function
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMouseMotion);
    glutReshapeFunc(onResize);
    glutIdleFunc(onIdle); // off-line task
    // when there are no other events or the OpenGL jas free time, what you want it to do
    
    /************** Part 4: Start loop **********/
    //Infinite Loop
    glutMainLoop(); // tell OpenGL to start a loop
    
    return 1;
}
