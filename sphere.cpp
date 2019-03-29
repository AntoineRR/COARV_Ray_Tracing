#include "sphere.h"

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0* dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4.0*a*c;

    if(discriminant > 0) {
        float temp = (-b-sqrt(discriminant))/(2.0*a);
        if(temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat;
            return true;
        }
        temp = (-b+sqrt(discriminant))/(2.0*a);
        if(temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat;
            return true;
        }
    }
    return false;
}

/**
    pour la détection de collision d'un rayon avec un triangle, on utilise la méthode vu en cours de colli.
*/
bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    /// on vérifie l'intersection avec le plan contenant le triangle
    if(dot(r.point_at_parameter(t_min)-origine, normale)*dot(r.point_at_parameter(t_max)-origine, normale)<0) {
        float a = normale.x(); float b = normale.y(); float c = normale.z();
        float d = -(a*origine.x()+b*origine.y()+c*origine.z());
        /// on calcule le point d'intersection avec le plan
        float temp = -(a*r.origin().x()+b*r.origin().y()+c*r.origin().z()+d)/(a*r.direction().x()+b*r.direction().y()+c*r.direction().z());
        vec3 intersection = r.point_at_parameter(temp);

        /// on vérifie que ce point soit bien dans le triangle
        float _angle = angle(points[0]-intersection, points[1]-intersection)+
                angle(points[1]-intersection, points[2]-intersection)+
                angle(points[2]-intersection, points[0]-intersection);
        //cout << _angle << endl;
        if(_angle > 2*PI-0.1) {
            //cout << _angle << endl;
            rec.t = temp;
            rec.p = intersection;
            rec.normal = normale;
            rec.mat_ptr = mat;
            return true;
        }
    }
    return false;
}

/**
    On vérifie pour le modele la collision avec tous les triangles le composant.
*/
bool modele::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;

    for(int i = 0; i < triangles.size(); i++) {
          //  cout << triangles.size() << " -- " << i << endl;
        if((*triangles[i]).hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}



