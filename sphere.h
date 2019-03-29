#pragma once

#include <iostream>
#include <vector>
#include "math.h"
#include "hitable.h"
#include "hitable_list.h"
#include "material.h"
#include "transform.h"

using namespace std;

class sphere: public hitable {
public:
    sphere() {}
    sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat(m) {}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;

    vec3 center;
    float radius;
    material *mat;
};

class triangle: public hitable {
public:
    triangle() {}
    triangle(vec3 p0, vec3 p1, vec3 p2, material *m) {
        points[0] = p0;
        points[1] = p1;
        points[2] = p2;
        mat = m;

        origine = (p0+p1+p2)/3.0;
        normale = unit_vector(cross(p1-p0, p2-p0));
    }
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;

    vec3 points[3];
    vec3 normale;
    vec3 origine;
    material *mat;
};

class modele: public hitable {
public:
    modele() {}

    /// création du modele et de tous les triangles en utilisant les données d'entrée.
    modele(vector<vec3*> vertices, vector<vec3*> links, vector<transform*> transforms, material *m) {
        triangles = vector<triangle*>();
        mat = m;
        //cout << vertices.size() << "    " << links.size() << endl;
        /// on sélection les bons points parmis vertices, puis on leur applique les transformations décrites dans transforms puis on crée le triangle
        for(int i = 0; i < links.size(); i++) {
            vec3 points[3];
            for(int j = 0; j < 3; j++) {
                points[j] = *(vertices[(*links[i])[j]-1]);
                for(int k = 0; k < transforms.size(); k++) {
                    points[j] = transforms[k]->appliquer(points[j]);
                }
            }
            triangles.push_back(new triangle(points[0], points[1], points[2], m));
        }
    }

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;

    vector<triangle*> triangles;
    material *mat;
};
