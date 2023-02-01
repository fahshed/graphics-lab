#include "iostream"

#include <windows.h>
#include <glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>

using namespace std;

#define pi (2 * acos(0.0))
#define INFINITY 99999

class Point
{
public:
    double x, y, z;

    Point()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Point add(Point p)
    {
        return Point(this->x + p.x, this->y + p.y, this->z + p.z);
    };

    Point mult(double value)
    {
        return Point(this->x * value, this->y * value, this->z * value);
    };

    Point crossProduct(Point p)
    {
        Point product;
        product.x = this->y * p.z - this->z * p.y;
        product.y = this->z * p.x - this->x * p.z;
        product.z = this->x * p.y - this->y * p.x;
        return product;
    };

    double dotProduct(Point p)
    {
        return this->x * p.x + this->y * p.y + this->z * p.z;
    };

    Point rotateArround(Point k, double theta)
    {
        Point cross = k.crossProduct(*this);
        // return add(mult(v, cos(theta)), mult(cross, sin(theta)));
        return this->mult(cos(theta)).add(cross.mult(sin(theta)));
    };

    void normalize()
    {
        double mod = sqrt(x * x + y * y + z * z);
        this->x = x / mod;
        this->y = y / mod;
        this->z = z / mod;
    }

    void printPoint()
    {
        cout << this->x << " " << this->y << " " << this->z << endl;
    }
};

class Determinant
{
    Point r1, r2, r3;

public:
    Determinant(Point r1, Point r2, Point r3)
    {
        this->r1 = r1;
        this->r2 = r2;
        this->r3 = r3;
    }

    double value()
    {
        return r1.x * (r2.y * r3.z - r3.y * r2.z) - r1.y * (r2.x * r3.z - r3.x * r2.z) + r1.z * (r2.x * r3.y - r3.x * r2.y);
    }
};

class Ray
{
public:
    Point start;
    Point dir;

    Ray(Point start, Point dir)
    {
        this->start = start;
        this->dir = dir;
    }
};

class Object
{
public:
    Point referencePoint;
    double height, width, length;
    double color[3];
    double coEfficients[4];
    double shine;

    Object() {}

    void setColor(double r, double g, double b)
    {
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
    }

    void setShine(double shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double ambient, double diffuse, double specular, double reflection)
    {
        this->coEfficients[0] = ambient;
        this->coEfficients[1] = diffuse;
        this->coEfficients[2] = specular;
        this->coEfficients[3] = reflection;
    }

    virtual void draw() {}

    virtual double intersect(Ray r, double *color, int level)
    {
        return -1.0;
    }
};

class Light
{
public:
    Point position;
    double color[3];

    Light(Point pos, double r, double g, double b)
    {
        this->position = pos;
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
    }

    void draw()
    {
        int stacks = 20;
        int slices = 20;
        double radius = 1;
        Point points[100][100];

        //generate points
        int i, j;
        double h, r;
        for (i = 0; i <= stacks; i++)
        {
            h = radius * sin(((double)i / (double)stacks) * (pi / 2));
            r = radius * cos(((double)i / (double)stacks) * (pi / 2));
            for (j = 0; j <= slices; j++)
            {
                points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
                points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
                points[i][j].z = h;
            }
        }
        //draw quads using generated points
        glPushMatrix();
        {
            glTranslatef(this->position.x, this->position.y, this->position.z);
            glColor3f(this->color[0], this->color[1], this->color[2]);
            for (i = 0; i < stacks; i++)
            {
                for (j = 0; j < slices; j++)
                {
                    glBegin(GL_QUADS);
                    {
                        //upper hemisphere
                        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
                        //lower hemisphere
                        glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
                    }
                    glEnd();
                }
            }
        }
        glPopMatrix();
    }
};

extern vector<Object *> objects;
extern vector<Light> lights;

Point getReflection(Point ray, Point normal)
{
    Point reflection = ray.add(normal.mult(-2 * ray.dotProduct(normal)));
    // Point reflection = normal.mult(2 * ray.dotProduct(normal)).add(ray.mult(-1));
    reflection.normalize();
    return reflection;
}

