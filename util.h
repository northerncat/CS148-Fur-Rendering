/* CS148 Summer 2014
 * Assignment 5
 * ------------
 * Name: Tsao-Wei (Brad) Huang
 * File: util.h
 * This file includes several structs and utility functions, including the parser
 * for parsing an OBJ file. The code is largelt modified from the util.h given in 
 * the starter code of HW3.
 */

#ifndef UTIL_H
#define UTIL_H
#include "math.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "stdio.h"
#include <vector>

using std::string;

// a 3D vertex
struct Vertexf {
    GLfloat x, y, z;
    Vertexf() { x = 0.0;  y = 0.0; z = 0.0; }
    Vertexf(GLfloat _x, GLfloat _y, GLfloat _z) { x = _x; y = _y; z = _z; }
};

// 3D texture coordinates as a struct. w is default 0
struct Texf {
    GLfloat u, v, w;
    Texf() { u = 0.0; v = 0.0; w = 0.0; }
    Texf(GLfloat _u, GLfloat _v) { u = _u; v = _v; w = 0.0; }
    Texf(GLfloat _u, GLfloat _v, GLfloat _w) { u = _u; v = _v; w = _w; }
};

struct Normalf {
    GLfloat dx, dy, dz;
    Normalf() { dx = 0.0;  dy = 0.0; dz = 0.0; }
    Normalf(GLfloat _dx, GLfloat _dy, GLfloat _dz) { dx = _dx; dy = _dy; dz = _dz; }
    GLfloat length() {
        return sqrt(dx*dx + dy*dy + dz*dz);
    }
    void Normalize2D() {
        dx /= sqrt(dx*dx + dy*dy); dy /= sqrt(dx*dx + dy*dy);
    }
    void Normalize() {
        dx /= this->length(); dy /= this->length(); dz /= this->length();
    }
};

// a 3D triangle specified by several vertices
struct Trianglef{
    Vertexf va, vb, vc;
    Texf tva, tvb, tvc;
    Normalf nva, nvb, nvc;
    Trianglef() {
        va = Vertexf(); vb = Vertexf(); vc = Vertexf(); 
        tva = Texf(); tvb = Texf(); tvc = Texf(); 
        nva = Normalf(); nvb = Normalf(); nvc = Normalf(); 
    }
    Trianglef(Vertexf _va, Vertexf _vb, Vertexf _vc) { va = _va; vb = _vb; vc = _vc; }
    Trianglef(Vertexf _va, Vertexf _vb, Vertexf _vc, Texf _tva, Texf _tvb, Texf _tvc) {
        va = _va; vb = _vb; vc = _vc; 
        tva = _tva; tvb = _tvb; tvc = _tvc; 
        nva = Normalf(); nvb = Normalf(); nvc = Normalf(); 
    }
    Trianglef(Vertexf _va, Vertexf _vb, Vertexf _vc, Normalf _nva, Normalf _nvb, Normalf _nvc) {
        va = _va; vb = _vb; vc = _vc; 
        tva = Texf(); tvb = Texf(); tvc = Texf(); 
        nva = _nva; nvb = _nvb; nvc = _nvc; 
    }
    Trianglef(Vertexf _va, Vertexf _vb, Vertexf _vc, Texf _tva, Texf _tvb, Texf _tvc, Normalf _nva, Normalf _nvb, Normalf _nvc) {
        va = _va; vb = _vb; vc = _vc; 
        tva = _tva; tvb = _tvb; tvc = _tvc; 
        nva = _nva; nvb = _nvb; nvc = _nvc; 
    }
};

// this function clamps a value to a range between a min and a max
float clamp(float val, float min, float max) {
    if (val > max) {
        return max;
    } else if (val < min) {
        return min;
    } else return val;
}

// this function counts the number of a character ch's occurrences in string s
int countChar(string s, char ch) {
    int n = 0;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == ch) n++;
    }
    return n;
}

