#ifndef MATERIALH
#define MATERIALH

#include "ray.h"
#include "hitable.h"

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

vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;
}

class material {
	public:
		virtual bool scatter(const ray& r_in, const hitRecord& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
	public:
		lambertian(const vec3& a) : albedo(a) {}
		virtual bool scatter(const ray& r_in, const hitRecord& rec, vec3& attenuation, ray& scattered) const {
			vec3 target = rec.p + rec.normal + randomInUnitSphere();
			scattered = ray(rec.p, target - rec.p);
			attenuation = albedo;
			return true;
		}

		vec3 albedo;
};

class metal : public material {
	public:
		metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
		virtual bool scatter(const ray& r_in, const hitRecord& rec, vec3& attenuation, ray& scattered) const {
			vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
			scattered = ray(rec.p, reflected + fuzz*randomInUnitSphere());
			attenuation = albedo;
			return (dot(scattered.direction(), rec.normal) > 0.0);
		}
		vec3 albedo;
		float fuzz;
};

#endif