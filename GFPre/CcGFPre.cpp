#include <sstream>
#include <iostream>
#include "CcGFPre.h"


std::vector<GDAL_GCP> get_gcp_from_rpc(const std::size_t& imgWidth, const std::size_t& imgHeight, GDALRPCInfo& rpcInfo)
{
	int gcpNum = 10000;
	double gcpGridSize = sqrt(floor(imgWidth * imgHeight / double(gcpNum)));
	int gcpGridXNum = floor(imgWidth / gcpGridSize);
	int gcpGridYNum = floor(imgHeight / gcpGridSize);

	std::vector<double> pixelVec;
	std::vector<double> lineVec;
	for (int i = 1; i < gcpGridXNum; i++)
	{
		double tempPixel = gcpGridSize * i;
		for (int j = 1; j < gcpGridYNum; j++)
		{
			double tempLine = gcpGridSize * j;
			pixelVec.push_back(tempPixel);
			lineVec.push_back(tempLine);
		}
	}

	int gcpNumActual = pixelVec.size();
	std::vector<double> zs(gcpNumActual, 0.0);
	std::vector<int> transStatus(gcpNumActual, 0);
	std::vector<double> pixelVecBackup(pixelVec.begin(), pixelVec.end());
	std::vector<double> lineVecBackup(lineVec.begin(), lineVec.end());

	char** rpcTransOption = nullptr;
	rpcTransOption = CSLSetNameValue(rpcTransOption, "RPC_HIGHT", "0.0");
	void* rpcTransform = GDALCreateRPCTransformer(&rpcInfo, false, 0.1, rpcTransOption); //pixel/line --> lat/long
	GDALRPCTransform(rpcTransform, false, gcpNumActual, pixelVec.data(), lineVec.data(), zs.data(), transStatus.data()); //pixel/line --> lat/long
	GDALDestroyRPCTransformer(rpcTransform);
	CSLDestroy(rpcTransOption);

	//����GCP
	std::vector<GDAL_GCP> gcpVec;
	for (int i = 0; i < gcpNumActual; i++)
	{
		if (!transStatus[i])
			continue;

		GDAL_GCP tempGCP;
		std::stringstream ss;
		ss << i;
		tempGCP.pszId = const_cast<char*>(ss.str().c_str());
		tempGCP.pszInfo = "GF";
		tempGCP.dfGCPLine = lineVecBackup[i];
		tempGCP.dfGCPPixel = pixelVecBackup[i];
		tempGCP.dfGCPX = pixelVec[i];
		tempGCP.dfGCPY = lineVec[i];
		tempGCP.dfGCPZ = zs[i];
		gcpVec.push_back(tempGCP);
	}

	return gcpVec;

}


bool gf_geocode_img(const std::string& gf1Path, const std::string& geocodePath)
{
	GDALAllRegister();

	GDALDataset* imgDataset = (GDALDataset*)GDALOpen(gf1Path.c_str(), GA_ReadOnly);
	if (imgDataset == nullptr)
	{
		std::cout << "Open File Fail " << std::endl;
		return false;
	}

	std::size_t numBand = imgDataset->GetRasterCount();
	if (numBand < 1)
	{
		std::cout << "Image No Band" << std::endl;
		GDALClose(imgDataset);
		return false;
	}

	//��ȡӰ����ߵ���Ϣ
	std::size_t imgWidth = imgDataset->GetRasterXSize();
	std::size_t imgHeight = imgDataset->GetRasterYSize();


	// ��ȡRPCת��������ת��ģ��
	char** rpcMateData = imgDataset->GetMetadata("RPC");
	GDALRPCInfo rpcInfo;
	GDALExtractRPCInfo(rpcMateData, &rpcInfo);

	// ����RPC�������Ƶ�
	std::vector<GDAL_GCP> gcpVec = get_gcp_from_rpc(imgWidth, imgHeight, rpcInfo);
	int gcpNumActual = gcpVec.size();


	//��ȡӰ���е�����
	std::size_t imgBandSize = imgWidth * imgHeight;
	std::size_t imgBufNum = imgBandSize * numBand;
	GByte* imgBuf = new GByte[imgBufNum];
	imgDataset->RasterIO(GF_Read, 0, 0, imgWidth, imgHeight, imgBuf, imgWidth, imgHeight, GDT_Byte, numBand, nullptr, 0, 0, 0);
	GDALClose(imgDataset);

	//�������Ӱ��
	GDALDriver* pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF");
	char** ppszOpt = nullptr;
	ppszOpt = CSLSetNameValue(ppszOpt, "BIGTIFF", "IF_NEEDED");

	GDALDataset* dstDataset = pDriver->Create(geocodePath.c_str(), imgWidth, imgHeight, numBand, GDT_Byte, ppszOpt);
	if (dstDataset == nullptr)
	{
		std::cout << "Create GeoTiff Fail" << std::endl;
		CSLDestroy(ppszOpt);
		return false;
	}

	dstDataset->SetGCPs(gcpNumActual, gcpVec.data(), SRS_WKT_WGS84);    //����GCP
	dstDataset->RasterIO(GF_Write, 0, 0, imgWidth, imgHeight, imgBuf, imgWidth, imgHeight, GDT_Byte, numBand, nullptr, 0, 0, 0);   //��д����

	delete[] imgBuf;
	GDALClose(dstDataset);
	return true;

}


bool gf_reproject_img(const std::string& gf1Path, const std::string& reprojPath)
{
	// ��ȡԭʼӰ��
	GDALAllRegister();

	GDALDatasetH imgDataset = GDALOpen(gf1Path.c_str(), GA_ReadOnly);
	if (imgDataset == nullptr)
	{
		std::cout << "Open File Fail " << std::endl;
		return false;
	}

	std::size_t numBand = ((GDALDataset*)imgDataset)->GetRasterCount();
	if (numBand < 1)
	{
		std::cout << "Image No Band" << std::endl;
		GDALClose(imgDataset);
		return false;
	}

	GDALDataset* dstDataset = (GDALDataset*)GDALWarp(reprojPath.c_str(), nullptr, 1,
		                                             &imgDataset, nullptr, nullptr);
	GDALClose(imgDataset);
	if (dstDataset == nullptr)
	{
		return false;
	}
	else
	{
		GDALClose(dstDataset);
		return true;

	}

}
