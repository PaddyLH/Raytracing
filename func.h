#pragma once

#include <windows.h>
#include <stdint.h>

#include <thread>
#include <iostream>
#include <vector>
#include <string>

typedef uint32_t u32;

using namespace std;

struct vec3 {
	double x = 0, y = 0, z = 0;
	vec3(double ix = 0, double iy = 0, double iz = 0) {
		x = ix; y = iy; z = iz;
	}
	operator u32() {
		return u32(255.0 * z) + (u32(255.0 * y) << 8) + (u32(255.0 * x) << 16);
	}

	vec3 operator+=(vec3 v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	vec3& operator-=(vec3& v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	vec3& operator/=(vec3& v) {
		x /= v.x; y /= v.y; z /= v.z;
		return *this;
	}
	vec3& operator*=(vec3& v) {
		x *= v.x; y *= v.y; z *= v.z;
		return *this;
	}
	vec3& operator*=(double v) {
		x *= v; y *= v; z *= v;
		return *this;
	}
	vec3& operator/=(double v) {
		x /= v; y /= v; z /= v;
		return *this;
	}
};

vec3 u32_colour(u32 x) {
	return vec3(
		double((x >> 16) & 0xff) / double(0xff),
		double((x >> 8) & 0xff) / double(0xff),
		double(x & 0xff) / double(0xff)
	);
}

vec3 operator+(vec3& v1, vec3& v2) {
	return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
vec3 operator-(vec3& v1, vec3& v2) {
	return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
vec3 operator*(vec3 v1, vec3 v2) {
	return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
vec3 operator/(vec3& v1, vec3& v2) {
	return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}
vec3 operator*(vec3 v1, double k) {
	return vec3(v1.x * k, v1.y * k, v1.z  * k);
}
vec3 operator*(double k, vec3 v1) {
	return vec3(v1.x * k, v1.y * k, v1.z * k);
}
double distance(vec3 v1, vec3 v2) {
	return sqrt(pow(v1.x - v2.z, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2) );
}
void rotate_x(vec3& v1, double o) {
	double z = v1.z, y = v1.y;
	v1.y = (y * cos(o)) - (z * sin(o));
	v1.z = (y * sin(o)) + (z * cos(o));
}
void rotate_y(vec3& v1, double o) {
	double x = v1.x, z = v1.z;
	v1.x = (x * cos(o)) + (z * sin(o));
	v1.z = (z * cos(o)) - (x * sin(o));
}
void rotate_z(vec3& v1, double o) {
	double x = v1.x, y = v1.y;
	v1.x = (x * cos(o)) - (y * sin(o));
	v1.y = (x * sin(o)) + (y * cos(o));
}
double dot_product(vec3& a, vec3& b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
double magnitude(vec3& v) {
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

double angle_between(vec3 a, vec3 b) {
	return acos(dot_product(a, b) / (magnitude(a) * magnitude(b)));
}
vec3 lerp(vec3 a, vec3 b, double k) {
	return vec3(
		(a.x * (1.0 - k)) + (b.x * k),
		(a.y * (1.0 - k)) + (b.y * k),
		(a.z * (1.0 - k)) + (b.z * k)
	);
}

struct ray {
	vec3 position, direction;

	ray(vec3 pos, vec3 dir) {
		position = pos; direction = dir;
	}
};



enum ShapeType : int {
	none = 0,
	tri = 1,
	sphere = 2,
	cuboid = 3
};

struct material {
	vec3 colour;

	vec3 emissionColour;
	double emissionStrength;

	double specularStrength = 0;

};

struct ray_collision {
	vec3 normal_collide;
	vec3 point;
	bool collided = false;
	double dist = 1000000;
	material mat;
};

struct Object {

	ShapeType type = none;

	// Material Details
	material mat;

	// Circle
	double sphere_radius = 0;
	vec3 sphere_position;
	
	// Triangle
	vec3 t1, t2, t3;

	// Cuboid
	vec3 c1, c2;

	Object() {
		return;
	}

	Object(vec3 p, double r = 1) {
		sphere_radius = r;
		sphere_position = p;
		type = sphere;
	}
	Object(vec3 a1, vec3 a2, vec3 a3) {
		t1 = a1; t2 = a2; t3 = a3;
		type = tri;
	}

	ray_collision collided(ray r) {

		ray_collision data;

		if (type == sphere) 
		{
			double a = (r.direction.x * r.direction.x)
				+ (r.direction.y * r.direction.y)
				+ (r.direction.z * r.direction.z);
			double b = (2 * r.direction.x * (r.position.x - sphere_position.x))
				+ (2 * r.direction.y * (r.position.y - sphere_position.y))
				+ (2 * r.direction.z * (r.position.z - sphere_position.z));
			double c = (sphere_position.x * sphere_position.x)
				+ (sphere_position.y * sphere_position.y)
				+ (sphere_position.z * sphere_position.z)
				+ (r.position.x * r.position.x)
				+ (r.position.y * r.position.y)
				+ (r.position.z * r.position.z)
				- (2 * ((sphere_position.x * r.position.x)
					+ (sphere_position.y * r.position.y)
					+ (sphere_position.z * r.position.z)))
				- (sphere_radius * sphere_radius);
			double d = (b * b) - (4 * a * c);

			data.collided = d > 0;

			if (data.collided) {
				double t = -(b + sqrt(d)) / (2.0 * a);

				if (t < 0) {
					data.collided = false;
					return data;
				}

				data.point.x = r.position.x + (t * r.direction.x);
				data.point.y = r.position.y + (t * r.direction.y);
				data.point.z = r.position.z + (t * r.direction.z);

				data.normal_collide.x = (data.point.x - sphere_position.x) / sphere_radius;
				data.normal_collide.y = (data.point.y - sphere_position.y) / sphere_radius;
				data.normal_collide.z = (data.point.z - sphere_position.z) / sphere_radius;
				
				data.dist = distance(data.point, r.position);
				data.mat = mat;
			}

			return data;
		}
		else if (type == tri) 
		{



		}
		else if (type == cuboid) 
		{



		}

		return data;
	}

};



struct SceneData {

	int bounce_limit = 1;
	int rays_per_pixel = 1;
	double sky_boundary = 100;

	vec3 scene_colour;
	vec3 sun_direction;

};


SceneData worldData;
vector<Object> worldObjects;
Object* lightSource;


struct Camera {

	vec3 position, direction;
	double fov = 90;
	double focal = 1;

	int screen_x = 0, screen_y = 9;

	double scale = 1;
	double dx = 0, dy = 0;

	void set_size(int x, int y) {
		dx = double(x - 1) / 2.0;
		dy = double(y - 1) / 2.0;
		scale = 1.0 / double(max(x, y));
	}

	ray coord_ray(int x, int y) {
		vec3 point;
		point.x = scale * (double(x) - dx);
		point.y = scale * (double(y) - dy);
		point.z = focal;
		rotate_x(point, direction.x);
		rotate_y(point, direction.y);
		rotate_z(point, direction.z);
		point /= magnitude(point);
		return ray(position, point);
	}

};


Camera worldCamera;

double random() {
	return double(rand()) / 65536.0;
}
double random(double a, double b) {
	return (random() * (b - a)) + a;
}

vec3 random_direction() {
	vec3 p;
	while (true) {
		p.x = random(-1, 1);
		p.y = random(-1, 1);
		p.z = random(-1, 1);
		double dist = magnitude(p);

		if (dist <= 1) {
			p /= sqrt(dist);
			return p;
		}
	}
}

vec3 random_hemi(vec3 normal) {
	vec3 dir = normal;

	double r1,  r2;
	r1 = random(0, 1); r2 = random(0, 1);
	double r = sqrt(-2 * log(r1));
	double o = 2 * 3.1415 * r2;
	r1 = r * cos(o);
	r2 = r * sin(o);

	rotate_x(dir, r1);
	rotate_y(dir, r2);
	return dir;
}



ray_collision CalculateCollision(ray r) {
	ray_collision data;
	double closest_dist = 10000000;

	for (Object& obj : worldObjects) {
		ray_collision rc = obj.collided(r);
		if (rc.dist < closest_dist) {
			data = rc; closest_dist = rc.dist;
		}
	}

	return data;
}

vec3 ground_colour = vec3(0.1, 0.1, 0.1);
vec3 sky_colour_1 = vec3(6.0 / 255.0, 70.0 / 255.0, 131.0 / 255.0);
vec3 sky_colour_2 = vec3(194.0 / 255.0, 234.0 / 255.0, 236.0 / 255.0);

vec3 scene_colour(vec3 dir) {
	return vec3(0, 0, 0);
	double s = angle_between(dir, vec3(0, -1, 0)) / (3.14159265 / 2);
	if (s > 1.3)
		return ground_colour;
	s /= 1.3;

	return vec3(
		sky_colour_1.x * s + (sky_colour_2.x * (1 - s)),
		sky_colour_1.y * s + (sky_colour_2.y * (1 - s)),
		sky_colour_1.z * s + (sky_colour_2.z * (1 - s))
	);
}

vec3 raytrace(ray r) {

	vec3 incomingLight = vec3(0, 0, 0);
	vec3 raycolour = vec3(1, 1, 1);

	for (int bounce = 0; bounce <= worldData.bounce_limit; bounce++) {

		ray_collision rc = CalculateCollision(r);


		if (rc.collided) 
		{
			r.position = rc.point;

			vec3 scaled = rc.normal_collide * (2 * dot_product(r.direction, rc.normal_collide));
			vec3 specularDir = r.direction - scaled;
			
			vec3 diffuseDir = random_hemi(rc.normal_collide);

			r.direction = lerp(diffuseDir, specularDir, rc.mat.specularStrength);

			vec3 emittedLight = rc.mat.emissionColour;
			emittedLight *= rc.mat.emissionStrength;
			incomingLight += emittedLight * raycolour;
			raycolour *= rc.mat.colour;
			//raycolour *= dot_product(r.direction, rc.normal_collide);
		}
		else 
		{
			incomingLight += raycolour * scene_colour(r.direction);
			break;
		}
	}

	return incomingLight;
}

vec3 ray_func(ray r) {
	vec3 ray_colour;
	for (int i = 0; i < worldData.rays_per_pixel; i++) {
		ray_colour += raytrace(r);
	} ray_colour /= worldData.rays_per_pixel;
	return ray_colour;
}

