#include "fix.h"
#include "tuple3t.h"
#include "planet.h"
#include "transformt.h"

#include <iostream>

void main()
{
    fix f(23), g(92.034), h(0.3f);
	Tuple3T<fix> t(f, g, h),
	             n = t.normal();

    PlaneT<fix> plane(f, g, h, 1);

    std::cout << g * g << std::endl;
    std::cout << (float)g * (float)g << std::endl;
				 
	std::cout << t << " " << t.lengthsq() << " " << t.length() << " " << n << " " << n.length() << std::endl;

	Tuple3T<float> ft(23, 92.034f, 0.3f),
	               fn = ft.normal();

    std::cout << ft << " " << ft.lengthsq() << " " << ft.length() << " " << fn << " " << fn.length() << std::endl;
}