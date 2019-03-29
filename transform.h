#pragma once

#include "vec3.h"
#include <cmath>

/// classe pour appliquer une trnasformation géométrique à un modele
class transform {
public:
    /// fonction qui renvoie l'image d'un point par cette transformation
    virtual vec3 appliquer(const vec3 &point) const =0;
};

class translation : public transform {
public:
    vec3 t;

    translation() {}
    translation(vec3 _t) : t(_t) {}

    virtual vec3 appliquer(const vec3 &point) const {
        return point+t;
    }
};

class rotation : public transform {
public:
    vec3 origine_axe;
    vec3 axe;
    float angle;

    rotation() {}
    rotation(vec3 _o, vec3 _axe, float _angle) : origine_axe(_o), axe(unit_vector(_axe)), angle(_angle) {}

    virtual vec3 appliquer(const vec3 &point) const {
        vec3 centre = origine_axe + dot(point - origine_axe, axe)*axe;
        return centre + cross(axe, point-centre)/sin(angle*PI/180.0);
    }
};

class scale : public transform {
public:
    vec3 centre;
    vec3 coeffs;

    scale() {}
    scale(vec3 _centre, vec3 _coeffs) : centre(_centre), coeffs(_coeffs) {}
    scale(vec3 _centre, float s) : centre(_centre), coeffs(vec3(s, s, s)) {}

    virtual vec3 appliquer(const vec3 &point) const {
        return centre + coeffs*(point-centre);
    }
};
