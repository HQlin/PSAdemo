/************************************************ 
* 	文件名称：CalgorithmRegion.h
* 	修改日期：2017年07月05日  
* 	功能：模板搜索区域、模板建立、选择两线基准、贴片计算
************************************************/
#pragma once

#include "HalconCpp.h"
#include "PublicTool.h"
#include "HalconTool.h"
#include "Markup.h"
#include <vector>
#include <algorithm>

using namespace std;
using namespace HalconCpp;

class CalgorithmRegion
{
	public:
		CalgorithmRegion();
		~CalgorithmRegion(void);

	public:
		//像素比率
		//0:底板
		//1:一贴片
		//2:二贴片
		//3:三贴片
		//4:四贴片
		double hv_AhcX[5];
		double hv_AhcY[5];

		//矩形参数
		//1.如果矩形区域1在矩形区域2内，则从矩形区域1往外矩形区域2寻找第一个矩形区域；
		//2.如果矩形区域1在矩形区域2外，则从矩形区域1往内矩形区域2寻找第一个矩形区域；
		//3.矩形中心为基准点，矩形方向为基准线。
		struct RectsParameter{			
			//目标矩形区域1参数
			HTuple hv_Row_rectangle_out;
			HTuple hv_Column_rectangle_out;
			HTuple hv_Phi_rectangle_out;
			HTuple hv_Length1_rectangle_out;
			HTuple hv_Length2_rectangle_out;	
			//目标矩形区域2参数
			HTuple hv_Row_rectangle_in;
			HTuple hv_Column_rectangle_in;
			HTuple hv_Phi_rectangle_in;
			HTuple hv_Length1_rectangle_in;
			HTuple hv_Length2_rectangle_in;
		};

		//圆弧参数
		//1.如果圆形区域11在圆形区域12内，则从圆形区域11往外圆形区域12寻找第一个圆弧区域；
		//2.如果圆形区域11在圆形区域12外，则从圆形区域11往内圆形区域12寻找第一个圆弧区域；
		//3.与矩形参数区别在于需要第二个圆弧区域，两个圆形确定一条基准线，一个圆心为基准点。
		struct CirclesParameter{
			//目标圆形区域11参数
			HTuple hv_Row_circle_out;
			HTuple hv_Column_circle_out;
			HTuple hv_Radius_circle_out;
			//目标圆形区域12参数
			HTuple hv_Row_circle_in;
			HTuple hv_Column_circle_in;
			HTuple hv_Radius_circle_in;

			//目标圆形区域21参数
			HTuple hv_Row_circle_out1;
			HTuple hv_Column_circle_out1;
			HTuple hv_Radius_circle_out1;
			//目标圆形区域22参数
			HTuple hv_Row_circle_in1;
			HTuple hv_Column_circle_in1;
			HTuple hv_Radius_circle_in1;

			//基准点
			//0:第一个圆弧区域圆心
			//1:第二个圆弧区域圆心
			int basepoint;
		};

		//双直线参数
		//1.目标直线区域1，以矩形方向寻找第一条直线；
		//2.目标直线区域2，以矩形方向寻找第二条直线；
		//3.两直线交点为基准点，一直线为基准线
		struct LinesParameter{
			//目标直线区域1参数
			HTuple hv_Row_rec;
			HTuple hv_Column_rec;
			HTuple hv_Phi_rec;
			HTuple hv_Length1_rec;
			HTuple hv_Length2_rec;
			//目标直线区域2参数
			HTuple hv_Row_rec1;
			HTuple hv_Column_rec1;
			HTuple hv_Phi_rec1;
			HTuple hv_Length1_rec1;
			HTuple hv_Length2_rec1;

			//基准线
			//0:第一条直线
			//1:第二条直线
			int baseline;
		};

		//单线段
		//1.目标单线段区域，以矩形方向寻找第一条直线；
		//2.单线段中心为基准点，线段为基准线。
		struct LineParameter{
			//目标单线段区域参数
			HTuple hv_Row_rec;
			HTuple hv_Column_rec;
			HTuple hv_Phi_rec;
			HTuple hv_Length1_rec;
			HTuple hv_Length2_rec;
		};

		//单圆弧参数
		//1.如果圆形区域1在圆形区域2内，则从圆形区域1往外圆形区域2寻找第一个圆弧区域；
		//2.如果圆形区域1在圆形区域2外，则从圆形区域1往内圆形区域2寻找第一个圆弧区域；
		//3.单圆弧圆心为基准点，没有基准线
		struct CircleParameter{
			//目标圆形区域1参数
			HTuple hv_Row_circle_out;
			HTuple hv_Column_circle_out;
			HTuple hv_Radius_circle_out;
			//目标圆形区域2参数
			HTuple hv_Row_circle_in;
			HTuple hv_Column_circle_in;
			HTuple hv_Radius_circle_in;
		};

