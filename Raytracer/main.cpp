using namespace std;
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"

double random() {
	return (rand() / (RAND_MAX + 1.0));
}

vec3 randomInUnitSphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(random(), random(), random()) - vec3(1.0, 1.0, 1.0);
	} while (p.squared_length() >= 1.0);
	return p;
}

vec3 color(const ray& r, hitable *world) {
	hitRecord rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)){
		vec3 target = rec.p + rec.normal + randomInUnitSphere();
		return 0.5 * color(ray(rec.p, target - rec.p), world);
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main() 
{
	ofstream outputImage;
	outputImage.open("output.ppm");

	int nx = 800;
	int ny = 400;
	int ns = 100;

	outputImage << "P3\n" << nx << " " << ny << "\n255\n";

	vec3 lowerLeftCorner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0.0, 0.0);

	hitable *list[2];
	list[0] = new sphere(vec3(0.0, 0.0, -1.0), 0.5);
	list[1] = new sphere(vec3(0.0, -100.5, -1.0), 100);
	hitableList *world = new hitableList(list, 2);

	camera cam;

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
				col += color(r, world);
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

	return 0;
}