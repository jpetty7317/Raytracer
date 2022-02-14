using namespace std;
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <chrono>
#include <regex>
#include <thread>
#include "sphere.h"
#include "triangle.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"
#include "material.h"

struct face {
	int index[3];
};

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

hitable* random_scene(vector<vec3> vertices, vector<face> faces) {
	hitable **list = new hitable*[faces.size() + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));

	lambertian* l = new lambertian(vec3(0.1, 0.5, 0.1));

	int i = 1;
	for (const face& face: faces){
		vec3 v1 = vertices[face.index[0]];
		vec3 v2 = vertices[face.index[1]];
		vec3 v3 = vertices[face.index[2]];
		list[i] = new triangle(v1, v2, v3, l);
		i++;
	}

	return new hitableList(list, i);
}

vec3 extractVector(string s)
{
	std::regex regex("\\s+");

	std::vector<std::string> out(
		std::sregex_token_iterator(s.begin(), s.end(), regex, -1),
		std::sregex_token_iterator()
	);

	char symbol = out[0][0];
	float v1 = std::stof(out[1]);
	float v2 = std::stof(out[2]);
	float v3 = std::stof(out[3]);

	return vec3(v1, v2, v3);
}

face extractFace(string s)
{
	std::regex regex("\\s+");

	std::vector<std::string> out(
		std::sregex_token_iterator(s.begin(), s.end(), regex, -1),
		std::sregex_token_iterator()
	);

	face ret;
	ret.index[0] = std::stoi(out[1]) - 1;
	ret.index[1] = std::stoi(out[2]) - 1;
	ret.index[2] = std::stoi(out[3]) - 1;

	return ret;
}

void renderRow(int j, int range, int nx, int ny, int ns, camera cam, hitable* world, vector<face> *output)
{
	for (int y = j; y < range; y++)
	{
		for (int i = 0; i < nx; i++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++) {
				float u = float(i) / float(nx);
				float v = float(y) / float(ny);
				ray r = cam.getRay(u, v);
				col += color(r, world, 0);
			}

			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			face irgb;
			irgb.index[0] = int(255.99 * col[0]);
			irgb.index[1] = int(255.99 * col[1]);
			irgb.index[2] = int(255.99 * col[2]);

			(*output)[y * nx + i] = irgb;
		}
	}
}

int main()
{
	ifstream obj("teapot.obj");
	string s;
	vector<vec3> vertices;
	vector<face> faces;

	while (!obj.eof()){
		getline(obj, s);
		
		if (s[0] == 'v'){
			vertices.push_back(extractVector(s));
		}
		else if (s[0] == 'f'){
			faces.push_back(extractFace(s));
		}
	}

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

	constexpr int nx = 1920;
	constexpr int ny = 1080;
	int ns = 100;

	hitable *world = random_scene(vertices, faces);

	vec3 lookfrom(0, 2, 20);
	vec3 lookat(0, 2, 0);
	float dist_to_focus = 20.0;
	float aperture = 0.1;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

	vector<thread> thread_pool;
	vector<face> output(nx * ny);

	for (int j = 0; j < ny; j += 5)
	{
		thread_pool.emplace_back(renderRow, j, j + 5, nx, ny, ns, cam, world, &output); 
	}

	for (auto& thread : thread_pool) {
		thread.join();
	}

	ofstream outputImage;
	outputImage.open("output.ppm");
	outputImage << "P3\n" << nx << " " << ny << "\n255\n";

	for (int y = ny - 1; y >= 0; y--) {
		for (int x = 0; x < nx; x++){
			face pixel = output[x + y * nx];
			outputImage << pixel.index[0] << " " << pixel.index[1] << " " << pixel.index[2] << "\n";
		}
	}

	outputImage.close();

	
	cerr << "TIME TO RENDER: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count() << endl;

	int x = 0;

	cin >> x;

	return 0;
}