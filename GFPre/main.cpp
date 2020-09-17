#include <iostream>
#include "CcGFPre.h"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "input argument error" << std::endl;
		std::cout << "CcGFPre.exe <image path> <geocode out path>" << std::endl;
		return -1;
	}

	std::string gfImg = argv[1];
	std::string outpath = argv[2];
	bool isSuccess = gf_geocode_img(gfImg, outpath);
	if (!isSuccess)
	{
		return -1;
	}

	return 0;
}