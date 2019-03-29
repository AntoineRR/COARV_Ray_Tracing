#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include <ctime>
#include <vector>
#include "camera.h"
#include "sphere.h"
#include "hitable_list.h"
#include "vec3.h"
#include "material.h"
#include "transform.h"

using namespace std;


/**
fonction permettant d'ouvrir un fichier obj et de récupérer les sommets et liens entre sommets
*/
bool loadOBJ(const char * path, vector <vec3*> & vertices, vector <vec3*> &links) {
    ifstream file(path, ios::in);
    if(file) {
        string entete = "";
        // on isole les marqueurs 'v'
        while(entete != "v")
            file >> entete;
        while(entete == "v") {
            vec3 *temp = new vec3();
            file >> (*temp).e[0];
            file >> (*temp).e[1];
            file >> (*temp).e[2];

            vertices.push_back(temp);
            file >> entete;
        }

        file.seekg(ios::beg);
        entete = "";
        // on isole les marqueurs 'f'
        while(entete != "f")
            file >> entete;
        string line;
        while(entete == "f" && getline(file, line, '/')) {
            vec3 *temp = new vec3();
            // on isole uniquement les ids des points et non des coordonnées de texture.
            std::istringstream iss (line);
            iss >> (*temp).e[0];
            getline(file, line, ' ');
            getline(file, line, '/');
            iss = std::istringstream(line);
            iss >> (*temp).e[1];
            getline(file, line, ' ');
            getline(file, line, '/');
            iss = std::istringstream(line);
            iss >> (*temp).e[2];
            getline(file, line);

            links.push_back(temp);
            file >> entete;
        }

        file.close();

        return true;
    }
    return false;
}

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if(world->hit(r, 0.001, 100000, rec)) {
        ray scattered;
        vec3 emitted = rec.mat_ptr->emitted();
        vec3 attenuation;
        if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation*color(scattered, world, depth+1);
        }
        else {
            return emitted;
        }
    }
    else {
        /// sans lumière

        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);

        /// avec lumière

        //return vec3(0, 0, 0);
    }
}

hitable* random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for(int a = -11; a < 11; a++) {
        for(int b = -11; b < 11; b++) {
            float choose_mat = drand();
            vec3 center(a+0.9*drand(), 0.2, b+0.9*drand());
            if((center-vec3(4, 0.2, 0)).lenght() > 0.9) {
                if(choose_mat < 0.8) {
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(drand()*drand(), drand()*drand(), drand()*drand())));
                }
                else if(choose_mat < 0.95) {
                    float flou;
                    if(drand() < 0.5)
                        flou = 0;
                    else
                        flou = drand();
                    list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1+drand()), 0.5*(1+drand()), 0.5*drand()), flou));
                }
                else {
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0));

    return new hitable_list(list, i);
}

int main() {
    srand(time(NULL));

    int nx = 200;
    int ny = 100;
    int ns = 10;

    ofstream img("image.ppm", ios::out | ios::trunc);
    img << "P3\n" << nx << " " << ny << "\n255\n";

    vector<vec3*> vertices;
    vector<vec3*> links;
    loadOBJ("deer.obj", vertices, links);

    hitable *list[2];

    /// listes de transformations pour le modele.
    vector<transform*> transforms;
    transforms.push_back(new scale(vec3(0, 0, 0), 1.0/800.0));
    transforms.push_back(new rotation(vec3(0, 0, 0), vec3(0, 1, 0), 90));
    transforms.push_back(new translation(vec3(0, -1, 0)));

    /// on crée le modele avec les sommets, les liens, et les transformations.
    list[0] = new modele(vertices, links, transforms, new lambertian(vec3(0.5, 1, 0.5)));

    list[1] = new sphere(vec3(0, -100, -1), 100, new metal(vec3(0.8, 0.8, 1), 0.1));
    //list[2] = new sphere(vec3(0, 0, 2), 1, new light());

    //list[2] = new triangle(vec3(0, 20, 3), vec3(20, 0, 3), vec3(-20, 0, 3), new lambertian(vec3(1, 0.5, 1)));
    /*list[2] = new sphere(vec3(1, 0, -1.5), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0));
    list[3] = new sphere(vec3(-1, 0, -1.5), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1, 0, -1.5), -0.45, new dielectric(1.5));
    *///list[2] = new sphere(vec3(-0.7, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0));
    //list[3] = new sphere(vec3(0.7, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0));
    hitable* world = new hitable_list(list, 2);

    //hitable *world = random_scene();

    //vec3 lookfrom(8, 1.5, -3), lookat(4, 1, 0);
    vec3 lookfrom(0, 0, -3), lookat(0, 0, 2);
    float dist_to_focus = (lookfrom-lookat).lenght();
    float aperture = 0;

    camera cam(lookfrom, lookat, vec3(0.0, 1.0, 0.0), 60, float(nx)/float(ny), aperture, dist_to_focus);

    int avancee = 0;
    long _clock = clock();
    for(int j = ny-1; j >= 0; j--) {
        for(int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for(int s = 0; s < ns; s++) {
                float u = float(i + drand())/float(nx);
                float v = float(j + drand())/float(ny);

                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

            int ir = int(255.99*col.r());
            int ig = int(255.99*col.g());
            int ib = int(255.99*col.b());

            img << ir << " " << ig << " " << ib << "\n";

            avancee++;
        }
        float r = 100.0*float(avancee)/float(nx*ny);
        cout << r << "%, temps restant : " << ((100.0-r)*float(clock()-_clock)/r)/(60.0*CLOCKS_PER_SEC) <<" min" << endl;
    }
    return 0;
}
