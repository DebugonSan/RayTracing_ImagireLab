#ifndef MATERIALH
#define MATERIALH 

struct hit_record;

#include "ray.h"
#include "hitable.h"

double drand48()
{
	return (double(rand()) / RAND_MAX);
}

#pragma region Schlick�̋ߎ���

float schlick(float cosine, float ref_idx) {
	//Schlick�̋ߎ���2
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;

	//Schlick�̋ߎ���1
    return r0 + (1-r0)*pow((1 - cosine),5);
}

#pragma endregion

//���܂����邽�߂̃R�[�h
bool 
refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);	//v�x�N�g���𐳋K��
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);	//= ex * sin' + ey * cos'
        return true;
    }
    else 
        return false;
}

//���˂����邽�߂̃R�[�h
vec3 reflect(const vec3& v, const vec3& n) {
     return v - 2*dot(v,n)*n;
}


vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(drand48(),drand48(),drand48()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}


class material  {
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
    public:
        lambertian(const vec3& a) : albedo(a) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
             vec3 target = rec.p + rec.normal + random_in_unit_sphere();
             scattered = ray(rec.p, target-rec.p);
             attenuation = albedo;
             return true;
        }

        vec3 albedo;
};

class metal : public material {
public:
	metal(const vec3& a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	vec3 albedo;
};

#pragma region Chap.8_ex.cord
/*

class metal : public material {
public:
metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
	vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
	scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
	attenuation = albedo;
	return (dot(scattered.direction(), rec.normal) > 0);
}
vec3 albedo;
float fuzz;
};

*/
#pragma endregion

//�}�̂̃R�[�h
class dielectric : public material { 
    public:
        dielectric(float ri) : ref_idx(ri) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
             vec3 outward_normal;
             vec3 reflected = reflect(r_in.direction(), rec.normal);
             float ni_over_nt;
             //attenuation = vec3(1.0, 1.0, 0.0);	//�F�̗v�f�̂ݓ��߂��Ȃ�
			 attenuation = vec3(1.0, 1.0, 1.0);
             vec3 refracted;
             float reflect_prob;
             float cosine;

             if (dot(r_in.direction(), rec.normal) > 0) {
                  outward_normal = -rec.normal;
                  ni_over_nt = ref_idx;
				  cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
             }
             else {
                  outward_normal = rec.normal;
                  ni_over_nt = 1.0 / ref_idx;
                  cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
             }

			 if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
				 reflect_prob = schlick(cosine, ref_idx);
			 }
			 else {
				 scattered = ray(rec.p, reflected);
				 reflect_prob = 1.0;
			 }

			 //Chap.9 Code5
			 if (drand48() < reflect_prob) {
				 scattered = ray(rec.p, reflected);

			 }
			 else {
				 scattered = ray(rec.p, refracted);
			 }
             return true;
        }

        float ref_idx;
};

#endif




