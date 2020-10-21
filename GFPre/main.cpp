#include <iostream>
#include <string>
#include <algorithm>
#include "CcGFPre.h"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "input argument error" << std::endl;
		std::cout << "CcGFPre.exe <image path> <geocode out path> [true | false]" << std::endl;
		return -1;
	}

	std::string gfImg = argv[1];
	std::string outpath = argv[2];
	bool bReproj = false;
	if (argc == 4)
	{
		std::string strReproj(argv[3]);
		std::transform(strReproj.begin(), strReproj.end(), strReproj.begin(), ::tolower);
		if (strReproj == std::string("true"))
		{
			bReproj = true;
		 }
	}
	bool isSuccess = false;
	if (bReproj)
	{
		isSuccess = gf_reproject_img(gfImg, outpath);
	}
	else
	{
		isSuccess = gf_geocode_img(gfImg, outpath);
	}
	 
	if (!isSuccess)
	{
		return -1;
	}

	return 0;
}