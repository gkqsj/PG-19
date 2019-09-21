#ifndef CAMERAH
#define CAMERAH

#include "vec3.h"
#include "vec2.h"
#include "matrix44.h"
#include "object.h"

#ifdef _WIN32 || WIN32
	#include <SDL.h>
#elif defined(__unix__)
	#include <SDL2/SDL.h>
#endif

const int WIDTH = 800;
const int HEIGHT = 600;

class camera
{
public:
	int imgWidth, imgHeight;
	float fov, _near, _far;
	float bottom, left, top, right, aspectRatio;
	matrix44 camToWorld;
	matrix44 worldToCamera;

	vec3 _from, _at, _up;
	vec3 axisX, axisY, axisZ;

public:
	camera();
	camera(const vec3& from, const vec3& at, const vec3& up,
		const float& f, const float& n,
		const int& windowWidth, const int& windowHeight, const float& far) :
		fov(f), _near(n), imgWidth(windowWidth), imgHeight(windowHeight),
		_from(from), _at(at), _up(up), _far(far)
	{
		float radfov = fov * (3.141592f / 180.f);
		top = tan(radfov / 2.f);
		bottom = -top;
		aspectRatio = windowWidth / windowHeight;
		right = tan(radfov / 2.f) * aspectRatio;
		left = -right;

		look_at(from, at, up);
	}


	void look_at(const vec3& from, const vec3& at, const vec3& up)
	{
		axisZ = from - at;
		axisZ.make_unit_vector();
		axisY = up - (dot(up, axisZ) / dot(axisZ, axisZ)) * axisZ;
		axisY.make_unit_vector();
		axisX = cross(axisY, axisZ);
		axisX.make_unit_vector();

		camToWorld = matrix44(
			axisX.x(), axisX.y(), axisX.z(), 0.0,
			axisY.x(), axisY.y(), axisY.z(), 0.0,
			axisZ.x(), axisZ.y(), axisZ.z(), 0.0,
			from.x(), from.y(), from.z(), 1.0
		);
		worldToCamera = camToWorld.inverse();
	}

	bool compute_pixel_coordinates(const vec3& pWorld, vec2& praster)
	{
		vec3 algo, algo2;

		matrix44 multi = matrix44(
			2 * _near / (right - left), 0.0, 0.0, 0.0,
			0.0, 2 * _near / (bottom - top), 0.0, 0.0,
			-(right + left) / (right - left), -(bottom + top) / (bottom - top), (_far + _near) / (_far - _near), 1.0,
			0.0, 0.0, -(2 * _near) / (_far - _near), 0.0
		);

		worldToCamera.mult_point_matrix(pWorld, algo);

		vec3 mProjecao = vec3(
			algo.x() * (_near / algo.z()),
			algo.y() * (_near / algo.z()),
			_near
		);


		multi.mult_point_matrix(mProjecao, algo2);

		praster = vec2((1 + algo2.x()) / 2 * imgWidth, (1 - algo2.y()) / 2 * imgHeight);

		if ((bottom <= algo.y() && algo.y() <= top) && (left <= algo.x() && algo.x() <= right)) 
		{
			return true;
		}
		else {
			return false;
			//calcula a cos(de algo), se der positivo, da false
		}
	}

	void desenharLinha(SDL_Renderer *renderer, vec2 &p0, vec2 &p1)
	{
		vec2 start = p1;
        vec2 diretor = p0-p1;
		int fInt = (int) diretor.length();
		diretor.make_unit_vector();

		for(int i = 0; i <= fInt; i++)
		{
			SDL_RenderDrawPoint(renderer, (int) start.x(), (int) start.y());
			start += diretor;
		}
	}

	int getOutcode(vec2 p, int xMin, int xMax, int yMin, int yMax){
		int bits   = 0;
		int left   = 1;
		int right  = 2;
		int bottom = 4;
		int top    = 8;
		
		if(p.y() > yMax)
		{
			bits &= top;
		}

		if(p.y() < yMin)
		{
			bits &= bottom;
		}

		if(p.x() > xMax)
		{
			bits &= right;
		}

		if(p.x() < xMin)
		{
			bits &= left;
		}

		return bits;
	}