		//模板参数
		struct ModelParameter{
			//模板名称
			CString model_Name;
			//模板路径
			CString model_Path;
			//单位像素长度
			HTuple	hv_Ahc;
			//搜索区域【矩形】
			HTuple hv_Row1;
			HTuple hv_Column1;
			HTuple hv_Row2;
			HTuple hv_Column2;

			//开始角度 角度范围
			HTuple hv_AStart;
			HTuple hv_ARange;
			//合格分数
			HTuple hv_Score;		
			//模板角度
			HTuple hv_Angle;		
			//模板中心坐标
			HTuple hv_Row;		
			HTuple hv_Column;	

			//模板区域【矩形】
			HTuple hv_Row_center_rec_model;
			HTuple hv_Column_center_rec_model;
			HTuple hv_Length1_rec_model;
			HTuple hv_Length2_rec_model;

			//模板重心坐标
			HTuple hv_Row_gravity_model;
			HTuple hv_Column_gravity_model;

			//图像增强对比参数
			HTuple hv_Mult;
			HTuple hv_Add;
			bool is_Mult;

			//是否中值滤波(去掉小孔，细缝)
			int hv_median_bool;
			//滤波器掩码
			int hv_Parameter_median;

			//识别方式
			//:模板匹配
			//Lines:双直线匹配
			//Circles:双圆弧匹配
			//Rects:矩形匹配
			//Line:单直线匹配
			//SolidCircle:单圆弧匹配
			CString check;
			
			//双直线参数
			LinesParameter linesParameter;
			//双圆弧参数
			CirclesParameter circlesParameter;
			//矩形参数
			RectsParameter rectsParameter;
			//单直线参数
			LineParameter lineParameter;
			//单圆弧参数
			CircleParameter circleParameter;

			//模板变量
			HTuple hv_ModelID;
		};

		//方案名称
		CString pro;
		//xml路径
		CString UpSetting, DownSetting;

