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

	vec3& operator+=(vec3& v) {
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


vec3 operator+(vec3& v1, vec3& v2) {
	return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
vec3 operator-(vec3& v1, vec3& v2) {
	return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
vec3 operator*(vec3& v1, vec3& v2) {
	return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
vec3 operator/(vec3& v1, vec3& v2) {
	return vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}
vec3 operator*(vec3 v1, double k) {
	return vec3(v1.x * k, v1.y * k, v1.z  * k);
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
struct ray_collision {
	vec3 normal_collide;
	vec3 point;
	bool collided = false;
	double t = 1000000000;
};

struct Object {

	ShapeType type = none;

	// Material Details
	vec3 colour = vec3(0, 0, 0);
	double reflective = 0;
	double emission = 0;

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
				
				data.t = t;
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
		scale = worldData.sky_boundary / double(max(x, y));
	}

	ray coord_ray(int x, int y) {
		vec3 point;
		point.x = scale * (double(x) - dx);
		point.y = scale * (double(y) - dy);
		point.z = focal * worldData.sky_boundary;
		rotate_x(point, direction.x);
		rotate_y(point, direction.y);
		rotate_z(point, direction.z);
		return ray(position, point);
	}

};


Camera worldCamera;

double dot_product(vec3& a, vec3& b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}
double magnitude(vec3& v) {
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

double angle_between(vec3& a, vec3& b) {
	return acos(dot_product(a, b) / (magnitude(a) * magnitude(b)));
}

double random() {
	return double(rand() / 65536.0);
}
double random(double a, double b) {
	return (random() * (b - a)) + a;
}

vec3 random_hemi(vec3 normal) {
	vec3 dir = normal;
	double ox = random(-1, 1) * (3.1415 / 2);
	double oy = random(-1, 1) * (3.1415 / 2);

	rotate_x(dir, ox);
	rotate_y(dir, oy);
	return dir;
}

vec3 raycast(ray r) {

	vec3 ray_colour(1, 1, 1);
	ray backup_ray = r;
	backup_ray.direction /= worldData.sky_boundary;

	int bounces = 0;
	bool hit_source = false;

	while (bounces < worldData.bounce_limit) {

		ray_collision c_data;
		Object* object = nullptr;

		for (int i = 0; i < worldObjects.size(); i++) {
			ray_collision data = worldObjects[i].collided(r);
			if (data.collided && c_data.t > data.t) {
				c_data = data; object = &worldObjects[i];
			}
		}

		if (c_data.collided && object != nullptr) {
			
			if (object->emission > 0) {
				ray_colour *= object->colour;
				ray_colour *= object->emission;
				return ray_colour;
			}

			r.position = c_data.point;
			r.direction = c_data.normal_collide;

			vec3 rl = vec3(
				lightSource->sphere_position.x - r.position.x,
				lightSource->sphere_position.y - r.position.y,
				lightSource->sphere_position.z - r.position.z
			);

			ray rr(r.position, rl);

			bool shadow = false;
			for (int i = 0; i < worldObjects.size(); i++) {

				if (&worldObjects[i] == lightSource)
					continue;

				ray_collision rc = worldObjects[i].collided(rr);
				if (rc.collided) {
					shadow = true; break;
				}

			}

			if (shadow) {

				ray_colour *= object->colour;
				ray_colour *= 0.2;

			}
			else 
			{
				double hf = 0;

				
				ray_colour *= object->colour;

				vec3 light(lightSource->sphere_position.x - r.position.x,
					lightSource->sphere_position.y - r.position.y,
					lightSource->sphere_position.z - r.position.z);
				light /= magnitude(light);

				

				double ka = 0.2, kd = 0.8;
				double s = dot_product(light, r.direction);

				if (s < 0)
					s = 0;

				ray_colour = vec3(
					(ka * ray_colour.x) + (kd * s * ray_colour.x) + hf,
					(ka * ray_colour.y) + (kd * s * ray_colour.y) + hf,
					(ka * ray_colour.z) + (kd * s * ray_colour.z) + hf
				);
			}

			bounces += 1;
			return ray_colour;
		}
		else 
		{
			r.position = backup_ray.direction + backup_ray.position;
			r.direction = vec3(
				lightSource->sphere_position.x - r.position.x,
				lightSource->sphere_position.y - r.position.y,
				lightSource->sphere_position.z - r.position.z
			);

			bool shadow = false;
			for (Object& obj : worldObjects) {

				if (&obj == lightSource)
					continue;

				ray_collision rc = obj.collided(r);
				if (rc.collided) {
					shadow = true; break;
				}
			}

			ray_colour *= worldData.scene_colour;

			if (shadow) 
				ray_colour *= 0.5;
			
			
			break;
		}
	}

	
	return ray_colour;
}