	bool ClipLine(vec2 &p0, vec2 &p1, int xMin, int xMax, int yMin, int yMax)
	{
		int outcode0 = getOutcode(p0, xMin, xMax, yMin, yMax);
		int outcode1 = getOutcode(p1, xMin, xMax, yMin, yMax);
		
		float slope, novoX, novoY = 0;

		bool accept = false;

		while(true){
			if(outcode0 == 0 & outcode1 == 0)
			{
				accept = true;
				break;
			} else if (outcode0 & outcode1)
			{
				break;
			} else {
				// Pelo menos um ponto está fora da janela
				int outcodeOutside = outcode1 != 0? outcode1 : outcode0;
				// Calcula x e y para interseção com top, bottom, right e left
				if (outcodeOutside & 8)
				{
                    //slope = (p1.y() - p0.y())/(p1.x() - p0.x());
					//novoX = p0.x() + (1.0f/slope)*(yMax - p0.y());
					novoY = yMax;
				} else if (outcodeOutside & 4)
				{
                    //slope = (p1.y() - p0.y())/(p1.x() - p0.x());
					//novoX = p0.x() + (1.0f/slope)*(yMin - p0.y());
					novoY = yMin;
				} else if (outcodeOutside & 2)
				{
                    //slope = (p1.y() - p0.y())/(p1.x() - p0.x());
					novoX = xMax;
					//novoY = p0.y() + slope*(xMax - p0.x());
				} else if (outcodeOutside & 1)
				{
                    //slope = (p1.y() - p0.y())/(p1.x() - p0.x());
					novoX = xMin;
					//novoY = p0.y() + slope*(xMin - p0.x());
				}

				if (outcodeOutside == outcode0)
				{
                    p0[0] = novoX;
                    p0[1] = novoY;
                    printf("Slope: %f, novoX: %f, novoY: %f",slope ,novoX, novoY);
					outcode0 = getOutcode(p0, xMin, xMax, yMin, yMax);
				} else {
                    p1[0] = novoX;
                    p1[1] = novoY;
                    printf("Slope: %f, novoX: %f, novoY: %f",slope ,novoX, novoY);
					outcode1 = getOutcode(p1, xMin, xMax, yMin, yMax);
				}
			}
		}
		return accept;
	}


	void render_scene(std::vector<Obj> objs, SDL_Renderer* renderer){
		int PosX = 0;
		int PosY = 0;
		int PosZ = 4;
		vec3 light(0.0f, 0.0f, -1.0f);
		light.make_unit_vector();
		int aa = 0;

		for (auto obj : objs) 
		{
			for (int i = 0; i < obj.mesh.tris.size(); i++)
			{
				vec2 praster1, praster2, praster3;
				vec2 bkpraster1, bkpraster2, bkpraster3;

				vec3 col(255, 255, 255);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

				bool v1, v2, v3;
				v1 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[0].pos, praster1);
				v2 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[1].pos, praster2);
				v3 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[2].pos, praster3);

				bkpraster1 = praster1;
				bkpraster2 = praster2;
				bkpraster3 = praster3;

				if (v1 && v2)
                {
                    if(ClipLine(bkpraster1, bkpraster2, 0, WIDTH, 0, HEIGHT))
                    {
                    	desenharLinha(renderer, bkpraster1, bkpraster2);
                    }
                }

				if (v1 && v3)
                {
                    if(ClipLine(bkpraster1, bkpraster3, 0, WIDTH, 0, HEIGHT))
                    {
                    	desenharLinha(renderer, bkpraster1, bkpraster3);
                    }
                }

				if (v2 && v3)
                {
                    if(ClipLine(bkpraster2, bkpraster3, 0, WIDTH, 0, HEIGHT))
                    {
                    	desenharLinha(renderer, bkpraster2, bkpraster3);
                    }                    
                }
			}
		}
	}
};


#endif
