#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0 * vec3(random(), random(), 0) - vec3(1, 1, 0);
	} while (dot(p,p) >= 1.0);
	return p;
}

class camera {
	public:
		camera(vec3 lookFrom, vec3 lookAt, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
			lens_radius = aperture / 2;
			float theta = vfov * M_PI / 180;
			float halfHeight = tan(theta / 2);
			float halfWidth = aspect * halfHeight;
			origin = lookFrom;
			w = unit_vector(lookFrom - lookAt);
			u = unit_vector(cross(vup, w));
			v = cross(w, u);
			lowerLeftCorner = origin - halfWidth * focus_dist * u - halfHeight * focus_dist * v - focus_dist * w;
			horizontal = 2 * halfWidth * focus_dist * u;
			vertical = 2 * halfHeight * focus_dist * v;
		}

		ray getRay(float s, float t) { 
			vec3 rd = lens_radius * random_in_unit_disk();
			vec3 offset = u * rd.x() + v * rd.y();
			return ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset); 
		}

		vec3 origin;
		vec3 lowerLeftCorner;
		vec3 horizontal;
		vec3 vertical;
		vec3 u, v, w;
		float lens_radius;
};

#endif