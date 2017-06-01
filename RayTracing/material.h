#ifndef MATERIALH
#define MATERIALH 

struct hit_record;

#include "ray.h"
#include "hitable.h"
#include<random>

double drand48()
{
	std::random_device rand;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rand());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_real_distribution<> rand48(0.0, 1.0);    // [1.0, 2.0] 範囲の一様乱数
	return rand48(mt);
}

#pragma region Schlickの近似式

float schlick(float cosine, float ref_idx) {
	//Schlickの近似式2
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;

	//Schlickの近似式1
    return r0 + (1-r0)*pow((1 - cosine),5);
}

#pragma endregion

//屈折させるためのコード
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    else 
        return false;
}

//反射させるためのコード
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
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	vec3 albedo;
	float fuzz;
};

//媒体のコード
class dielectric : public material { 
    public:
        dielectric(float ri) : ref_idx(ri) {}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
             vec3 outward_normal;
             vec3 reflected = reflect(r_in.direction(), rec.normal);
             float ni_over_nt;
             //attenuation = vec3(1.0, 1.0, 0.0);	//青色のみ抜く
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




