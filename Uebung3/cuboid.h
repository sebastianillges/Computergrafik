#include "vec.h"
#include "mat.h"

class Cuboid 
{
public:
    Cuboid() {
    }
	Cuboid(CVec3f bottom_left_corner, float edge_length) {
		float x = bottom_left_corner.get(0);
		float y = bottom_left_corner.get(1);
		float z = bottom_left_corner.get(2);
		this->points[0] = bottom_left_corner;
		float atData[3] = {x, y+edge_length, z};
		this->points[1] = CVec3f(atData);
		float atData2[3] = {x+edge_length, y+edge_length, z};
		this->points[2] = CVec3f(atData2);
		float atData3[3] = {x+edge_length, y, z};
		this->points[3] = CVec3f(atData3);
		float atData4[3] = {x, y, z+edge_length};
		this->points[4] = CVec3f(atData4);
		float atData5[3] = {x, y+edge_length, z+edge_length};
		this->points[5] = CVec3f(atData5);
		float atData6[3] = {x+edge_length, y+edge_length, z+edge_length};
		this->points[6] = CVec3f(atData6);
		float atData7[3] = {x+edge_length, y, z+edge_length};
		this->points[7] = CVec3f(atData7);
	}
	CVec4f get_homogeneous(int i) {
		float oldData[3];
		oldData[0] = this->points[i].get(0);
        oldData[1] = this->points[i].get(1);
        oldData[2] = this->points[i].get(2);
		float newData[4] = {oldData[0], oldData[1], oldData[2], 1};
		return CVec4f(newData);
	}
	CVec3f points[8];
};