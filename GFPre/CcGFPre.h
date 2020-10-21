/*!
* \file CcGFPre.h
* \date 2020/9/17 17:47
*
* \author LiMingci
* Contact: mingcich@126.com
*
* \brief 将带有RPC的高分数据转换成geotiff 
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


// 根据rpc参数创建控制点
std::vector<GDAL_GCP> get_gcp_from_rpc(const std::size_t& imgWidth, const std::size_t& imgHeight, GDALRPCInfo& rpcInfo);


// 将带有RPC的高分数据转换成geotiff 
bool gf_geocode_img(const std::string& gf1Path, const std::string& geocodePath);


// 对影像进行重投影
bool gf_reproject_img(const std::string& gf1Path, const std::string& reprojPath);