	public:
		//选定单圆弧区域
		bool SetCircleRect(HalconTool &Tool, HObject &scrImage,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool SetUpCircleRect(HalconTool &ToolUp, HObject &imageUp,
			ModelParameter &Parameter);
		bool SetDownCircleRect(HalconTool &ToolDown, HObject &imageDown,
			ModelParameter &Parameter);

		//选定单直线区域
		bool SetLineRect(HalconTool &Tool, HObject &scrImage,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool SetUpLineRect(HalconTool &Tool, HObject &scrImage,
			ModelParameter &Parameter);
		bool SetDownLineRect(HalconTool &Tool, HObject &scrImage,
			ModelParameter &Parameter);

		//选定矩形区域
		bool SetRectsRect(HalconTool &Tool, HObject &scrImage,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool SetUpRectsRect(HalconTool &ToolUp, HObject &imageUp,
			ModelParameter &Parameter);
		bool SetDownRectsRect(HalconTool &ToolDown, HObject &imageDown,
			ModelParameter &Parameter);

		//选定圆弧区域
		bool SetCirclesRect(HalconTool &Tool, HObject &scrImage, int index,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool SetUpCirclesRect(HalconTool &ToolUp, HObject &imageUp, int index, 
			ModelParameter &Parameter);
		bool SetDownCirclesRect(HalconTool &ToolDown, HObject &imageDown, int index,
			ModelParameter &Parameter);

		//选定直线区域
		bool SetLinesRect(HalconTool &Tool, HObject &scrImage, int index,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool SetUpLinesRect(HalconTool &ToolUp, HObject &imageUp, int index,
			ModelParameter &Parameter);
		bool SetDownLinesRect(HalconTool &ToolDown, HObject &imageDown, int index,
			ModelParameter &Parameter);

		//图像预处理
		HObject SetPretreatImage(HalconTool &Tool, HObject &scrImage,
			CString Setting, ModelParameter &Parameter, CString modelName);

		//中值滤波器
		HObject SetMedianImage(HalconTool &Tool, HObject &scrImage, int hv_median_bool, int hv_Parameter_median,
			CString Setting, ModelParameter &Parameter, CString modelName);

		//增强原图像对比度
		HObject SetMultImage(HalconTool &Tool, HObject &scrImage, HTuple hv_Mult, HTuple hv_Add, bool is_Mult, 
			CString Setting, ModelParameter &Parameter, CString modelName);

		//选择贴片、底板模板
		bool BuildModel(HalconTool &Tool, HObject &scrImage,
			int UseRect2,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool BuildUpModel(HalconTool &ToolUp, HObject &imageUp,
			int UseRect2,
			ModelParameter &Parameter);
		bool BuildDownModel(HalconTool &ToolDown, HObject &imageDown,
			int UseRect2,
			ModelParameter &Parameter);

		//贴片、底板搜索区域
		bool SetRect(HalconTool &Tool, HObject &scrImage, 
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool SetUpRect(HalconTool &ToolUp, HObject &imageUp,
			ModelParameter &Parameter);
		bool SetDownRect(HalconTool &ToolDown, HObject &imageDown,
			ModelParameter &Parameter);

		//贴片、底板识别
		//HalconTool &Tool:显示图像工具
		//HObject &image:识别原图像
		//vector<double> &results:识别结果数据
		//CString Setting:参数文件路径
		//ModelParameter &Parameter:模板参数
		//bool isSaveImage:是否保存识别原图像
		bool FindModel(HalconTool &Tool, HObject &image, vector<double> &results,
			CString Setting, ModelParameter &Parameter, bool isSaveImage);
		bool FindUpModel(HalconTool &ToolUp, HObject &imageUp, vector<double> &results,
			ModelParameter &Parameter, bool isSaveImage);
		bool FindDownModel(HalconTool &ToolDown, HObject &imageDown, vector<double> &results,
			ModelParameter &Parameter, bool isSaveImage);

		//保存贴片、底板参数：搜索开始角度 角度范围 合格分数 识别方式 基准线 基准点
		bool SaveParameter(CString Setting, ModelParameter &Parameter);
		bool SaveUpParameter(ModelParameter &Parameter);
		bool SaveDownParameter(ModelParameter &Parameter);

		//读取贴片、底板所有模板名称
		CString GetModels(CString Setting, vector<CString> &Models, ModelParameter &Parameter, CString dir);
		CString GetUpModels(vector<CString> &upModels, ModelParameter &Parameter);
		CString GetDownModels(vector<CString> &downModels, ModelParameter &Parameter);

		//指定贴片、底板模板名称
		bool SetModel(CString Setting, CString Selected, ModelParameter &Parameter);
		bool SetUpModel(CString upSelected, ModelParameter &Parameter);
		bool SetDownModel(CString downSelected, ModelParameter &Parameter);

		//新模板添加
		bool AddModel(CString Setting, CString newModel, ModelParameter &Parameter, CString dir);
		bool AddUpModel(CString newModel, ModelParameter &Parameter);
		bool AddDownModel(CString newModel, ModelParameter &Parameter);

		//删除模板
		bool DeleteModel(CString Setting, CString model, ModelParameter &Parameter, CString dir);
		bool DeleteUpModel(CString model, ModelParameter &Parameter);
		bool DeleteDownModel(CString model, ModelParameter &Parameter);

		//贴合时计算结果
		bool GetResult(vector<double> &parameters, vector<double> &results);

		//添加方案
		bool AddPro(CString pro);

		//选择方案
		bool SetPro(CString pro);

		//保存图像
		bool SaveImage(CString dirPath, HObject image);

		//寻找吸嘴中心位置
		bool FindCenterPoint(vector<HObject> ho_ImageVector, vector<double> &results,
			HalconTool &Tool, HTuple hv_Ahc, bool isSaveImage);

		//计算相机比率
		bool FindCamScale(vector<HObject> ho_ImageVector, vector<double> &results,
			CString Setting, ModelParameter &Parameter, CString modelName,
			HalconTool &Tool, HTuple hv_X_move, HTuple hv_Y_move);

		//模拟贴合位置(图像坐标、逆时针-)
		bool DrawUponDown(HalconTool &ToolDown, HObject &scrImage, 
			vector<double> &parameters, ModelParameter &DownParameter, ModelParameter &UpParameter);

		//显示模板
		void dev_display_shape_matchinresults (vector<HObject> &outlines, 
			HTuple hv_ModelID, 
			HTuple hv_Row, HTuple hv_Column, HTuple hv_Angle, 
			HTuple hv_ScaleR, HTuple hv_ScaleC, HTuple hv_Model);
		
		//0==mode当前模板、1==mode还原模板
		bool RestoreModel(HalconTool &Tool, HObject &scrImage, 
			HObject &ho_Rectangle, HObject &ho_Rectangle_model, 
			HObject &ho_RubberRectangle, HObject &ho_Region_xld, int mode,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool RestoreUpModel(HalconTool &ToolUp, HObject &imageUp, 
			HObject &ho_Rectangle, HObject &ho_Rectangle_model, 
			HObject &ho_RubberRectangle, HObject &ho_Region_xld, int mode,
			ModelParameter &Parameter);
		bool RestoreDownModel(HalconTool &ToolDown, HObject &imageDown, 
			HObject &ho_Rectangle, HObject &ho_Rectangle_model, 
			HObject &ho_RubberRectangle, HObject &ho_Region_xld, int mode,
			ModelParameter &Parameter);
		//擦除模板
		bool EditModel(HalconTool &Tool, HObject &scrImage, 
			HObject &ho_Rectangle, HObject &ho_Rectangle_model, 
			HObject &ho_RubberRectangle, HObject &ho_Region_xld);
		//保存模板
		bool SaveModel(HalconTool &Tool, HObject &scrImage,
			HObject &ho_Rectangle_model,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool SaveUpModel(HalconTool &ToolUp, HObject &imageUp,
			HObject &ho_Rectangle_model,
			ModelParameter &Parameter);
		bool SaveDownModel(HalconTool &ToolDown, HObject &imageDown,
			HObject &ho_Rectangle_model,
			ModelParameter &Parameter);
		//提取重心
		bool SaveGravity(HalconTool &Tool, HObject &scrImage, 
			HObject &ho_Rectangle, HObject &ho_Rectangle_model, HObject &ho_RubberRectangle, HObject &ho_Region_xld,
			CString Setting, ModelParameter &Parameter, CString modelName);
		bool SaveUpGravity(HalconTool &ToolUp, HObject &imageUp, 
			HObject &ho_Rectangle, HObject &ho_Rectangle_model, HObject &ho_RubberRectangle, HObject &ho_Region_xld,
			ModelParameter &Parameter);
		bool SaveDownGravity(HalconTool &ToolDown, HObject &imageDown, 
			HObject &ho_Rectangle, HObject &ho_Rectangle_model, HObject &ho_RubberRectangle, HObject &ho_Region_xld,
			ModelParameter &Parameter);

		//像素转中心坐标mm
		bool Pix2CenterMM(double &hv_AhcX, double &hv_AhcY, CString modelName);

		//计算点在Mark坐标系中的位置
		bool Machine2Board(HTuple hv_X_Mark, HTuple hv_Y_Mark, HTuple hv_R_Mark, HTuple hv_X_Point, HTuple hv_Y_Point,
			double &hv_X_Point2Mark, double &hv_Y_Point2Mark);

		//计算点在机械坐标系中的位置
		//1
		bool Board2Machine(vector<double> &marks, 
			HTuple hv_X_Point2Mark, HTuple hv_Y_Point2Mark,
			HTuple hv_NumberOfRow_Baseplate, HTuple hv_NumberOfCol_Baseplate,
			HTuple hv_GapOfRow_Baseplate, HTuple hv_GapOfCol_Baseplate,
			double &hv_X_Point, double &hv_Y_Point);

		//2
		bool Board2Machine(HTuple hv_X_Mark, HTuple hv_Y_Mark, HTuple hv_R_Mark, HTuple hv_R_Mark1, 
			HTuple hv_X_Point2Mark, HTuple hv_Y_Point2Mark,
			double &hv_X_Point, double &hv_Y_Point);

		//3
		bool Board2Machine(vector<double> &parameters, vector<double> &results);

		//计算底板整体角度
		bool GetMarksXYR(vector<double> &marks, double &hv_Angle_Mark, double &hv_Angle_Mark1);

		//限制角度范围0~180 0~-180
		double LitmitAngle(double Angle);

		//求中值
		double MedianVector(vector<double> &degAngle);

		//计算点在Mark坐标系中的位置
		bool BulidBoard(vector<double> &parameters, vector<double> &resultXs, vector<double> &resultYs);

		//计算点在Mark坐标系中的位置（两点相对关系）
		void DistanceAndAngle (HTuple hv_X_Mark, HTuple hv_Y_Mark, HTuple hv_X_Point, HTuple hv_Y_Point, 
			HTuple *hv_Distance_Point2Mark, HTuple *hv_Angle_Point2Mark);

		//手动生成数据
		bool GetAllDatas(vector<double> &parameters, vector<double> &resultXs, vector<double> &resultYs, vector<double> &resultRs);

		//保存参数数据比较
		bool CompareParameters(ModelParameter &saveParameter, ModelParameter &Parameter);
		bool CompareUpParameters(ModelParameter &Parameter);
		bool CompareDownParameters(ModelParameter &Parameter);
};