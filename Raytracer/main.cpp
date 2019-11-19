using namespace std;
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <chrono>
#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"
#include "material.h"

vec3 color(const ray& r, hitable *world, int depth) {
	hitRecord rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)){
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.matPtr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		} else {
			return vec3(0, 0, 0);
		}
	} else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

hitable *random_scene() {
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = random();
			vec3 center(a + 0.9 * random(), 0.2, b + 0.9 * random());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) { // diffuse
					list[i++] = new sphere(center, 0.2, new lambertian(vec3(random() * random(), random() * random(), random() * random())));
				}
				else if (choose_mat < 0.95) { // metal
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1.0 + random()), 0.5*(1.0 + random()), 0.5*(1.0 + random())), 0.5 * random()));
				}
				else { // glass
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7,0.6,0.5), 0.0));

	return new hitableList(list, i);
}

int main() 
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

	ofstream outputImage;
	outputImage.open("output.ppm");

	int nx = 1920;
	int ny = 1080;
	int ns = 100;

	outputImage << "P3\n" << nx << " " << ny << "\n255\n";

	hitable *world = random_scene();

	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.1;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++) {
				float u = float(i + random()) / float(nx);
				float v = float(j + random()) / float(ny);
				ray r = cam.getRay(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}

			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			outputImage << ir << " " << ig << " " << ib << "\n";
		}
	}

	outputImage.close();
	
	cerr << "TIME TO RENDER: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count() << endl;

	int x = 0;

	cin >> x;

	return 0;
}