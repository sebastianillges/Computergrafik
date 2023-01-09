#include <math.h>
#include "vec.h"
#include "mat.h"

class Point
{
public:
	Point(int x = 0, int y = 0)
	{
		this->x = x;
		this->y = y;
	}
	Point(CVec3f cvec) {
		this->x = (int) cvec.get(0);
		this->y = (int) cvec.get(1);
	}
	Point(CVec4f cvec) {
		this->x = (int) cvec.get(0);
		this->y = (int) cvec.get(1);
	}
	bool operator() (const Point& lhs, const Point& rhs) const
   	{
       return lhs.x < rhs.x;
   	}
	int x, y;
};

class Color
{
public:
	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
	bool equals(Color c) {
		return (this->r == c.r && this->g == c.g && this->b == c.b);
	}
    void print() {
        printf("(%f, %f, %f)\n", this->r, this->g, this->b);
    }
	float r, g, b;
};

CVec4f cross(CVec4f v1, CVec4f v2) {
	float atData[4];
	atData[3] = 1;
	atData[0] = v1.get(1)*v2.get(2) - v1.get(2)*v2.get(1);
	atData[1] = v1.get(2)*v2.get(0) - v1.get(0)*v2.get(2);
	atData[2] = v1.get(0)*v2.get(1) - v1.get(1)*v2.get(0);
	return CVec4f(atData);
}

CVec4f homogenize(CVec3f cvec)
{
	float atData[4] = {cvec.get(0), cvec.get(1), cvec.get(2), 1};
	return CVec4f(atData);
}

CMat4f trans(CVec4f t) {
	float arr[4][4] = {{1,0,0,t.get(0)},
					   {0,1,0,t.get(1)},
					   {0,0,1,t.get(2)},
					   {0,0,0,1}};
	return CMat4f(arr);
}

CMat4f transpose(CMat4f mat) {
	float r_transp[3][3];
	float r_transp_negated[3][3];
	float t[3];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			r_transp[i][j] = mat.get(j, i);
			r_transp_negated[i][j] = -mat.get(j, i);
		}
		t[i] = mat.get(i, 3);
	}
	CVec3f t_vec = CVec3f(t);
	CMat3f r = CMat3f(r_transp);
	CVec3f t_new_vec = -r * t_vec;

	float rt_arr[4][4];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			rt_arr[i][j] = r_transp[i][j];
		}
		rt_arr[i][3] = t_new_vec.get(i);
	}
	rt_arr[3][0] = 0;
	rt_arr[3][1] = 0;
	rt_arr[3][2] = 0;
	rt_arr[3][3] = 1;
	return CMat4f(rt_arr);
}

CMat3f rotx(float angle) {
	float r[3][3] = {{1,0,0},
					{0,cos(angle),-sin(angle)},
					{0,sin(angle), cos(angle)}};
	return CMat3f(r);
}

CMat3f roty(float angle) {
	float r[3][3] = {{cos(angle),0,sin(angle)},
					 {0,1,0},
					 {-sin(angle),0,cos(angle)}};
	return CMat3f(r);
}

CMat3f rotz(float angle) {
	float r[3][3] = {{cos(angle), -sin(angle),0},
					{sin(angle), cos(angle),0},
					{0,0,1}};
	return CMat3f(r);
}

float skalarProd(CVec3f v1, CVec3f v2)
{
    float res = 0;
    for (int i = 0; i < v1.getDimension(); i++) res += v1.get(i) * v2.get(i);
    return res;
}