// This function loads the mesh information into a vector of triangles.
// The parser reads the OBJ file line by line to extract information.
void loadMesh(std::ifstream &file, std::vector<Trianglef> &faces) {
    if (file.is_open()) {
        std::vector<Vertexf> vertices;
        std::vector<Texf> textures;
        std::vector<Normalf> normals;
        char type;
        float f1, f2, f3, nf1, nf2, nf3, tf1, tf2, tf3;
        int v1, v2, v3, t1, t2, t3, n1, n2, n3;
        Vertexf va, vb, vc;
        Texf ta, tb, tc;
        Normalf na, nb, nc;
        string line;
        int nLines = 0;
        while (std::getline(file, line)) {
            nLines++;
            std::istringstream lstream(line);
            lstream >> type;
            tf3 = 0.0;
            if (type == 'v') {
                if (line[1] == ' ') { // vertex case
                    lstream >> f1; lstream >> f2; lstream >> f3;
                    vertices.push_back(Vertexf(f1, f2, f3));
                } else if (line[1] == 't') { // texture coordinate case
                    sscanf(line.c_str(), "%*s %f %f %f", &tf1, &tf2, &tf3);
                    textures.push_back(Texf(tf1, tf2, tf3));
                } else if (line[1] == 'n') { // normal case
                    sscanf(line.c_str(), "%*s %f %f %f", &nf1, &nf2, &nf3);
                    normals.push_back(Normalf(nf1, nf2, nf3));
                }
            } else if (type == 'f') { // face case
                int nSlash = countChar(line, '/');
                if (nSlash == 0) { // only vertex case
                    lstream >> v1; lstream >> v2; lstream >> v3;
                    va = vertices.at(v1-1); vb = vertices.at(v2-1); vc = vertices.at(v3-1);
                    faces.push_back(Trianglef(va, vb, vc));
                } else if (nSlash == 3) { // vertex and texture case
                    lstream >> v1; lstream >> t1; 
                    lstream >> v2; lstream >> t2; 
                    lstream >> v3; lstream >> t3;
                    va = vertices.at(v1-1); vb = vertices.at(v2-1); vc = vertices.at(v3-1);
                    ta = textures.at(t1-1); tb = textures.at(t2-1); tc = textures.at(t3-1);
                    faces.push_back(Trianglef(va, vb, vc, ta, tb, tc));
                } else {
                    if (sscanf(line.c_str(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3) == 6) {
                        va = vertices.at(v1-1); vb = vertices.at(v2-1); vc = vertices.at(v3-1);
                        na = normals.at(n1-1); nb = normals.at(n2-1); nc = normals.at(n3-1);
                        faces.push_back(Trianglef(va, vb, vc, na, nb, nc));
                    } else {
                        va = vertices.at(v1-1); vb = vertices.at(v2-1); vc = vertices.at(v3-1);
                        ta = textures.at(t1-1); tb = textures.at(t2-1); tc = textures.at(t3-1);
                        na = normals.at(n1-1); nb = normals.at(n2-1); nc = normals.at(n3-1);
                        faces.push_back(Trianglef(va, vb, vc, ta, tb, tc, na, nb, nc));
                    }
                }
            }
        }
    }
}

// This function checks the filename and open the corresponding obj file
bool openOBJ(string filename, std::vector<Trianglef> &faces) {
    // peek at the extension
    std::string::size_type idx;
    idx = filename.rfind('.');

    if (idx != std::string::npos) {
        std::string extension = filename.substr(idx + 1);

        if (extension == "obj") {
            faces.clear();
            std::ifstream file;
            file.open(filename.c_str());
            loadMesh(file, faces);
            return true;
        } else {
            std::cerr << "ERROR: unable to load file " << filename << "  -- unknown extension." << std::endl;
            std::cerr << "Input only .obj files" << std::endl;
        }
    }

    // No filename extension found, or none matching {.obj}
    return false;
}

#endif