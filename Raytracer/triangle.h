#ifndef TRIANGLEH
#define TRIANGLEH

#include "hitable.h"

class  triangle : public hitable {
	public :
		triangle() {}
		triangle(vec3 v0, vec3 v1, vec3 v2, /*vec3 n, */ material * m) :
				vertex0(v0), vertex1(v1), vertex2(v2), /*normal(n),*/ mat_ptr(m) {};
		virtual bool hit(const ray& r, float tMin, float tMax, hitRecord& rec) const;
		vec3 vertex0;
		vec3 vertex1;
		vec3 vertex2;
		//vec3 normal;
		material* mat_ptr;
};

// Equation from https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
bool triangle::hit(const ray& r, float tMin, float tMax, hitRecord& rec) const {
	// compute plane's normal
	vec3 v0v1 = vertex1 - vertex0;
	vec3 v0v2 = vertex2 - vertex0;
	vec3 N = cross(v0v1, v0v2);

	if (dot(N, r.direction()) < 0)
		return false;

	// Step 1 : Finding P

	// check if ray and plane are parallel
	float nDotr = dot(N, r.direction());
	if (fabs(nDotr) < FLT_EPSILON) // almost 0
		return false;

	// compute d parameter for equation 2
	float d = dot(-N, vertex0);

	// compute t equation 3
	float t = -(dot(N, r.origin()) + d) / nDotr;

	if (t > tMax || t < tMin)
		return false;

	// compute the intersection point from equation 1
	vec3 P = r.point_at_parameter(t);

	// Step 2 : Inside out test
	vec3 C;

	// edge0
	vec3 edge0 = vertex1 - vertex0;
	vec3 vp0 = P - vertex0;
	C = cross(edge0, vp0);
	if (dot(N, C) < 0)
		return false;

	vec3 edge1 = vertex2 - vertex1;
	vec3 vp1 = P - vertex1;
	C = cross(edge1, vp1);
	if (dot(N, C) < 0)
		return false;

	vec3 edge2 = vertex0 - vertex2;
	vec3 vp2 = P - vertex2;
	C = cross(edge2, vp2);
	if (dot(N, C) < 0)
		return false;

	rec.t = t;
	rec.p = P;
	rec.normal = N;
	rec.matPtr = mat_ptr;
	return true;
}

#endif
