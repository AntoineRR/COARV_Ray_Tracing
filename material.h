#pragma once

#include "ray.h"
#include "vec3.h"
#include "sphere.h"

vec3 random_in_unit_sphere();

vec3 reflect(const vec3 &v, const vec3 &n);

bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted);

float schilck(float cosine, float ref_idx);

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
    virtual vec3 emitted() const {
        return vec3(0, 0, 0);
    }
};

class light : public material {
public:
    light() {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        return false;
    }
    virtual vec3 emitted() const {
        return 4.0*vec3(255, 0, 0)/255.0;
    }
};

class lambertian : public material {
public:
    vec3 albedo;

    lambertian(const vec3 &a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target-rec.p);
        attenuation = albedo;

        return true;
    }
};

class metal : public material {
public:
    vec3 albedo;
    float fuzz;

    metal(const vec3 &a, float f) : albedo(a) {
        if(f < 1)
            fuzz = f;
        else
            fuzz = 1;
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
        attenuation = albedo;

        return (dot(scattered.direction(), rec.normal) > 0);
    }
};

class dielectric : public material {
public:
    float ref_idx;

    dielectric(float ri) : ref_idx(ri) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;

        float reflect_prob, cosine;

        if(dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().lenght();
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().lenght();
        }
        if(refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
            reflect_prob = schilck(cosine, ref_idx);
        }
        else {
            scattered = ray(rec.p, refracted);
            reflect_prob = 1.0;
        }
        if(drand() < reflect_prob) {
            scattered = ray(rec.p, reflected);
        }
        else {
            scattered = ray(rec.p, refracted);
        }
        return true;
    }
};