bool isObsecured(Ray L)
{
    for (int j = 0; j < objects.size(); j++)
    {
        double t = objects[j]->intersect(L, nullptr, 0);
        if (t != INFINITY)
        {
            return true;
        }
    }
    return false;
}

class Sphere : public Object
{
public:
    Sphere(Point center, double radius)
    {
        this->referencePoint = center;
        this->length = radius;
    }

    void draw()
    {
        int slices = 70;
        int stacks = 25;
        Point points[100][100];
        int i, j;
        double h, r;
        //generate points
        for (i = 0; i <= stacks; i++)
        {
            h = this->length * sin(((double)i / (double)stacks) * (pi / 2));
            r = this->length * cos(((double)i / (double)stacks) * (pi / 2));
            for (j = 0; j <= slices; j++)
            {
                points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
                points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
                points[i][j].z = h;
            }
        }
        //draw quads using generated points
        glPushMatrix();
        {
            glTranslatef(this->referencePoint.x, this->referencePoint.y, this->referencePoint.z);
            glColor3f(this->color[0], this->color[1], this->color[2]);
            for (i = 0; i < stacks; i++)
            {
                for (j = 0; j < slices; j++)
                {
                    glBegin(GL_QUADS);
                    {
                        //upper hemisphere
                        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
                        //lower hemisphere
                        glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
                    }
                    glEnd();
                }
            }
        }
        glPopMatrix();
    }

    double intersect(Ray r, double *color, int level)
    {
        Point Ro = r.start.add(this->referencePoint.mult(-1));
        Point Rd = r.dir;

        double a = 1;
        double b = 2 * Rd.dotProduct(Ro);
        double c = Ro.dotProduct(Ro) - (this->length * this->length);

        double d = b * b - 4 * a * c;

        if (d < 0)
            return INFINITY;

        double t;
        double tPLUS = (-b + sqrt(d)) / 2 * a;
        double tMINUS = (-b - sqrt(d)) / 2 * a;

        if (tMINUS > 0)
            t = tMINUS;
        else if (tPLUS > 0)
            t = tPLUS;
        else
            return INFINITY;

        if (level != 0)
        {
            Point intersection = Ro.add(Rd.mult(t));
            applyColor(color, intersection, Rd);
        }

        return t;
    }

    void applyColor(double *color, Point intersection, Point ray)
    {
        color[0] = this->color[0] * this->coEfficients[0];
        color[1] = this->color[1] * this->coEfficients[0];
        color[2] = this->color[2] * this->coEfficients[0];

        // for (int i = 0; i < lights.size(); i++)
        // {
        //     Point lDir = lights[i].position.add(intersection.mult(-1));
        //     lDir.normalize();
        //     Point lStart = intersection.add(lDir.mult(1));
        //     Ray L(lStart, lDir);

        //     if (isObsecured(L))
        //         continue;

        //     // Point l = lDir.mult(-1);
        //     Point normal = getNormal(intersection);
        //     double lambert = lDir.dotProduct(normal);
        //     double phong = ray.dotProduct(getReflection(lDir, normal));

        //     for (int k = 0; k < 3; k++)
        //     {
        //         color[k] += lights[i].color[k] * lambert * coEfficients[1] * this->color[k];
        //         color[k] += lights[i].color[k] * pow(phong, this->shine) * coEfficients[2] * this->color[k];
        //     }
        // }
    }

    Point getNormal(Point p)
    {
        Point n = p.add(this->referencePoint.mult(-1));
        n.normalize();
        return n;
    }
};

class Triangle : public Object
{
public:
    Point vertices[3];

    Triangle(Point a, Point b, Point c)
    {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
    }

    void draw()
    {
        glColor3f(this->color[0], this->color[1], this->color[2]);
        glBegin(GL_TRIANGLES);
        {
            for (int i = 0; i < 3; i++)
            {
                glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
            }
        }
        glEnd();
    }

