#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
const double DOUBLE_INFINITY = 1e8;
const double BIAS = 1e-3;
const double EPS = 1e-5;
const int DEPTH_UPPER_BOUND = 2;


struct HitPoint
{
	double t;
	glm::dvec3 pos;
	int sphereIndex;
};

struct Ray
{
	glm::dvec3 dir;
	glm::dvec3 originPos;
};

struct Sphere
{
	glm::dvec3 center;
	double r;
	double r2;
	bool isLight;
	glm::dvec3 emission;
	glm::dvec3 diffuse;
	glm::dvec3 specular;
};


class RayTracker
{
public:
	RayTracker(
		const std::vector<Sphere> &spheres,
		const glm::dvec3 &background,
		const glm::dvec3 &ambient
	) :
		spheres(spheres),
		background(background),
		ambient(ambient)
	{
		// empty
	}
	const std::vector<Sphere> &spheres;
	const glm::dvec3 &background;
	const glm::dvec3 &ambient;

	bool intersectSphere(
		const Ray &ray,
		const Sphere &sphere,
		HitPoint &hitPoint)
	{
		glm::dvec3 l = sphere.center - ray.originPos;
		double tca = glm::dot(l, ray.dir);
		if (tca < 0) return false;
		float d2 = glm::dot(l, l) - tca * tca;
		if (d2 > sphere.r2) return false;
		double thc = glm::sqrt(sphere.r2 - d2);

		hitPoint.t = tca - thc;
		hitPoint.pos = ray.originPos + hitPoint.t*ray.dir;

		return true;
	}

	glm::dvec3 getNormForHitPointOnSphere(
		const HitPoint &hitPoint
	)
	{
		return glm::normalize(spheres[hitPoint.sphereIndex].center - hitPoint.pos);
	}

	HitPoint intersect(const Ray &ray)
	{
		HitPoint hitPoint = { DOUBLE_INFINITY, glm::dvec3(), -1 };
		HitPoint swapHitPoint = { DOUBLE_INFINITY,glm::dvec3(), -1 };
		for (int i = 0; i < spheres.size(); ++i)
		{
			if (intersectSphere(ray, spheres[i], swapHitPoint) &&
				hitPoint.t > swapHitPoint.t)
			{
				hitPoint = swapHitPoint;
				hitPoint.sphereIndex = i;
			}
		}
		return hitPoint;
	}

	glm::dvec3 track(const Ray &ray, int depth)
	{
		HitPoint hitPoint = intersect(ray);
		if (hitPoint.t == DOUBLE_INFINITY)
		{
			return background;
		}
		else if (depth == DEPTH_UPPER_BOUND)
		{
			return spheres[hitPoint.sphereIndex].emission +
				ambient*spheres[hitPoint.sphereIndex].diffuse +
				getDiffuse(hitPoint);
		}
		else
		{
			return spheres[hitPoint.sphereIndex].emission +
				ambient*spheres[hitPoint.sphereIndex].diffuse +
				getDiffuse(hitPoint) +
				getReflect(ray, hitPoint, depth);
		}

	}

	glm::dvec3 getDiffuse(const HitPoint &hitPoint)
	{
		glm::dvec3 color = glm::dvec3();

		for (int i = 0; i < spheres.size(); ++i) {
			if (!spheres[i].isLight)
			{
				continue;
			}
			Ray shadowRay = {
				spheres[i].center - hitPoint.pos,
				hitPoint.pos + BIAS*getNormForHitPointOnSphere(hitPoint) };

			if (glm::dot(shadowRay.dir, getNormForHitPointOnSphere(hitPoint)) < 0)
			{
				continue;
			}

			HitPoint shadowRayHitPoint = intersect(shadowRay);
			if (shadowRayHitPoint.t != DOUBLE_INFINITY &&
				shadowRayHitPoint.sphereIndex != i)
			{
				continue;
			}
			color += spheres[i].emission*
				spheres[hitPoint.sphereIndex].diffuse*
				glm::dot(shadowRay.dir, getNormForHitPointOnSphere(hitPoint));
		}

		return color;
	}

	glm::dvec3 getReflect(const Ray &ray, const HitPoint &hitPoint, int depth)
	{
		glm::dvec3 hitPointNorm = getNormForHitPointOnSphere(hitPoint);
		Ray reflectRay = {
			ray.dir - 2 * glm::dot(ray.dir, hitPointNorm)*hitPointNorm,
			hitPoint.pos + BIAS*getNormForHitPointOnSphere(hitPoint) };

		return track(reflectRay, depth + 1)*spheres[hitPoint.sphereIndex].specular;
	}
};

