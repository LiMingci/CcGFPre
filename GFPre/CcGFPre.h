/*!
* \file CcGFPre.h
* \date 2020/9/17 17:47
*
* \author LiMingci
* Contact: mingcich@126.com
*
* \brief ������RPC�ĸ߷�����ת����geotiff 
*
*
* \note
*/
#pragma once
#include <vector>

#include <gdal.h>
#include <gdal_priv.h>
#include <gdal_alg.h>
#include <gdal_mdreader.h>
#include <gdalwarper.h>
#include <gdal_utils.h>


// ����rpc�����������Ƶ�
std::vector<GDAL_GCP> get_gcp_from_rpc(const std::size_t& imgWidth, const std::size_t& imgHeight, GDALRPCInfo& rpcInfo);


// ������RPC�ĸ߷�����ת����geotiff 
bool gf_geocode_img(const std::string& gf1Path, const std::string& geocodePath);


// ��Ӱ�������ͶӰ
bool gf_reproject_img(const std::string& gf1Path, const std::string& reprojPath);