    double intersect(Ray r, double *color, int level)
    {
        Point a = vertices[0];
        Point b = vertices[1];
        Point c = vertices[2];
        Point Ro = r.start;
        Point Rd = r.dir;

        Determinant betaDet(Point(a.x - Ro.x, a.x - c.x, Rd.x), Point(a.y - Ro.y, a.y - c.y, Rd.y), Point(a.z - Ro.z, a.z - c.z, Rd.z));
        Determinant gamaDet(Point(a.x - b.x, a.x - Ro.x, Rd.x), Point(a.y - b.y, a.y - Ro.y, Rd.y), Point(a.z - b.z, a.z - Ro.z, Rd.z));
        Determinant tDet(Point(a.x - b.x, a.x - c.x, a.x - Ro.x), Point(a.y - b.y, a.y - c.y, a.y - Ro.y), Point(a.z - b.z, a.z - c.z, a.z - Ro.z));
        Determinant aDet(Point(a.x - b.x, a.x - c.x, Rd.x), Point(a.y - b.y, a.y - c.y, Rd.y), Point(a.z - b.z, a.z - c.z, Rd.z));

        double A = aDet.value();
        if (A == 0)
            return INFINITY;

        double beta = betaDet.value() / A;
        double gama = gamaDet.value() / A;
        double t = tDet.value() / A;

        if ((beta + gama) < 1 && beta > 0 && gama > 0 && t > 0)
        {
            Point intersection = Ro.add(Rd.mult(t));

            if (level != 0)
            {
                applyColor(color, intersection, Rd);
            }

            return t;
        }

        return INFINITY;
    }

    void applyColor(double *color, Point intersection, Point ray)
    {
        color[0] = this->color[0] * this->coEfficients[0];
        color[1] = this->color[1] * this->coEfficients[0];
        color[2] = this->color[2] * this->coEfficients[0];

        // for (int i = 0; i < lights.size(); i++)
        // {
        //     Point lDir = lights[i].position.add(intersection.mult(-1));
        //     lDir.normalize();
        //     Point lStart = intersection.add(lDir.mult(1));
        //     Ray L(lStart, lDir);

        //     if (isObsecured(L))
        //         continue;

        //     // Point l = lDir.mult(-1);
        //     Point normal = getNormal(intersection);
        //     double lambert = lDir.dotProduct(normal);
        //     double phong = ray.dotProduct(getReflection(lDir, normal));

        //     for (int k = 0; k < 3; k++)
        //     {
        //         color[k] += lights[i].color[k] * lambert * coEfficients[1] * this->color[k];
        //         color[k] += lights[i].color[k] * pow(phong, this->shine) * coEfficients[2] * this->color[k];
        //     }
        // }
    }

    Point getNormal(Point intersection)
    {
        Point a = vertices[0];
        Point b = vertices[1];
        Point c = vertices[2];

        Point term1 = b.add(a.mult(-1));
        Point term2 = c.add(a.mult(-1));

        Point result = term1.crossProduct(term2);
        return result;
    }
};

class General : public Object
{
public:
    vector<double> constants;

    General(vector<double> constants, Point center, double length, double width, double height)
    {
        this->constants = constants;
        this->referencePoint = center;
        this->length = length;
        this->width = width;
        this->height = height;
    }

    void draw() {}

