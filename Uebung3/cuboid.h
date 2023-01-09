#include "vec.h"
#include "mat.h"

class Cuboid 
{
public:
    Cuboid() {
    }
	Cuboid(CVec4f bottom_left_corner, float edge_length) {
		float x = bottom_left_corner.get(0);
		float y = bottom_left_corner.get(1);
		float z = bottom_left_corner.get(2);
		this->corners[0] = bottom_left_corner;
		float atData[4] = {x, y+edge_length, z, 1};
		this->corners[1] = CVec4f(atData);
		float atData2[4] = {x+edge_length, y+edge_length, z, 1};
		this->corners[2] = CVec4f(atData2);
		float atData3[4] = {x+edge_length, y, z, 1};
		this->corners[3] = CVec4f(atData3);
		float atData4[4] = {x, y, z+edge_length, 1};
		this->corners[4] = CVec4f(atData4);
		float atData5[4] = {x, y+edge_length, z+edge_length, 1};
		this->corners[5] = CVec4f(atData5);
		float atData6[4] = {x+edge_length, y+edge_length, z+edge_length, 1};
		this->corners[6] = CVec4f(atData6);
		float atData7[4] = {x+edge_length, y, z+edge_length, 1};
		this->corners[7] = CVec4f(atData7);
	}
	CVec4f corners[8];
};