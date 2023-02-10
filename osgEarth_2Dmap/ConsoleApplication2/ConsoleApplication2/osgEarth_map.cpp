// ConsoleApplication2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <Windows.h>
#include <iostream>
#include <string>

#include <osgViewer/Viewer>
#include <osgDB/ReadFile>

#include <osg/MatrixTransform> //移动节点的矩阵类，最常用的移动节点的类。可随动、旋转控制节点
#include <osg/PositionAttitudeTransform> //位置变换节点类，提供模型的位置变换、大小缩放、原点位置的设置、坐标系的变换
#include <osg/Camera>  //相机节点，管理OSG中的模型——视图矩阵，相机的管理主要是通过各种变换实现的


#include <osgEarth/MapNode>
#include <osgEarth/Map>

#include <osgEarth/GDAL>

#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#include <osgEarth/ImageLayer>

//包名不符合
#include <osgEarth/EarthManipulator>

#include<gdal_priv.h>
#include "ogr_core.h"
#include "proj_api.h"



using namespace std;

int main()
{
	//Web墨卡托投影平面坐标系的EPSG代码是3857，所以只需要直接传入相应的代码就行了。此处设置GDAL_DATA环境变量，也可直接设置。
	CPLSetConfigOption("GDAL_DATA", "D:/Work/OSGNewBuild/OpenSceneGraph-3.6.4/3rdParty/x64/gdal-data");
	//两种投影代码对应两种显示格式，二维与三维
	string wktString = "EPSG:3857";			//web墨卡托投影
	//string wktString = "EPSG:4326";			//wgs84
	osgEarth::ProfileOptions profileOpts;
	string srs;
	profileOpts.srsString() = wktString;
	//地图配置：设置缓存目录
	osgEarth::Drivers::FileSystemCacheOptions cacheOpts;
	string cacheDir = "D:/OSGEarth-3.1-install-vs2017-x64-for-OSG-3.4.1/work/tmp";
	cacheOpts.rootPath() = cacheDir;
	//问题包
	osgEarth::Map::Options mapOpts;
	mapOpts.cache() = cacheOpts;

	//Osgeartg3.x中删去了mapOptions中的coordSysType该属性，也为提及涉及到投影形式设定函数，此处存疑，可能影响二维地图视点等
	//mapOpts.coordSysType() = osgEarth::MapOptions::CSTYPE_PROJECTED;//二维地图时，此处需要设置为投影形式
	//缓存策略
	mapOpts.cachePolicy() = osgEarth::CachePolicy::USAGE_READ_WRITE;
	mapOpts.profile() = profileOpts;
	//初始化，可去除
	osgEarth::initialize();


	//创建地图节点
	osg::ref_ptr<osgEarth::Map> map = new osgEarth::Map(mapOpts);
	osg::ref_ptr<osgEarth::MapNode> mapNode = new osgEarth::MapNode(map);

	//读取地图图片
	osg::ref_ptr<osgEarth::GDALImageLayer> baselayer = new osgEarth::GDALImageLayer();
	baselayer->setURL("D:/OSGEarth-3.1-install-vs2017-x64-for-OSG-3.4.1/data/world.tif");

     //2.x版本中ImageLayer的使用，3x中去除了中间层。
	//osgEarth::GDAL::Options gdal;
	//gdal.url() = "D:/OSGEarth-3.1-install-vs2017-x64-for-OSG-3.4.1/data/world.tif";
	//osg::ref_ptr<osgEarth::ImageLayer> layer = new osgEarth::ImageLayer("BlueMarble", gdal);
	map->addLayer(baselayer);

	//显示与漫游器设置
	osgViewer::Viewer viewer;
	viewer.setSceneData(mapNode);
	osg::ref_ptr<osg::Camera> camera = viewer.getCamera();

	//相机到focalPoint点的距离，也就是高度
	//vp.range() = 100;
	//camera->setViewport(new osgEarth::Viewport::Viewpoint(0, 0, 100, 200,100,100));//设置视口
	//viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES);
	//viewer.getCamera()->setNearFarRatio(0.000000000000000000009);

	//osgEarth::Bounds bound = baselayer->getTileSource()->getDataExtents().data()->bounds();
	//double x = bound.center2d().x();
	//double y = bound.center2d().y();
	//osgEarth::Viewpoint vp("", x, y, 0.0, 0, -90.0, 2000);
	//mEM->setViewpoint(vp, 8);

	osg::ref_ptr< osgEarth::Util::EarthManipulator> mainManipulator = new osgEarth::Util::EarthManipulator;
	//设置相机视点，三维显示无需，二维地图会出现地图贴到摄像头导致地图显示不全问题，原因暂不明，推测与设置投影形式有关？
	//定义视点类对象
	osgEarth::Viewpoint vp = mainManipulator->getViewpoint();
	//osgEarth::Viewpoint vp(0.01, 0, 0, 0);
	//vp.focalPoint()->set(geoSRS, logtide, latide, 0, osgEarth::ALTMODE_ABSOLUTE);
	//设置相机相对于焦点的俯仰
	vp.setNode(mapNode);

	vp.pitch()->set(-90, osgEarth::Units::DEGREES);
	vp.range()->set(99999999.0, osgEarth::Units::METERS);

	viewer.setCameraManipulator(mainManipulator);

	viewer.setUpViewInWindow(100, 100,800,600);
	
	mainManipulator->setViewpoint(vp);

	return viewer.run();

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