    double intersect(Ray r, double *color, int level)
    {
        Point Ro = r.start;
        Point Rd = r.dir;

        double a = (constants[0] * Rd.x * Rd.x) + (constants[1] * Rd.y * Rd.y) + (constants[2] * Rd.z * Rd.z) + (constants[3] * Rd.x * Rd.y) + (constants[4] * Rd.x * Rd.z) + (constants[5] * Rd.y * Rd.z);
        double b = (constants[0] * 2 * Ro.x * Rd.x) + (constants[1] * 2 * Ro.y * Rd.y) + (constants[2] * 2 * Ro.z * Rd.z) + (constants[3] * Ro.x * Rd.y) + (constants[3] * Ro.y * Rd.x) + (constants[4] * Ro.x * Rd.z) + (constants[4] * Ro.z * Rd.x) + (constants[5] * Ro.y * Rd.z) + (constants[5] * Ro.z * Rd.y) + (constants[6] * Rd.x) + (constants[7] * Rd.y) + (constants[8] * Rd.z);
        double c = (constants[0] * Ro.x * Ro.x) + (constants[1] * Ro.y * Ro.y) + (constants[2] * Ro.z * Ro.z) + (constants[3] * Ro.x * Ro.y) + (constants[4] * Ro.x * Ro.z) + (constants[5] * Ro.y * Ro.z) + (constants[6] * Ro.x) + (constants[7] * Ro.y) + (constants[8] * Ro.z) + constants[9];
        double d = b * b - 4 * a * c;

        if (d < 0)
            return INFINITY;

        double tPLUS = (-b + sqrt(d)) / 2 * a;
        double tMINUS = (-b - sqrt(d)) / 2 * a;

        if (level != 0)
        {
            color[0] = this->color[0] * this->coEfficients[0];
            color[1] = this->color[1] * this->coEfficients[0];
            color[2] = this->color[2] * this->coEfficients[0];
        }

        if (tMINUS > 0)
        {
            Point sect = Ro.add(Rd.mult(tMINUS));
            if (isInsideQube(sect))
            {
                return tMINUS;
            }
        }
        if (tPLUS > 0)
        {
            Point sect = Ro.add(Rd.mult(tPLUS));
            if (isInsideQube(sect))
            {
                return tPLUS;
            }
        }
        return INFINITY;
    }

    bool isInsideQube(Point sect)
    {
        Point ref = this->referencePoint;
        if (this->length != 0 && (sect.x < ref.x || abs(sect.x - ref.x) > this->length))
            return false;
        if (this->width != 0 && (sect.y < ref.y || abs(sect.y - ref.y) > this->width))
            return false;
        if (this->height != 0 && (sect.z < ref.z || abs(sect.z - ref.z) > this->height))
            return false;
        return true;
    }
};

class Floor : public Object
{
public:
    Floor(double floorWidth, double tileWidth)
    {
        this->referencePoint = Point(-floorWidth / 2, -floorWidth / 2, 0);
        this->length = tileWidth;
    }

    void draw()
    {
        double x = this->referencePoint.x;
        double y = this->referencePoint.y;
        double a = this->length / 2;
        int numberOfTiles = this->referencePoint.x * (-2) / this->length;

        glPushMatrix();
        {
            glTranslatef(x + a, y - a, 0);
            for (int i = 0; i < numberOfTiles; i++)
            {
                glPushMatrix();
                {
                    for (int j = 0; j < numberOfTiles; j++)
                    {
                        glTranslatef(0, 2 * a, 0);
                        glColor3f(this->color[0] * ((i + j) % 2), this->color[1] * ((i + j) % 2), this->color[2] * ((i + j) % 2));
                        glBegin(GL_QUADS);
                        {
                            glVertex3f(a, a, 0);
                            glVertex3f(a, -a, 0);
                            glVertex3f(-a, -a, 0);
                            glVertex3f(-a, a, 0);
                        }
                        glEnd();
                    }
                }
                glPopMatrix();
                glTranslatef(2 * a, 0, 0);
            }
        }
        glPopMatrix();
    }

    double intersect(Ray r, double *color, int level)
    {
        Point Ro = r.start;
        Point Rd = r.dir;
        Point n(0, 0, 1);
        Point ref = this->referencePoint;

        double t = -1 * n.dotProduct(Ro) / n.dotProduct(Rd);

        Point sect = Ro.add(Rd.mult(t));

        if (sect.x >= ref.x && sect.y >= ref.y && sect.x <= (ref.x * -1) && sect.y <= (ref.y * -1) && t > 0)
        {
            if (level != 0)
            {
                int i = (int)(sect.y - ref.y) / (int)(this->length);
                int j = (int)(sect.x - ref.x) / (int)(this->length);
                int alternate = (i + j) % 2;
                applyColor(alternate, color, sect, Rd);
            }
            return t;
        }
        return INFINITY;
    }

    void applyColor(int alternate, double *color, Point intersection, Point ray)
    {
        color[0] = this->color[0] * alternate * this->coEfficients[0];
        color[1] = this->color[1] * alternate * this->coEfficients[0];
        color[2] = this->color[2] * alternate * this->coEfficients[0];
    }
};
