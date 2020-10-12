#pragma once
#include <math.h>
#include <corecrt_math_defines.h>
#include <iostream>
class PVector {
private:
	double mag;
	int angle;

public:
	PVector(double m, int a) : mag(m), angle(a) {}
	PVector() : mag(0), angle(0) {}
	PVector(int x, int y) {
		mag = sqrt(pow(x, 2) + pow(y, 2));
		//std::cout << "mag = " << mag << std::endl;
		angle = (int)(atan2(y, x) * 180.0 / M_PI) % 360;
	}

	int getDir() {
		//std::cout << round(double(angle) / 45.0) << std::endl;
		return round(double(angle) / 45.0);
	}

	int getAngle() {
		return this->angle;
	}

	double getMag() {
		return this->mag;
	}
};
