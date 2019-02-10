#include "StdAfx.h"
#include "CalgorithmRegion.h"

CalgorithmRegion::CalgorithmRegion()
{
	//底板像素比例，分XY方向
	hv_AhcX[0] = 0.1;
	hv_AhcY[0] = 0.1;

	//一二贴片像素比例，1号相机分一二吸嘴，分XY方向
	hv_AhcX[1] = 0.1;
	hv_AhcY[1] = 0.1;
	hv_AhcX[2] = 0.1;
	hv_AhcY[2] = 0.1;

	//三四贴片像素比例，2号相机分三四吸嘴，分XY方向
	hv_AhcX[3] = 0.1;
	hv_AhcY[3] = 0.1;
	hv_AhcX[4] = 0.1;
	hv_AhcY[4] = 0.1;
}

CalgorithmRegion::~CalgorithmRegion(void)
{
	
}

//选定单圆弧区域
bool CalgorithmRegion::SetCircleRect(HalconTool &Tool, HObject &scrImage,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	Tool.uplines.clear();
	Tool.downlines.clear();
	Tool.outlines.clear();

	//图像预处理
	dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
	
	try{
		Tool.strReslut = L"选定单圆弧区域--点击右键确定";
		Tool.ShowImage(dstImage);

		HObject  ho_Circle_out, ho_Circle_in, ho_Difference_circle, ho_ImageReduced,
			ho_Edges, ho_UnionContours, ho_SelectedContours, ho_SortedContours, ho_edge_i, ho_ObjectSelected,
			ho_Circle, ho_SelectedXLD;
		HTuple hv_Number_edges, hv_radius_max_object, hv_radius_min_object, 
			hv_Index_max_obj, hv_Index_in_obj, hv_i, hv_Row2, 
			hv_Column2, hv_Radius_i, hv_StartPhi2, hv_EndPhi2, hv_PointOrder2, hv_Row_circle_center, 
			hv_Column_circle_center, hv_Radius_circle, hv_StartPhi, hv_EndPhi, hv_PointOrder;

		//************************选定目标区域1********************
		DrawCircle(Tool.hv_WindowHandle, 
			&Parameter.circleParameter.hv_Row_circle_out, 
			&Parameter.circleParameter.hv_Column_circle_out, 
			&Parameter.circleParameter.hv_Radius_circle_out);
		GenCircle(&ho_Circle_out, 
			Parameter.circleParameter.hv_Row_circle_out, 
			Parameter.circleParameter.hv_Column_circle_out,
			Parameter.circleParameter.hv_Radius_circle_out);
		Tool.outlines.push_back(ho_Circle_out);
		Tool.ShowImage(dstImage);
		DrawCircle(Tool.hv_WindowHandle, 
			&Parameter.circleParameter.hv_Row_circle_in, 
			&Parameter.circleParameter.hv_Column_circle_in, 
			&Parameter.circleParameter.hv_Radius_circle_in);
		GenCircle(&ho_Circle_in, 
			Parameter.circleParameter.hv_Row_circle_in, 
			Parameter.circleParameter.hv_Column_circle_in, 
			Parameter.circleParameter.hv_Radius_circle_in);
		Tool.outlines.push_back(ho_Circle_in);
		Tool.ShowImage(dstImage);
		SymmDifference(ho_Circle_out, ho_Circle_in, &ho_Difference_circle);

		//******提取目标区域1内的轮廓*********
		ReduceDomain(dstImage, ho_Difference_circle, &ho_ImageReduced);
		EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 20, 40);
		UnionAdjacentContoursXld(ho_Edges,&ho_Edges,15,1,"attr_keep");
		//选择长度大于20的边缘
		UnionCocircularContoursXld(ho_Edges, &ho_UnionContours, HTuple(10).TupleRad(), 
			HTuple(3).TupleRad(), HTuple(10).TupleRad(), 50, 10, 10, "true", 1);
		SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 50, 10000, 0, 0);
		//SegmentContoursXld (ho_SelectedContours,  &ho_SelectedContours, "lines_circles", 5, 4, 2);
		//SelectShapeXld(ho_SelectedContours, &ho_SelectedXLD, "circularity", "and", 0.05, 1);
		//************排序选择最长的线并画直线*****************
		//排序
		SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
			"row");
		//求面积
		//area_center_points_xld (SortedContours, Area_SortedContours, Row_SortedContours, Column_SortedContours)
		//计算目标数
		CountObj(ho_SortedContours, &hv_Number_edges);
		if (0 != (hv_Number_edges==0))
		{
			AfxMessageBox(L"单圆弧区域未提取到边缘");
		}
		else
		{
			//拟合半径参数
			hv_radius_max_object = 0;
			hv_radius_min_object = 1000;
			//目标轮廓索引
			hv_Index_max_obj = HTuple();
			hv_Index_in_obj = HTuple();
			//找到半径最大和最小的索引
			HTuple end_val159 = hv_Number_edges-1;
			HTuple step_val159 = 1;
			for (hv_i=0; hv_i.Continue(end_val159, step_val159); hv_i += step_val159)
			{
				SelectObj(ho_SortedContours, &ho_edge_i, hv_i+1);
				FitCircleContourXld(ho_edge_i, "algebraic", -1, 0, 0, 3, 2, &hv_Row2, 
					&hv_Column2, &hv_Radius_i, &hv_StartPhi2, &hv_EndPhi2, &hv_PointOrder2);
				if(((Parameter.circleParameter.hv_Radius_circle_out>hv_Radius_i)&&(hv_Radius_i>Parameter.circleParameter.hv_Radius_circle_in))
					||((Parameter.circleParameter.hv_Radius_circle_out<hv_Radius_i)&&(hv_Radius_i<Parameter.circleParameter.hv_Radius_circle_in)))
				{
					if (0 != (hv_Radius_i>hv_radius_max_object))
					{
						hv_radius_max_object = hv_Radius_i;
						hv_Index_max_obj = hv_i+1;
					}
					if (0 != (hv_Radius_i<hv_radius_min_object))
					{
						hv_radius_min_object = hv_Radius_i;
						hv_Index_in_obj = hv_i+1;
					}
				}						
			}
			if (0 != (Parameter.circleParameter.hv_Radius_circle_out>Parameter.circleParameter.hv_Radius_circle_in))
			{
				SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_max_obj);
			}
			else
			{
				SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_in_obj);
			}
			//拟合圆并计算参数
			FitCircleContourXld(ho_ObjectSelected, "atukey", -1, 0, 0, 3, 2, &hv_Row_circle_center, 
				&hv_Column_circle_center, &hv_Radius_circle, &hv_StartPhi, &hv_EndPhi, 
				&hv_PointOrder);

			//画出圆
			GenCircle(&ho_Circle, hv_Row_circle_center, hv_Column_circle_center, hv_Radius_circle);
		}
		Tool.outlines.clear();
		Tool.strReslut = L"选定单圆弧区域--已确定";
		Tool.outlines.push_back(ho_Circle);
		Tool.outlines.push_back(ho_Difference_circle);
		Tool.ShowImage(dstImage);
	}
	catch(...)
	{
		AfxMessageBox(L"选定实心圆弧区域失败");
		return false;
	}		
	return true;
}
bool CalgorithmRegion::SetUpCircleRect(HalconTool &ToolUp, HObject &imageUp,
	ModelParameter &Parameter)
{
	return SetCircleRect(ToolUp, imageUp,
		UpSetting, Parameter, Parameter.model_Name);

}
bool CalgorithmRegion::SetDownCircleRect(HalconTool &ToolDown, HObject &imageDown,
	ModelParameter &Parameter)
{
	return SetCircleRect(ToolDown, imageDown,
		DownSetting, Parameter, Parameter.model_Name);
}

//选定单直线
bool CalgorithmRegion::SetLineRect(HalconTool &Tool, HObject &scrImage,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	// Local iconic variables 
	HObject  dstImage, ho_Rectangle_model, ho_Domain_model;
	HObject  ho_Rectangle_obj, ho_image_obj;
	HObject  ho_Image_median, ho_Domain_Ractangle, ho_Edges;
	HObject  ho_UnionContours, ho_SelectedContours, ho_SortedContours;
	HObject  ho_ObjectSelected, ho_Regions_line, ho_RegionIntersection;
	HObject  ho_Rectangle_obj1,ho_RegionIntersection1;
					
	// Local control variables 
	HTuple	hv_Area1, hv_Row_LineCenter1, hv_Column_LineCenter1;
	HTuple  hv_Width_model, hv_Height_model;
	HTuple  hv_mult_bool, hv_Parameter_mult, hv_Parameter_add;
	HTuple  hv_Row1_rec;
	HTuple  hv_Column1_rec, hv_Row2_rec, hv_Column2_rec, hv_Row_center_rec_model;
	HTuple  hv_Column_center_rec_model, hv_Angle_rec, hv_Length1_rec_model;
	HTuple  hv_Length2_rec_model, hv_Constract;
	HTuple  hv_Row_model, hv_Column_model, hv_Angle_model, hv_Score;
	HTuple  hv_ImageFiles, hv_Index, hv_Width_Image;
	HTuple  hv_Height_Image, hv_Seconds1, hv_Row, hv_Column;
	HTuple  hv_Seconds2, hv_HomMat2D_model;
	HTuple  hv_Number_edges, hv_RowBegin, hv_ColBegin;
	HTuple  hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc, hv_Dist, hv_angle_line;
	HTuple  hv_Area, hv_Row_LineCenter, hv_Column_LineCenter;
	HTuple  hv_Angle1, hv_DegAngle, hv_Angle_mark, hv_Row_mark;
	HTuple  hv_Row_corner, hv_Column_mark, hv_Column_corner;
	HTuple  hv_Angle_line;

	Tool.uplines.clear();
	Tool.downlines.clear();
	Tool.outlines.clear();
	Tool.strReslut = L"选定单直线区域--点击右键确定";
	//图像预处理
	dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
	Tool.ShowImage(dstImage);

	try{
		DrawRectangle2(Tool.hv_WindowHandle, &Parameter.lineParameter.hv_Row_rec, &Parameter.lineParameter.hv_Column_rec, &Parameter.lineParameter.hv_Phi_rec, &Parameter.lineParameter.hv_Length1_rec, 
			&Parameter.lineParameter.hv_Length2_rec);
		GenRectangle2(&ho_Rectangle_obj, 
			Parameter.lineParameter.hv_Row_rec, 
			Parameter.lineParameter.hv_Column_rec, 
			Parameter.lineParameter.hv_Phi_rec, 
			Parameter.lineParameter.hv_Length1_rec, 
			Parameter.lineParameter.hv_Length2_rec);
		GenRectangle2(&ho_Rectangle_obj1, 
			(((Parameter.lineParameter.hv_Phi_rec).TupleSin())*Parameter.lineParameter.hv_Length1_rec), 
			(((Parameter.lineParameter.hv_Phi_rec).TupleCos())*Parameter.lineParameter.hv_Length1_rec), 
			Parameter.lineParameter.hv_Phi_rec, 
			Parameter.lineParameter.hv_Length2_rec, 
			Parameter.lineParameter.hv_Length2_rec);

		ReduceDomain(dstImage, ho_Rectangle_obj, &ho_image_obj);

		//******提取目标区域1内的轮廓*********
		MedianImage(dstImage, &ho_Image_median, "square", Parameter.hv_Parameter_median, "mirrored");
		ReduceDomain(ho_Image_median, ho_Rectangle_obj, &ho_Domain_Ractangle);
		EdgesSubPix(ho_Domain_Ractangle, &ho_Edges, "canny", 1, 20, 40);
		UnionAdjacentContoursXld(ho_Edges, &ho_UnionContours, 15, 1, "attr_keep");
		/////////////////////////20180528修改///////////////////////
		//选择长度大于20的边缘
		SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 
			20, 10000, 0, 0);
		//选择头尾相差超过20的轮廓
		SelectContoursXld(ho_SelectedContours, &ho_SelectedContours, "open", 20, 10000, 
			0, 0);
		//*矩形框角度在-45到45之间
		if (0 != ((Parameter.lineParameter.hv_Phi_rec.TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
		{
			SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
				"column");
			//求面积
		}
		//*矩形框角度在45到135之间
		if (0 != (((Parameter.lineParameter.hv_Phi_rec-(HTuple(90).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
		{
			SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "false", 
				"row");
		}
		//*矩形框角度135到225之间
		if (0 != (((Parameter.lineParameter.hv_Phi_rec-(HTuple(180).TupleRad())).TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
		{
			SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "false", 
				"column");
		}
		//*矩形框角度在225到315（-45）之间
		if (0 != (((Parameter.lineParameter.hv_Phi_rec-(HTuple(270).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
		{
			SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
				"row");
		}

		CountObj(ho_SortedContours, &hv_Number_edges);
		if (0 != (hv_Number_edges==0))
		{
			AfxMessageBox(L"单直线区域未提取到边缘");
		}
		else
		{

			//选择序列中的第一个轮廓
			SelectObj(ho_SortedContours, &ho_ObjectSelected, 1);
			//拟合直线并计算参数
			FitLineContourXld(ho_ObjectSelected, "drop", -1, 10, 5, 1, &hv_RowBegin, 
				&hv_ColBegin, &hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);

			if (0 != (hv_Nc>0))
			{
				hv_angle_line = (hv_Nr/hv_Nc).TupleAtan();
			}
			else if (0 != (hv_Nc==0))
			{
				hv_angle_line = hv_Nr*(HTuple(90).TupleRad());
			}
			else
			{
				hv_angle_line = ((hv_Nr/hv_Nc).TupleAtan())+(HTuple(180).TupleRad());
			}
			//画出直线
			GenRegionHline(&ho_Regions_line, hv_angle_line, hv_Dist);
			Intersection(ho_Regions_line, ho_Rectangle_obj, &ho_RegionIntersection);
			Intersection(ho_Regions_line, ho_Rectangle_obj1, &ho_RegionIntersection1);	
		}

		//****求线中点
		AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Row_LineCenter, &hv_Column_LineCenter);

		AreaCenter(ho_RegionIntersection1, &hv_Area1, &hv_Row_LineCenter1, &hv_Column_LineCenter1);
		AngleLx(hv_Row_LineCenter, hv_Column_LineCenter, hv_Row_LineCenter1, hv_Column_LineCenter1, 
			&hv_Angle1);
		hv_Angle_mark = hv_Angle1;

		hv_Angle_mark = hv_Angle1.TupleDeg();

		//在重心画十字线
		HObject ho_RegionLines,ho_RegionLines1,ho_Region_gravity;
		GenRegionLine(&ho_RegionLines, hv_Row_LineCenter+8, hv_Column_LineCenter, 
			hv_Row_LineCenter-8, hv_Column_LineCenter);
		GenRegionLine(&ho_RegionLines1, hv_Row_LineCenter, hv_Column_LineCenter-8, 
			hv_Row_LineCenter, hv_Column_LineCenter+8);
		Union2(ho_RegionLines, ho_RegionLines1, &ho_Region_gravity);
		//直线
		Tool.outlines.push_back(ho_RegionIntersection);
		//中心
		Tool.outlines.push_back(ho_Region_gravity);
		Tool.ShowImage(dstImage);
	}
	catch(...)
	{
		AfxMessageBox(L"选定单直线区域失败");
		return false;
	}
	return true;
}
bool CalgorithmRegion::SetUpLineRect(HalconTool &Tool, HObject &scrImage,
	ModelParameter &Parameter)
{
	return SetLineRect(Tool, scrImage,
		UpSetting, Parameter, Parameter.model_Name);
}
bool CalgorithmRegion::SetDownLineRect(HalconTool &Tool, HObject &scrImage,
	ModelParameter &Parameter)
{
	return SetLineRect(Tool, scrImage,
		DownSetting, Parameter, Parameter.model_Name);
}


//选定矩形区域
bool CalgorithmRegion::SetRectsRect(HalconTool &Tool, HObject &scrImage,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	try{
		Tool.uplines.clear();
		Tool.downlines.clear();
		Tool.outlines.clear();
		Tool.strReslut = L"选定矩形区域--点击右键确定";
		//图像预处理
		dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
		Tool.ShowImage(dstImage);

		HObject  ho_Rectangle_out, ho_Rectangle_in, ho_Difference_rectangle, ho_ImageReduced,
			ho_Edges, ho_UnionContours, ho_UnionContours1,
			ho_SelectedContours, ho_SelectedContours1,
			ho_SortedContours, ho_edge_i, ho_ObjectSelected, ho_Rectangle_obj,
			ho_Circle, ho_SelectedXLD;
		HTuple hv_Number_edges, hv_Length_max_object, hv_Length_min_object, 
			hv_Index_max_obj, hv_Index_min_obj,
			hv_Index_in_obj, hv_i, hv_Row2, 
			hv_Column2, hv_Radius_i, hv_StartPhi2, hv_EndPhi2, hv_PointOrder2, 
			hv_Row_rec_obj, hv_Column_rec_obj, hv_Phi_rec_obj, hv_Length1_rec_obj, hv_Length2_rec_obj, hv_PointOrder,
			hv_Row_rec_fit, hv_Column_rec_fit, hv_Phi_rec_fit, hv_Length1_rec_fit, hv_Length2_rec_fit, hv_PointOrder3;


		//************************选定目标区域1********************
		DrawRectangle2(Tool.hv_WindowHandle, &Parameter.rectsParameter.hv_Row_rectangle_out, &Parameter.rectsParameter.hv_Column_rectangle_out, 
			&Parameter.rectsParameter.hv_Phi_rectangle_out, &Parameter.rectsParameter.hv_Length1_rectangle_out, &Parameter.rectsParameter.hv_Length2_rectangle_out);
		GenRectangle2(&ho_Rectangle_out, Parameter.rectsParameter.hv_Row_rectangle_out, Parameter.rectsParameter.hv_Column_rectangle_out, 
			Parameter.rectsParameter.hv_Phi_rectangle_out, Parameter.rectsParameter.hv_Length1_rectangle_out, Parameter.rectsParameter.hv_Length2_rectangle_out);
		Tool.outlines.push_back(ho_Rectangle_out);
		Tool.ShowImage(dstImage);
		DrawRectangle2(Tool.hv_WindowHandle, &Parameter.rectsParameter.hv_Row_rectangle_in, &Parameter.rectsParameter.hv_Column_rectangle_in, 
			&Parameter.rectsParameter.hv_Phi_rectangle_in, &Parameter.rectsParameter.hv_Length1_rectangle_in, &Parameter.rectsParameter.hv_Length2_rectangle_in);
		GenRectangle2(&ho_Rectangle_in, Parameter.rectsParameter.hv_Row_rectangle_in, Parameter.rectsParameter.hv_Column_rectangle_in, Parameter.rectsParameter.hv_Phi_rectangle_in, 
			Parameter.rectsParameter.hv_Length1_rectangle_in, Parameter.rectsParameter.hv_Length2_rectangle_in);
		Tool.outlines.push_back(ho_Rectangle_in);
		Tool.ShowImage(dstImage);
		SymmDifference(ho_Rectangle_out, ho_Rectangle_in, &ho_Difference_rectangle);
		ReduceDomain(dstImage, ho_Difference_rectangle, &ho_ImageReduced);

		EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 20, 40);
		/////////////////////////20180528修改///////////////////////
		
		UnionCollinearContoursXld(ho_Edges, &ho_UnionContours, 10, 1, 2, 0.1, "attr_keep");
		UnionAdjacentContoursXld(ho_UnionContours, &ho_UnionContours1, 5, 1, "attr_keep");
		SelectContoursXld(ho_UnionContours1, &ho_SelectedContours, "contour_length", 
			50, 10000, 0, 0);
		SelectContoursXld(ho_SelectedContours, &ho_SelectedContours1, "closed", 0.5, 
			2, -0.5, 0.5);

		//************排序选择最长的线并画直线*****************
		//排序
		SortContoursXld(ho_SelectedContours1, &ho_SortedContours, "upper_left", "true", 
			"row");
		//计算目标数
		CountObj(ho_SortedContours, &hv_Number_edges);
		if (0 != (hv_Number_edges==0))
		{
			AfxMessageBox(L"矩形区域未提取到边缘");
		}
		else
		{
			//拟合半径参数
			hv_Length_max_object = 0;
			hv_Length_min_object = 1000;
			//目标轮廓索引
			hv_Index_max_obj = HTuple();
			hv_Index_min_obj = HTuple();
			//找到半径最大和最小的索引
			HTuple end_val147 = hv_Number_edges-1;
			HTuple step_val147 = 1;
			for (hv_i=0; hv_i.Continue(end_val147, step_val147); hv_i += step_val147)
			{
				SelectObj(ho_SortedContours, &ho_edge_i, hv_i+1);
				FitRectangle2ContourXld(ho_edge_i, "regression", -1, 0, 0, 3, 2, &hv_Row_rec_fit, 
					&hv_Column_rec_fit, &hv_Phi_rec_fit, &hv_Length1_rec_fit, &hv_Length2_rec_fit, 
					&hv_PointOrder3);
				if((Parameter.rectsParameter.hv_Length1_rectangle_out>hv_Length1_rec_fit )&& (hv_Length1_rec_fit>Parameter.rectsParameter.hv_Length1_rectangle_in)
					||(Parameter.rectsParameter.hv_Length1_rectangle_out<hv_Length1_rec_fit )&& (hv_Length1_rec_fit<Parameter.rectsParameter.hv_Length1_rectangle_in))
				{
					if (0 != (hv_Length1_rec_fit>hv_Length_max_object))
					{
						hv_Length_max_object = hv_Length1_rec_fit;
						hv_Index_max_obj = hv_i+1;
					}
					if (0 != (hv_Length1_rec_fit<hv_Length_min_object))
					{
						hv_Length_min_object = hv_Length1_rec_fit;
						hv_Index_min_obj = hv_i+1;
					}
				}
			}
			if (0 != (Parameter.rectsParameter.hv_Length1_rectangle_out>Parameter.rectsParameter.hv_Length1_rectangle_in))
			{
				SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_max_obj);
			}
			else
			{
				SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_min_obj);
			}
			//拟合矩形并计算参数
			FitRectangle2ContourXld(ho_ObjectSelected, "regression", -1, 0, 0, 3, 2, 
				&hv_Row_rec_obj, &hv_Column_rec_obj, &hv_Phi_rec_obj, &hv_Length1_rec_obj, 
				&hv_Length2_rec_obj, &hv_PointOrder);

			//画出矩形
			GenRectangle2(&ho_Rectangle_obj, hv_Row_rec_obj, hv_Column_rec_obj, hv_Phi_rec_obj, 
				hv_Length1_rec_obj, hv_Length2_rec_obj);
		}
		Tool.outlines.clear();
		Tool.strReslut = L"选定矩形区域--已确定";
		Tool.outlines.push_back(ho_Rectangle_obj);
		Tool.outlines.push_back(ho_Difference_rectangle);
		Tool.ShowImage(dstImage);
	}
	catch(...)
	{
		AfxMessageBox(L"选定矩形区域失败");
		return false;
	}
	return true;
}	
bool CalgorithmRegion::SetUpRectsRect(HalconTool &ToolUp, HObject &imageUp,
	ModelParameter &Parameter)
{
	return SetRectsRect(ToolUp, imageUp,
		UpSetting, Parameter, Parameter.model_Name);
}
bool CalgorithmRegion::SetDownRectsRect(HalconTool &ToolDown, HObject &imageDown,
	ModelParameter &Parameter)
{
	return SetRectsRect(ToolDown, imageDown,
		DownSetting, Parameter, Parameter.model_Name);
}

//选定双圆弧区域
bool CalgorithmRegion::SetCirclesRect(HalconTool &Tool, HObject &scrImage, int index,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	Tool.uplines.clear();
	Tool.downlines.clear();
	Tool.outlines.clear();

	//图像预处理
	dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
	
	try{
		switch(index)
		{
		case 1:
			{
				Tool.strReslut = L"选定圆弧1区域--点击右键确定";
				Tool.ShowImage(dstImage);

				HObject  ho_Circle_out, ho_Circle_in, ho_Difference_circle, ho_ImageReduced,
					ho_Edges, ho_UnionContours, ho_SelectedContours, ho_SortedContours, ho_edge_i, ho_ObjectSelected,
					ho_Circle, ho_SelectedXLD;
				HTuple hv_Number_edges, hv_radius_max_object, hv_radius_min_object, 
					hv_Index_max_obj, hv_Index_in_obj, hv_i, hv_Row2, 
					hv_Column2, hv_Radius_i, hv_StartPhi2, hv_EndPhi2, hv_PointOrder2, hv_Row_circle_center, 
					hv_Column_circle_center, hv_Radius_circle, hv_StartPhi, hv_EndPhi, hv_PointOrder;

				//************************选定目标区域1********************
				DrawCircle(Tool.hv_WindowHandle, &Parameter.circlesParameter.hv_Row_circle_out, &Parameter.circlesParameter.hv_Column_circle_out, &Parameter.circlesParameter.hv_Radius_circle_out);
				GenCircle(&ho_Circle_out, Parameter.circlesParameter.hv_Row_circle_out, Parameter.circlesParameter.hv_Column_circle_out,Parameter.circlesParameter.hv_Radius_circle_out);
				Tool.outlines.push_back(ho_Circle_out);
				Tool.ShowImage(dstImage);
				DrawCircle(Tool.hv_WindowHandle, &Parameter.circlesParameter.hv_Row_circle_in, &Parameter.circlesParameter.hv_Column_circle_in, &Parameter.circlesParameter.hv_Radius_circle_in);
				GenCircle(&ho_Circle_in, Parameter.circlesParameter.hv_Row_circle_in, Parameter.circlesParameter.hv_Column_circle_in, Parameter.circlesParameter.hv_Radius_circle_in);
				Tool.outlines.push_back(ho_Circle_in);
				Tool.ShowImage(dstImage);
				SymmDifference(ho_Circle_out, ho_Circle_in, &ho_Difference_circle);

				//******提取目标区域1内的轮廓*********
				ReduceDomain(dstImage, ho_Difference_circle, &ho_ImageReduced);
				EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 15, 15);
	/*			UnionCocircularContoursXld(ho_Edges, &ho_UnionContours, HTuple(10).TupleRad(), 
				HTuple(3).TupleRad(), HTuple(10).TupleRad(), 50, 10, 10, "true", 1);*/
				UnionAdjacentContoursXld(ho_Edges,&ho_UnionContours,20,1,"attr_keep");	
				SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 50, 100000, 0, 0);
				//SegmentContoursXld (ho_SelectedContours,  &ho_SelectedContours, "lines_circles", 5, 4, 2);
				//SelectShapeXld(ho_SelectedContours, &ho_SelectedXLD, "circularity", "and", 0.05, 1);
				//************排序选择最长的线并画直线*****************
				//排序
				SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
					"row");
				//求面积
				//area_center_points_xld (SortedContours, Area_SortedContours, Row_SortedContours, Column_SortedContours)
				//计算目标数
				CountObj(ho_SortedContours, &hv_Number_edges);
				if (0 != (hv_Number_edges==0))
				{
					AfxMessageBox(L"圆弧1区域未提取到边缘");
				}
				else
				{
					//拟合半径参数
					hv_radius_max_object = 0;
					hv_radius_min_object = 1000;
					//目标轮廓索引
					hv_Index_max_obj = HTuple();
					hv_Index_in_obj = HTuple();
					//找到半径最大和最小的索引
					HTuple end_val159 = hv_Number_edges-1;
					HTuple step_val159 = 1;
					for (hv_i=0; hv_i.Continue(end_val159, step_val159); hv_i += step_val159)
					{
						SelectObj(ho_SortedContours, &ho_edge_i, hv_i+1);
						FitCircleContourXld(ho_edge_i, "algebraic", -1, 0, 0, 3, 2, &hv_Row2, 
							&hv_Column2, &hv_Radius_i, &hv_StartPhi2, &hv_EndPhi2, &hv_PointOrder2);
						if(((Parameter.circlesParameter.hv_Radius_circle_out>hv_Radius_i)&&(hv_Radius_i>Parameter.circlesParameter.hv_Radius_circle_in))
							||((Parameter.circlesParameter.hv_Radius_circle_out<hv_Radius_i)&&(hv_Radius_i<Parameter.circlesParameter.hv_Radius_circle_in)))
						{
							if (0 != (hv_Radius_i>hv_radius_max_object))
							{
								hv_radius_max_object = hv_Radius_i;
								hv_Index_max_obj = hv_i+1;
							}
							if (0 != (hv_Radius_i<hv_radius_min_object))
							{
								hv_radius_min_object = hv_Radius_i;
								hv_Index_in_obj = hv_i+1;
							}
						}						
					}
					if (0 != (Parameter.circlesParameter.hv_Radius_circle_out>Parameter.circlesParameter.hv_Radius_circle_in))
					{
						SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_max_obj);
					}
					else
					{
						SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_in_obj);
					}
					//拟合圆并计算参数
					FitCircleContourXld(ho_ObjectSelected, "atukey", -1, 0, 0, 3, 2, &hv_Row_circle_center, 
						&hv_Column_circle_center, &hv_Radius_circle, &hv_StartPhi, &hv_EndPhi, 
						&hv_PointOrder);

					//画出圆
					GenCircle(&ho_Circle, hv_Row_circle_center, hv_Column_circle_center, hv_Radius_circle);
				}
				Tool.outlines.clear();
				Tool.strReslut = L"选定圆弧1区域--已确定";
				Tool.outlines.push_back(ho_Circle);
				Tool.outlines.push_back(ho_Difference_circle);
				Tool.ShowImage(dstImage);
			}
			break;
		case 2:
			{
				Tool.strReslut = L"选定圆弧2区域--点击右键确定";
				Tool.ShowImage(dstImage);

				HObject  ho_Circle_out1, ho_Circle_in1, ho_Difference_circle1, ho_ImageReduced1,
					ho_Edges1, ho_UnionContours1, ho_SelectedContours1, ho_SortedContours1, ho_edge_j, ho_ObjectSelected1,
					ho_Circle1, ho_SelectedXLD1;
				HTuple hv_Number_edges1, hv_radius_max_object1, hv_radius_min_object1, 
					hv_Index_max_obj1, hv_Index_in_obj1, hv_j, hv_Row2, 
					hv_Column2, hv_Radius_j, hv_StartPhi2, hv_EndPhi2, hv_PointOrder2, hv_Row_circle_center1, 
					hv_Column_circle_center1, hv_Radius_circle1, hv_StartPhi1, hv_EndPhi1, hv_PointOrder1;

				//************************选定目标区域2********************
				DrawCircle(Tool.hv_WindowHandle, &Parameter.circlesParameter.hv_Row_circle_out1, &Parameter.circlesParameter.hv_Column_circle_out1, &Parameter.circlesParameter.hv_Radius_circle_out1);
				GenCircle(&ho_Circle_out1, Parameter.circlesParameter.hv_Row_circle_out1, Parameter.circlesParameter.hv_Column_circle_out1, Parameter.circlesParameter.hv_Radius_circle_out1);
				Tool.outlines.push_back(ho_Circle_out1);
				Tool.ShowImage(dstImage);
				DrawCircle(Tool.hv_WindowHandle, &Parameter.circlesParameter.hv_Row_circle_in1, &Parameter.circlesParameter.hv_Column_circle_in1, &Parameter.circlesParameter.hv_Radius_circle_in1);
				GenCircle(&ho_Circle_in1, Parameter.circlesParameter.hv_Row_circle_in1, Parameter.circlesParameter.hv_Column_circle_in1, Parameter.circlesParameter.hv_Radius_circle_in1);
				Tool.outlines.push_back(ho_Circle_in1);
				Tool.ShowImage(dstImage);
				SymmDifference(ho_Circle_out1, ho_Circle_in1, &ho_Difference_circle1);

				//******提取目标区域2内的轮廓*********
				ReduceDomain(dstImage, ho_Difference_circle1, &ho_ImageReduced1);
				EdgesSubPix(ho_ImageReduced1, &ho_Edges1, "canny", 1, 15, 15);
				//UnionCocircularContoursXld(ho_Edges1, &ho_UnionContours1, HTuple(10).TupleRad(), 
				//	HTuple(3).TupleRad(), HTuple(10).TupleRad(), 50, 10, 10, "true", 1);
				UnionAdjacentContoursXld(ho_Edges1,&ho_UnionContours1,20,1,"attr_keep");
				SelectContoursXld(ho_UnionContours1, &ho_SelectedContours1, "contour_length", 
					50, 100000, 0, 0);
				//SegmentContoursXld (ho_SelectedContours1,  &ho_SelectedContours1, "lines_circles", 5, 4, 2);
				//SelectShapeXld(ho_SelectedContours1, &ho_SelectedXLD1, "circularity", "and", 0.05, 1);
				//************排序选择最长的线*****************
				//排序
				SortContoursXld(ho_SelectedContours1, &ho_SortedContours1, "upper_left", 
					"true", "row");

				//计算目标数
				CountObj(ho_SortedContours1, &hv_Number_edges1);
				if (0 != (hv_Number_edges1==0))
				{
					AfxMessageBox(L"圆弧2区域未提取到边缘");
				}
				else
				{
					//拟合半径参数
					hv_radius_max_object1 = 0;
					hv_radius_min_object1 = 1000;
					//目标轮廓索引
					hv_Index_max_obj1 = HTuple();
					hv_Index_in_obj1 = HTuple();
					//找到半径最大和最小的索引
					HTuple end_val234 = hv_Number_edges1-1;
					HTuple step_val234 = 1;
					for (hv_j=0; hv_j.Continue(end_val234, step_val234); hv_j += step_val234)
					{
						SelectObj(ho_SortedContours1, &ho_edge_j, hv_j+1);
						FitCircleContourXld(ho_edge_j, "algebraic", -1, 0, 0, 3, 2, &hv_Row2, 
							&hv_Column2, &hv_Radius_j, &hv_StartPhi2, &hv_EndPhi2, &hv_PointOrder2);
						if(((Parameter.circlesParameter.hv_Radius_circle_out>hv_Radius_j)&&(hv_Radius_j>Parameter.circlesParameter.hv_Radius_circle_in))
							||((Parameter.circlesParameter.hv_Radius_circle_out<hv_Radius_j)&&(hv_Radius_j<Parameter.circlesParameter.hv_Radius_circle_in)))
						{
							if (0 != (hv_Radius_j>hv_radius_max_object1))
							{
								hv_radius_max_object1 = hv_Radius_j;
								hv_Index_max_obj1 = hv_j+1;
							}
							if (0 != (hv_Radius_j<hv_radius_min_object1))
							{
								hv_radius_min_object1 = hv_Radius_j;
								hv_Index_in_obj1 = hv_j+1;
							}
						}		
					}
					if (0 != (Parameter.circlesParameter.hv_Radius_circle_out1>Parameter.circlesParameter.hv_Radius_circle_in1))
					{
						SelectObj(ho_SortedContours1, &ho_ObjectSelected1, hv_Index_max_obj1);
					}
					else
					{
						SelectObj(ho_SortedContours1, &ho_ObjectSelected1, hv_Index_in_obj1);
					}

					//计算目标参数
					FitCircleContourXld(ho_ObjectSelected1, "atukey", -1, 0, 0, 3, 2, &hv_Row_circle_center1, 
						&hv_Column_circle_center1, &hv_Radius_circle1, &hv_StartPhi1, &hv_EndPhi1, 
						&hv_PointOrder1);
					//根据参数画圆
					GenCircle(&ho_Circle1, hv_Row_circle_center1, hv_Column_circle_center1, 
						hv_Radius_circle1);
				}

				Tool.outlines.clear();
				Tool.strReslut = L"选定圆弧2区域--已确定";
				Tool.outlines.push_back(ho_Circle1);
				Tool.outlines.push_back(ho_Difference_circle1);
				Tool.ShowImage(dstImage);			
			}
			break;
		}

	}
	catch(...)
	{
		AfxMessageBox(L"选定圆弧区域失败");
		return false;
	}
	return true;
}
bool CalgorithmRegion::SetUpCirclesRect(HalconTool &ToolUp, HObject &imageUp, int index,
	ModelParameter &Parameter)
{
	return SetCirclesRect(ToolUp, imageUp, index,
		UpSetting, Parameter, Parameter.model_Name);
}
bool CalgorithmRegion::SetDownCirclesRect(HalconTool &ToolDown, HObject &imageDown, int index,
	ModelParameter &Parameter)
{
	return SetCirclesRect(ToolDown, imageDown, index,
		DownSetting, Parameter, Parameter.model_Name);
}

//选定双直线区域
bool CalgorithmRegion::SetLinesRect(HalconTool &Tool, HObject &scrImage, int index,
			CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	Tool.uplines.clear();
	Tool.downlines.clear();
	Tool.outlines.clear();

	//图像预处理
	dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
	try{
		switch(index)
		{
		case 1:
			{
				Tool.strReslut = L"选定直线1区域--点击右键确定";
				Tool.ShowImage(dstImage);

				HObject  ho_Rectangle_obj, ho_ImageReduced, ho_Edges, ho_SortedContours, ho_ObjectSelected, ho_Regions_line;
				HTuple hv_RowTrans, hv_ColTrans, hv_Area_SortedContours, hv_Row_SortedContours, hv_Column_SortedContours, 
					hv_Number_edges, hv_area_object, hv_Index_obj, 
					hv_RowBegin, hv_ColBegin, hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc, hv_Dist, hv_angle_line, hv_i;

				//************************选定目标区域1********************
				DrawRectangle2(Tool.hv_WindowHandle, &Parameter.linesParameter.hv_Row_rec, &Parameter.linesParameter.hv_Column_rec, &Parameter.linesParameter.hv_Phi_rec, &Parameter.linesParameter.hv_Length1_rec, 
					&Parameter.linesParameter.hv_Length2_rec);
				GenRectangle2(&ho_Rectangle_obj, Parameter.linesParameter.hv_Row_rec, Parameter.linesParameter.hv_Column_rec, Parameter.linesParameter.hv_Phi_rec, Parameter.linesParameter.hv_Length1_rec, 
					Parameter.linesParameter.hv_Length2_rec);

				//**************************************找线 1处理过程*********************************************************			
				//******提取目标区域1内的轮廓*********
				ReduceDomain(dstImage, ho_Rectangle_obj, &ho_ImageReduced);
				EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 20, 40);
				UnionAdjacentContoursXld(ho_Edges,&ho_Edges,15,1,"attr_keep");
				////************方向排序选择的线并画直线*****************
				HObject ho_SelectedContours;
				//选择长度大于20的边缘
				SelectContoursXld(ho_Edges, &ho_SelectedContours, "contour_length", 20, 10000, 
					0, 0);
				//选择头尾相差超过20的轮廓
				SelectContoursXld(ho_SelectedContours, &ho_SelectedContours, "open", 20, 10000, 
					0, 0);
				//*矩形框角度在-45到45之间
				if (0 != ((Parameter.linesParameter.hv_Phi_rec.TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
						"column");
					//求面积
				}
				//*矩形框角度在45到135之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec-(HTuple(90).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "false", 
						"row");
				}
				//*矩形框角度135到225之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec-(HTuple(180).TupleRad())).TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "false", 
						"column");
				}
				//*矩形框角度在225到315（-45）之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec-(HTuple(270).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
						"row");
				}

				CountObj(ho_SortedContours, &hv_Number_edges);
				if (0 != (hv_Number_edges==0))
				{
					AfxMessageBox(L"直线1区域未提取到边缘");
				}
				//选择序列中的第一个轮廓
				SelectObj(ho_SortedContours, &ho_ObjectSelected, 1);
				////************方向排序选择的线并画直线*****************

				//拟合直线并计算参数
				FitLineContourXld(ho_ObjectSelected, "drop", -1, 10, 5, 1, &hv_RowBegin, &hv_ColBegin, 
					&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);
				if (0 != (hv_Nc>0))
				{
					hv_angle_line = (hv_Nr/hv_Nc).TupleAtan();
				}
				else if (0 != (hv_Nc==0))
				{
					hv_angle_line = hv_Nr*(HTuple(90).TupleRad());
				}
				else
				{
					hv_angle_line = ((hv_Nr/hv_Nc).TupleAtan())+(HTuple(180).TupleRad());
				}
				//画出直线1
				GenRegionHline(&ho_Regions_line, hv_angle_line, hv_Dist);

				Tool.strReslut = L"选定直线1区域--已确定";
				Tool.outlines.push_back(ho_Regions_line);
				Tool.outlines.push_back(ho_Rectangle_obj);
				Tool.ShowImage(dstImage);			
			}
			break;
		case 2:
			{
				HObject  ho_Rectangle_obj1, ho_ImageReduced1, ho_Edges1, ho_SortedContours1, ho_ObjectSelected1, ho_Regions_line1;
				HTuple hv_RowTrans1, hv_ColTrans1, hv_Area_SortedContours1, hv_Row_SortedContours1, hv_Column_SortedContours1, 
					hv_Number_edges1, hv_area_object1, hv_Index_obj1, 
					hv_RowBegin1, hv_ColBegin1, hv_RowEnd1, hv_ColEnd1, 
					hv_Nr1, hv_Nc1, hv_Dist1, hv_angle_line1, hv_j, hv_Area_SortedContours;

				Tool.strReslut = L"选定直线2区域--点击右键确定";
				Tool.ShowImage(dstImage);

				//************************选定目标区域2********************
				DrawRectangle2(Tool.hv_WindowHandle, &Parameter.linesParameter.hv_Row_rec1, &Parameter.linesParameter.hv_Column_rec1, &Parameter.linesParameter.hv_Phi_rec1, &Parameter.linesParameter.hv_Length1_rec1, 
					&Parameter.linesParameter.hv_Length2_rec1);
				GenRectangle2(&ho_Rectangle_obj1, Parameter.linesParameter.hv_Row_rec1, Parameter.linesParameter.hv_Column_rec1, Parameter.linesParameter.hv_Phi_rec1, Parameter.linesParameter.hv_Length1_rec1, 
					Parameter.linesParameter.hv_Length2_rec1);

				//**************************************找线2处理过程************************************************************
				//***************提取目标区域2内的轮廓****************
				ReduceDomain(dstImage, ho_Rectangle_obj1, &ho_ImageReduced1);
				EdgesSubPix(ho_ImageReduced1, &ho_Edges1, "canny", 1, 20, 40);
				UnionAdjacentContoursXld(ho_Edges1,&ho_Edges1,15,1,"attr_keep");
				////************方向排序选择的线并画直线*****************
				HObject ho_SelectedContours;
				//选择长度大于20的边缘
				SelectContoursXld(ho_Edges1, &ho_SelectedContours, "contour_length", 20, 10000, 
					0, 0);
				//选择头尾相差超过20的轮廓
				SelectContoursXld(ho_SelectedContours, &ho_SelectedContours, "open", 20, 10000, 
					0, 0);
				//*矩形框角度在-45到45之间
				if (0 != ((Parameter.linesParameter.hv_Phi_rec1.TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours1, "upper_left", "true", 
						"column");
					//求面积
				}
				//*矩形框角度在45到135之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec1-(HTuple(90).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours1, "upper_left", "false", 
						"row");
				}
				//*矩形框角度135到225之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec1-(HTuple(180).TupleRad())).TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours1, "upper_left", "false", 
						"column");
				}
				//*矩形框角度在225到315（-45）之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec1-(HTuple(270).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours1, "upper_left", "true", 
						"row");
				}

				CountObj(ho_SortedContours1, &hv_Number_edges1);
				if (0 != (hv_Number_edges1==0))
				{
					AfxMessageBox(L"直线2区域未提取到边缘");
				}
				//选择序列中的第一个轮廓
				SelectObj(ho_SortedContours1, &ho_ObjectSelected1, 1);
				////************方向排序选择的线并画直线*****************

				//计算目标参数
				FitLineContourXld(ho_ObjectSelected1, "drop", -1, 10, 5, 1, &hv_RowBegin1, 
					&hv_ColBegin1, &hv_RowEnd1, &hv_ColEnd1, &hv_Nr1, &hv_Nc1, &hv_Dist1);
				if (0 != (hv_Nc1>0))
				{
					hv_angle_line1 = (hv_Nr1/hv_Nc1).TupleAtan();
				}
				else if (0 != (hv_Nc1==0))
				{
					hv_angle_line1 = hv_Nr1*(HTuple(90).TupleRad());
				}
				else
				{
					hv_angle_line1 = ((hv_Nr1/hv_Nc1).TupleAtan())+(HTuple(180).TupleRad());
				}
				//画出直线2
				GenRegionHline(&ho_Regions_line1, hv_angle_line1, hv_Dist1);
				//*************************************线2处理结束*******************************************************

				Tool.strReslut = L"选定直线2区域--已确定";
				Tool.outlines.push_back(ho_Regions_line1);
				Tool.outlines.push_back(ho_Rectangle_obj1);
				Tool.ShowImage(dstImage);
			}
			break;
		}

	}
	catch(...)
	{
		AfxMessageBox(L"选定双直线区域失败");
		return false;
	}
	return true;
}
bool CalgorithmRegion::SetUpLinesRect(HalconTool &ToolUp, HObject &imageUp, int index,
	ModelParameter &Parameter)
{
	return SetLinesRect(ToolUp, imageUp, index,
		UpSetting, Parameter, Parameter.model_Name);
}
bool CalgorithmRegion::SetDownLinesRect(HalconTool &ToolDown, HObject &imageDown, int index,
	ModelParameter &Parameter)
{
	return SetLinesRect(ToolDown, imageDown, index,
		DownSetting, Parameter, Parameter.model_Name);
}

//图像预处理
HObject CalgorithmRegion::SetPretreatImage(HalconTool &Tool, HObject &scrImage,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	//增强原图像对比度
	dstImage = SetMultImage(Tool, scrImage, Parameter.hv_Mult, Parameter.hv_Add, Parameter.is_Mult,
		Setting, Parameter, modelName);
	//中值滤波器
	dstImage = SetMedianImage(Tool, dstImage, Parameter.hv_median_bool, Parameter.hv_Parameter_median,
		Setting, Parameter, modelName);
	return dstImage;
}

//中值滤波器
HObject CalgorithmRegion::SetMedianImage(HalconTool &Tool, HObject &scrImage, int hv_median_bool, int hv_Parameter_median,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	Parameter.hv_median_bool = hv_median_bool;
	Parameter.hv_Parameter_median = hv_Parameter_median;
	try{
		if(Parameter.hv_median_bool)
		{
			MedianImage(scrImage, &dstImage, "square", hv_Parameter_median, "cyclic");
		}	
		else
		{
			dstImage = scrImage;
		}
		//Tool.ShowImage(dstImage);
	}catch(...)
	{
		AfxMessageBox(L"图像中值滤波失败！");
	}
	return dstImage;
}

//增强原图像对比度
HObject CalgorithmRegion::SetMultImage(HalconTool &Tool, HObject &scrImage, HTuple hv_Mult, HTuple hv_Add, bool is_Mult, 
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	Parameter.hv_Mult = hv_Mult;
	Parameter.hv_Add = hv_Add;
	Parameter.is_Mult = is_Mult;
	try{
		if(is_Mult)
		{
			MultImage(scrImage, scrImage, &dstImage, hv_Mult, hv_Add);
		}	
		else
		{
			dstImage = scrImage;
		}
		//Tool.ShowImage(dstImage);
	}catch(...)
	{
		AfxMessageBox(L"图像增强失败！");
	}
	return dstImage;
}

//选择模板
bool CalgorithmRegion::BuildModel(HalconTool &Tool, HObject &scrImage,
	int UseRect2,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject ho_Region_xld;//模板轮廓
	HObject ho_model, dstImage;//增强图像对比度
	HObject ho_Rectangle;//模板矩形
	HObject ho_ModelContours, ho_ContoursAffinTrans, ho_SortedContours;//模板轮廓
	HObject ho_ObjectSelected, ho_Region_selected;
	HObject ho_Region_gravity, ho_RegionLines, ho_RegionLines1;//重心
	HTuple	hv_Row1_rec, hv_Column1_rec, hv_Row2_rec, hv_Column2_rec, hv_Angle_rec;
	HTuple  hv_Score;//模板数据
	HTuple  hv_HomMat2D;//模板->模板矩形投影
	HTuple	hv_Number_xld, hv_i, hv_Row_obj, hv_Col_obj, hv_Area1;

	try{
		Tool.outlines.clear();
		Tool.strReslut = L"建立模板--点击右键确定";
		//图像预处理
		dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
		Tool.ShowImage(dstImage);

		double aa = (double)Parameter.hv_Row_center_rec_model;
		double cc = (double)Parameter.hv_Column_center_rec_model;

		if(UseRect2)
		{
			//建立模板矩形2区域
			DrawRectangle2(Tool.hv_WindowHandle, &Parameter.hv_Row_center_rec_model, &Parameter.hv_Column_center_rec_model, 
				&Parameter.hv_Angle, 
				&Parameter.hv_Length1_rec_model, &Parameter.hv_Length2_rec_model);
		}
		else
		{
			//建立模板矩形1区域
			DrawRectangle1(Tool.hv_WindowHandle, &hv_Row1_rec, &hv_Column1_rec, &hv_Row2_rec, &hv_Column2_rec);
			Parameter.hv_Row_center_rec_model = (hv_Row1_rec+hv_Row2_rec)/2;
			Parameter.hv_Column_center_rec_model = (hv_Column1_rec+hv_Column2_rec)/2;
			Parameter.hv_Angle = 0;
			Parameter.hv_Length1_rec_model = ((hv_Column1_rec-hv_Column2_rec).TupleAbs())/2;
			Parameter.hv_Length2_rec_model = ((hv_Row1_rec-hv_Row2_rec).TupleAbs())/2;
		}

		GenRectangle2(&ho_Rectangle, Parameter.hv_Row_center_rec_model, Parameter.hv_Column_center_rec_model, 
			Parameter.hv_Angle, Parameter.hv_Length1_rec_model, Parameter.hv_Length2_rec_model);

		ReduceDomain(dstImage, ho_Rectangle, &ho_model);
		CreateShapeModel(ho_model, "auto", HTuple(Parameter.hv_AStart).TupleRad(), HTuple(Parameter.hv_ARange).TupleRad(), 
			"auto", "auto", "use_polarity",
			"auto", "auto", &Parameter.hv_ModelID);
		FindShapeModel(dstImage, Parameter.hv_ModelID, HTuple(Parameter.hv_AStart).TupleRad(), HTuple(Parameter.hv_ARange).TupleRad(), 
			0.1, 1, 0, "least_squares", 0, 0.1, &Parameter.hv_Row, &Parameter.hv_Column, &hv_Angle_rec, &hv_Score);

		VectorAngleToRigid(0, 0, 0, Parameter.hv_Row, Parameter.hv_Column, hv_Angle_rec, &hv_HomMat2D);
		GetShapeModelContours(&ho_ModelContours, Parameter.hv_ModelID, 1);
		AffineTransContourXld(ho_ModelContours, &ho_ContoursAffinTrans, hv_HomMat2D);
		GenEmptyObj(&ho_Region_xld);
		SortContoursXld(ho_ContoursAffinTrans, &ho_SortedContours, "upper_left", "true", "row");
		CountObj(ho_SortedContours, &hv_Number_xld);
		HTuple end_val49 = hv_Number_xld-1;
		HTuple step_val49 = 1;
		for (hv_i=0; hv_i.Continue(end_val49, step_val49); hv_i += step_val49)
		{
			SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_i+1);
			GetContourXld(ho_ObjectSelected, &hv_Row_obj, &hv_Col_obj);
			GenRegionPoints(&ho_Region_selected, hv_Row_obj, hv_Col_obj);
			Union2(ho_Region_xld, ho_Region_selected, &ho_Region_xld);
		}
		//求模板轮廓的重心
		AreaCenter(ho_Region_xld, &hv_Area1, &Parameter.hv_Row_gravity_model, &Parameter.hv_Column_gravity_model);
		//在重心画十字线
		GenRegionLine(&ho_RegionLines, Parameter.hv_Row_gravity_model+8, Parameter.hv_Column_gravity_model, 
			Parameter.hv_Row_gravity_model-8, Parameter.hv_Column_gravity_model);
		GenRegionLine(&ho_RegionLines1, Parameter.hv_Row_gravity_model, Parameter.hv_Column_gravity_model-8, 
			Parameter.hv_Row_gravity_model, Parameter.hv_Column_gravity_model+8);
		Union2(ho_RegionLines, ho_RegionLines1, &ho_Region_gravity);

		Tool.strReslut = L"建立模板--已确定";
		Tool.outlines.clear();
		Tool.outlines.push_back(ho_Region_xld);
		Tool.uplines.clear();
		Tool.uplines.push_back(ho_Region_gravity);
		Tool.ShowImage(dstImage);
	}
	catch(...)
	{
		AfxMessageBox(L"建立模板失败,请选择平滑轮廓作为模板");
		return false;
	}
	return true;
}
bool CalgorithmRegion::BuildUpModel(HalconTool &ToolUp, HObject &imageUp,
	int UseRect2,
	ModelParameter &Parameter)
{	
	return BuildModel(ToolUp, imageUp,
		UseRect2, UpSetting, Parameter, Parameter.model_Name);
}
bool CalgorithmRegion::BuildDownModel(HalconTool &ToolDown, HObject &imageDown,
	int UseRect2,
	ModelParameter &Parameter)
{
	return BuildModel(ToolDown, imageDown,
		UseRect2, DownSetting, Parameter, Parameter.model_Name);
}

//贴片、底板搜索区域
bool CalgorithmRegion::SetRect(HalconTool &Tool, HObject &scrImage, CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject Rect, dstImage;
	HTuple hv_Width_model, hv_Height_model;

	Tool.outlines.clear();
	Tool.strReslut = L"搜索范围--点击右键确定";
	//图像预处理
	dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
	Tool.ShowImage(dstImage);

	GetImageSize(dstImage, &hv_Width_model, &hv_Height_model);
	DrawRectangle1(Tool.hv_WindowHandle, &Parameter.hv_Row1, &Parameter.hv_Column1, &Parameter.hv_Row2, &Parameter.hv_Column2);
	if((double)Parameter.hv_Row1 == (double)Parameter.hv_Row2 || (double)Parameter.hv_Column1 == (double)Parameter.hv_Column2)
	{
		Parameter.hv_Row1 = 1;
		Parameter.hv_Row2 = hv_Height_model;
		Parameter.hv_Column1 =1;
		Parameter.hv_Column2 = hv_Width_model;
	}
	GenRectangle1(&Rect, Parameter.hv_Row1, Parameter.hv_Column1, Parameter.hv_Row2, Parameter.hv_Column2);

	Tool.outlines.clear();
	Tool.strReslut = L"搜索范围--已确定";
	Tool.outlines.push_back(Rect);
	Tool.ShowImage(dstImage);

	return true;
}
bool CalgorithmRegion::SetUpRect(HalconTool &ToolUp, HObject &imageUp, ModelParameter &Parameter)
{
	return SetRect(ToolUp, imageUp, UpSetting, Parameter, Parameter.model_Name);
}
bool CalgorithmRegion::SetDownRect(HalconTool &ToolDown, HObject &imageDown, ModelParameter &Parameter)
{
	return SetRect(ToolDown, imageDown, DownSetting, Parameter, Parameter.model_Name);
}

//贴片、底板识别
bool CalgorithmRegion::FindModel(HalconTool &Tool, HObject &scrImage, vector<double> &results,
	CString Setting, ModelParameter &Parameter, bool isSaveImage)
{
	bool ret = false;
	HObject modelCross;
	HObject Rect, imageRect, dstImage;
	HObject ho_ModelContours, ho_ContoursAffinTrans;

	HTuple  hv_Seconds1, hv_Seconds2;
	HTuple  hv_Row, hv_Column, hv_Angle, hv_Score;
	HTuple  hv_Width, hv_Height, hv_HomMat2D, hv_i, hv_j;

	HTuple	hv_X_Trans, hv_Y_Trans;
	try{
		double hv_AhcX, hv_AhcY;
		Pix2CenterMM(hv_AhcX, hv_AhcY, Parameter.model_Name);
		Parameter.hv_Ahc = (hv_AhcX+hv_AhcY)/2;
		GetImageSize(scrImage, &hv_Width, &hv_Height);

		if(isSaveImage)
			SaveImage(Parameter.model_Name, scrImage);
		//原图像比率显示
		Tool.m_scale = 1;
		Tool.m_X = 0;
		Tool.m_Y = 0;
				
		results.clear();
		for(int i = 0; i<20; i++)
		{
			results.push_back(0);
		}
		Tool.outlines.clear();
		Tool.uplines.clear();
		Tool.downlines.clear();

		//图像预处理
		dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, Parameter.model_Name);
		GenRectangle1(&Rect, Parameter.hv_Row1, Parameter.hv_Column1, Parameter.hv_Row2, Parameter.hv_Column2);
		ReduceDomain(dstImage, Rect, &imageRect);


		CountSeconds(&hv_Seconds1);
		FindShapeModel(imageRect, Parameter.hv_ModelID, HTuple(Parameter.hv_AStart).TupleRad(), HTuple(Parameter.hv_ARange).TupleRad(), 
			0.1, 1, 0, "least_squares", 0, 0.1, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
		CountSeconds(&hv_Seconds2);

		//模板中心
		hv_X_Trans = (hv_Column-hv_Width/2);
		hv_Y_Trans = (hv_Height/2-hv_Row);

		if((double)hv_Score*100 >= Parameter.hv_Score)
		{
			//模板匹配
			if(L"" == Parameter.check)
			{
				//1.重新计算
				HObject ho_RegionLines, ho_RegionLines1,
					ho_ModelContours1, ho_ContoursAffinTrans1, ho_Region_xld1,
					ho_SortedContours1, ho_Region_selected1, ho_ObjectSelected1;
				HTuple hv_Row_gravity, hv_Column_gravity,
					hv_HomMat2D_model1, hv_Number_xld1, hv_Row_obj1, hv_Col_obj1, hv_Area;

				GetShapeModelContours(&ho_ModelContours1, Parameter.hv_ModelID, 1);
				VectorAngleToRigid(0, 0, 0, hv_Row, hv_Column, hv_Angle, &hv_HomMat2D_model1);
				AffineTransContourXld(ho_ModelContours1, &ho_ContoursAffinTrans1, hv_HomMat2D_model1);
				GenEmptyObj(&ho_Region_xld1);
				SortContoursXld(ho_ContoursAffinTrans1, &ho_SortedContours1, "upper_left", "true", 
					"row");
				CountObj(ho_SortedContours1, &hv_Number_xld1);
				HTuple end_val13 = hv_Number_xld1-1;
				HTuple step_val13 = 1;
				for (hv_j=0; hv_j.Continue(end_val13, step_val13); hv_j += step_val13)
				{
					SelectObj(ho_SortedContours1, &ho_ObjectSelected1, hv_j+1);
					GetContourXld(ho_ObjectSelected1, &hv_Row_obj1, &hv_Col_obj1);
					GenRegionPoints(&ho_Region_selected1, hv_Row_obj1, hv_Col_obj1);
					Union2(ho_Region_xld1, ho_Region_selected1, &ho_Region_xld1);
				}
				AreaCenter(ho_Region_xld1, &hv_Area, &hv_Row_gravity, &hv_Column_gravity);

				//在重心画十字线
				HObject ho_Cross;
				GenCrossContourXld(&ho_Cross, hv_Row_gravity, hv_Column_gravity, 8, 0);

				VectorAngleToRigid(0, 0, 0, -hv_Height/2, hv_Width/2, -(HTuple(90).TupleRad()), &hv_HomMat2D);
				AffineTransPixel(hv_HomMat2D, hv_Row_gravity, hv_Column_gravity, &hv_X_Trans, &hv_Y_Trans);
				//显示模板
				dev_display_shape_matchinresults(Tool.outlines, Parameter.hv_ModelID,
					hv_Row, 
					hv_Column, 
					hv_Angle, 
					1, 
					1, 
					0);
				//显示弧度
				hv_Angle = hv_Angle+Parameter.hv_Angle;
				Tool.outlines.push_back(Rect);
				Tool.outlines.push_back(ho_Cross);

				////2.映射计算
				//HObject ho_RegionLines, ho_RegionLines1, ho_Region_gravity;
				////HTuple hv_Row_gravity, hv_Column_gravity;

				////VectorAngleToRigid(Parameter.hv_Row, Parameter.hv_Column, Parameter.hv_Angle, 
				////	hv_Row, hv_Column,  Parameter.hv_Angle+hv_Angle, &hv_HomMat2D);
				//////重心
				////AffineTransPixel(hv_HomMat2D, Parameter.hv_Row_gravity_model, Parameter.hv_Column_gravity_model, 
				////	&hv_Row_gravity, &hv_Column_gravity);

				////计算重心与模板中心关系
				//// Local control variables 
				//HTuple  hv_Row_model, hv_Column_model, hv_Angle_model;
				//HTuple  hv_Row_gravity_model, hv_Column_gravity_model;
				//HTuple  hv_Row_Pow, hv_Column_Pow;
				//HTuple  hv_Distance_Center2Gravity, hv_Angle_Center2Gravity_model;
				//HTuple  hv_Angle_Center2Gravity, hv_Row_gravity, hv_Column_gravity;
				////************************************输入**********************************
				////*************模板中心参数
				//hv_Row_model = Parameter.hv_Row;
				//hv_Column_model = Parameter.hv_Column;
				//hv_Angle_model = Parameter.hv_Angle;
				////原图像中的模板重心坐标
				//hv_Row_gravity_model = Parameter.hv_Row_gravity_model;
				//hv_Column_gravity_model = Parameter.hv_Column_gravity_model;


				////*******************************计算*************************************
				//TuplePow(hv_Row_gravity_model-hv_Row_model, 2, &hv_Row_Pow);
				//TuplePow(hv_Column_gravity_model-hv_Column_model, 2, &hv_Column_Pow);
				//hv_Distance_Center2Gravity = (hv_Row_Pow+hv_Column_Pow).TupleSqrt();
				//AngleLx(hv_Row_model, hv_Column_model, hv_Row_gravity_model, hv_Column_gravity_model, 
				//	&hv_Angle_Center2Gravity_model);
				//hv_Angle_Center2Gravity = (hv_Angle_Center2Gravity_model-hv_Angle_model)+hv_Angle;
				//hv_Row_gravity = (hv_Row+(hv_Distance_Center2Gravity*((-hv_Angle_Center2Gravity).TupleSin())))-hv_Angle_model;
				//hv_Column_gravity = hv_Column+(hv_Distance_Center2Gravity*((-hv_Angle_Center2Gravity).TupleCos()));


				////在重心画十字线
				//GenRegionLine(&ho_RegionLines, hv_Row_gravity+8, hv_Column_gravity, 
				//	hv_Row_gravity-8, hv_Column_gravity);
				//GenRegionLine(&ho_RegionLines1, hv_Row_gravity, hv_Column_gravity-8, 
				//	hv_Row_gravity, hv_Column_gravity+8);
				//Union2(ho_RegionLines, ho_RegionLines1, &ho_Region_gravity);

				////VectorAngleToRigid(0, 0, 0, -hv_Height/2, hv_Width/2, -(HTuple(90).TupleRad()), &hv_HomMat2D);
				////AffineTransPixel(hv_HomMat2D, hv_Row_gravity, hv_Column_gravity, &hv_X_Trans, &hv_Y_Trans);
				////自计算
				//hv_X_Trans = (hv_Column_gravity-hv_Width/2);
				//hv_Y_Trans = (hv_Height/2-hv_Row_gravity);
				//
				////显示模板
				//dev_display_shape_matchinresults(Tool.outlines, hv_ModelID,
				//	hv_Row, 
				//	hv_Column, 
				//	hv_Angle, 
				//	1, 
				//	1, 
				//	0);
				////显示弧度
				//hv_Angle = hv_Angle+Parameter.hv_Angle;
				//Tool.outlines.push_back(Rect);
				//Tool.outlines.push_back(ho_Region_gravity);

				//模板中心与角度
				//results[0] = (double)hv_X_Trans*Parameter.hv_Ahc;
				//results[1] = (double)hv_Y_Trans*Parameter.hv_Ahc;
				results[0] = (double)hv_X_Trans*hv_AhcX;
				results[1] = (double)hv_Y_Trans*hv_AhcY;
				results[2] = (double)hv_Angle.TupleDeg();
				results[3] = (double)hv_Score*100;

				//中心坐标(像素)
				results[4] = (double)hv_X_Trans;
				results[5] = (double)hv_Y_Trans;
				results[6] = (double)hv_Angle.TupleDeg();

				//模板中心
				//hv_X_Trans = (hv_Column-hv_Width/2);
				//hv_Y_Trans = (hv_Height/2-hv_Row);
				//results[7] = (double)hv_X_Trans;
				//results[8] = (double)hv_Y_Trans;
			}
			//双直线匹配
			else if(L"Lines" == Parameter.check)
			{

				HObject  ho_Rectangle_obj0, ho_ImageReduced, ho_Edges, ho_SortedContours, ho_ObjectSelected, ho_Regions_line;

				HTuple hv_RowTrans, hv_ColTrans, hv_Area_SortedContours, hv_Row_SortedContours, hv_Column_SortedContours, 
					hv_Number_edges, hv_area_object, hv_Index_obj, 
					hv_RowBegin, hv_ColBegin, hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc, hv_Dist, hv_angle_line;

				//**************************************找线 1处理过程*********************************************************
				//**********搜索目标区域1*************
				VectorAngleToRigid(Parameter.hv_Row, Parameter.hv_Column, Parameter.hv_Angle, hv_Row, hv_Column, Parameter.hv_Angle+hv_Angle, &hv_HomMat2D);
				AffineTransPixel(hv_HomMat2D, Parameter.linesParameter.hv_Row_rec, Parameter.linesParameter.hv_Column_rec, &hv_RowTrans, &hv_ColTrans);
				GenRectangle2(&ho_Rectangle_obj0, hv_RowTrans, hv_ColTrans, Parameter.linesParameter.hv_Phi_rec+Parameter.hv_Angle, 
					Parameter.linesParameter.hv_Length1_rec, Parameter.linesParameter.hv_Length2_rec);				

				//******提取目标区域1内的轮廓*********
				ReduceDomain(dstImage, ho_Rectangle_obj0, &ho_ImageReduced);
				EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 20, 40);
				UnionAdjacentContoursXld(ho_Edges,&ho_Edges,15,1,"attr_keep");
				////************方向排序选择的线并画直线*****************
				HObject ho_SelectedContours;
				//选择长度大于20的边缘
				SelectContoursXld(ho_Edges, &ho_SelectedContours, "contour_length", 20, 10000, 
					0, 0);
				//选择头尾相差超过20的轮廓
				SelectContoursXld(ho_SelectedContours, &ho_SelectedContours, "open", 20, 10000, 
					0, 0);
				//*矩形框角度在-45到45之间
				if (0 != ((Parameter.linesParameter.hv_Phi_rec.TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
						"column");
					//求面积
				}
				//*矩形框角度在45到135之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec-(HTuple(90).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "false", 
						"row");
				}
				//*矩形框角度135到225之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec-(HTuple(180).TupleRad())).TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "false", 
						"column");
				}
				//*矩形框角度在225到315（-45）之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec-(HTuple(270).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
						"row");
				}

				CountObj(ho_SortedContours, &hv_Number_edges);
				if (0 != (hv_Number_edges==0))
				{
					//AfxMessageBox(L"区域1未提取到边缘");
					return false;
				}
				//选择序列中的第一个轮廓
				SelectObj(ho_SortedContours, &ho_ObjectSelected, 1);
				////************方向排序选择的线并画直线*****************

				//拟合直线并计算参数
				FitLineContourXld(ho_ObjectSelected, "drop", -1, 10, 5, 1, &hv_RowBegin, &hv_ColBegin, 
					&hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);
				if (0 != (hv_Nc>0))
				{
					hv_angle_line = (hv_Nr/hv_Nc).TupleAtan();
				}
				else if (0 != (hv_Nc==0))
				{
					hv_angle_line = hv_Nr*(HTuple(90).TupleRad());
				}
				else
				{
					hv_angle_line = ((hv_Nr/hv_Nc).TupleAtan())+(HTuple(180).TupleRad());
				}
				//画出直线1
				GenRegionHline(&ho_Regions_line, hv_angle_line, hv_Dist);
				//***************************************线1处理结束*******************************************************

				HObject  ho_Rectangle_obj1, ho_ImageReduced1, ho_Edges1, ho_SortedContours1, ho_ObjectSelected1, ho_Regions_line1;

				HTuple hv_RowTrans1, hv_ColTrans1, hv_Area_SortedContours1, hv_Row_SortedContours1, hv_Column_SortedContours1, 
					hv_Number_edges1, hv_area_object1, hv_Index_obj1, 
					hv_RowBegin1, hv_ColBegin1, hv_RowEnd1, hv_ColEnd1, hv_Nr1, hv_Nc1, hv_Dist1, hv_angle_line1;

				//**************************************找线2处理过程************************************************************
				//*********搜索目标区域2************
				AffineTransPixel(hv_HomMat2D, Parameter.linesParameter.hv_Row_rec1, Parameter.linesParameter.hv_Column_rec1, &hv_RowTrans1, &hv_ColTrans1);
				GenRectangle2(&ho_Rectangle_obj1, hv_RowTrans1, hv_ColTrans1, Parameter.linesParameter.hv_Phi_rec1+Parameter.hv_Angle, Parameter.linesParameter.hv_Length1_rec1, Parameter.linesParameter.hv_Length2_rec1);
				//***************提取目标区域2内的轮廓****************
				ReduceDomain(dstImage, ho_Rectangle_obj1, &ho_ImageReduced1);
				EdgesSubPix(ho_ImageReduced1, &ho_Edges1, "canny", 1, 20, 40);
				UnionAdjacentContoursXld(ho_Edges1,&ho_Edges1,15,1,"attr_keep");
				////************方向排序选择的线并画直线*****************
				//选择长度大于20的边缘
				SelectContoursXld(ho_Edges1, &ho_SelectedContours, "contour_length", 20, 10000, 
					0, 0);
				//选择头尾相差超过20的轮廓
				SelectContoursXld(ho_SelectedContours, &ho_SelectedContours, "open", 20, 10000, 
					0, 0);
				//*矩形框角度在-45到45之间
				if (0 != ((Parameter.linesParameter.hv_Phi_rec1.TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours1, "upper_left", "true", 
						"column");
					//求面积
				}
				//*矩形框角度在45到135之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec1-(HTuple(90).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours1, "upper_left", "false", 
						"row");
				}
				//*矩形框角度135到225之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec1-(HTuple(180).TupleRad())).TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours1, "upper_left", "false", 
						"column");
				}
				//*矩形框角度在225到315（-45）之间
				if (0 != (((Parameter.linesParameter.hv_Phi_rec1-(HTuple(270).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours1, "upper_left", "true", 
						"row");
				}

				CountObj(ho_SortedContours1, &hv_Number_edges1);
				if (0 != (hv_Number_edges1==0))
				{
					//AfxMessageBox(L"区域2未提取到边缘");
					return false;
				}
				//选择序列中的第一个轮廓
				SelectObj(ho_SortedContours1, &ho_ObjectSelected1, 1);
				////************方向排序选择的线并画直线*****************

				//计算目标参数
				FitLineContourXld(ho_ObjectSelected1, "drop", -1, 10, 5, 1, &hv_RowBegin1, 
					&hv_ColBegin1, &hv_RowEnd1, &hv_ColEnd1, &hv_Nr1, &hv_Nc1, &hv_Dist1);
				if (0 != (hv_Nc1>0))
				{
					hv_angle_line1 = (hv_Nr1/hv_Nc1).TupleAtan();
				}
				else if (0 != (hv_Nc1==0))
				{
					hv_angle_line1 = hv_Nr1*(HTuple(90).TupleRad());
				}
				else
				{
					hv_angle_line1 = ((hv_Nr1/hv_Nc1).TupleAtan())+(HTuple(180).TupleRad());
				}
				//画出直线2
				GenRegionHline(&ho_Regions_line1, hv_angle_line1, hv_Dist1);
				//*************************************线2处理结束*******************************************************

				HObject ho_RegionDilation_line, ho_RegionDilation_line1, ho_Circle, ho_ellipse, ho_ellipse1, ho_RegionIntersection_lines;

				HTuple hv_Area_corner, hv_Row_corner, hv_Column_corner, hv_Angle_line, hv_Angle_line1,
					hv_deAngle_line, hv_deAngle_line1, hv_x, hv_y;

				//直线方向
				DilationCircle(ho_Regions_line, &ho_RegionDilation_line, 1.5);
				DilationCircle(ho_Regions_line1, &ho_RegionDilation_line1, 1.5);

				Intersection(ho_RegionDilation_line1, ho_RegionDilation_line, &ho_RegionIntersection_lines);
				AreaCenter(ho_RegionIntersection_lines, &hv_Area_corner, &hv_Row_corner, &hv_Column_corner);
				AngleLx(hv_Row_corner, hv_Column_corner, (hv_RowBegin+hv_RowEnd)/2, (hv_ColBegin+hv_ColEnd)/2, &hv_Angle_line);
				AngleLx(hv_Row_corner, hv_Column_corner, (hv_RowBegin1+hv_RowEnd1)/2, (hv_ColBegin1+hv_ColEnd1)/2, &hv_Angle_line1);
				GenCircle(&ho_Circle, hv_Row_corner, hv_Column_corner, 5);
				GenEllipse(&ho_ellipse, (hv_RowBegin+hv_RowEnd)/2, (hv_ColBegin+hv_ColEnd)/2, hv_Angle_line, 10, 5);
				GenEllipse(&ho_ellipse1, (hv_RowBegin1+hv_RowEnd1)/2, (hv_ColBegin1+hv_ColEnd1)/2, hv_Angle_line1, 10, 5);
				hv_deAngle_line = hv_Angle_line.TupleDeg();
				hv_deAngle_line1 = hv_Angle_line1.TupleDeg();
				//VectorAngleToRigid(0, 0, 0, -hv_Height/2, hv_Width/2, HTuple(-90).TupleRad(), &hv_HomMat2D);
				//AffineTransPixel(hv_HomMat2D, hv_Row_corner, hv_Column_corner, &hv_x, &hv_y);


				//在重心画十字线
				HObject ho_Cross;
				GenCrossContourXld(&ho_Cross, hv_Row_corner, hv_Column_corner, 8, 0);

				//自计算
				hv_x = (hv_Column_corner-hv_Width/2);
				hv_y = (hv_Height/2-hv_Row_corner);

				//直线
				Union2(ho_Regions_line1, ho_Regions_line, &ho_Regions_line);
				Tool.outlines.push_back(ho_Regions_line);
				//搜索范围
				Tool.outlines.push_back(Rect);

				switch(Parameter.linesParameter.baseline)
				{
				case 0:
					{
						results[0] = (double)hv_x*hv_AhcX;
						results[1] = (double)hv_y*hv_AhcY;
						results[2] = (double)hv_deAngle_line;
						results[3] = (double)hv_Score*100;
						//直线1方向
						Tool.outlines.push_back(ho_Cross);
						Tool.outlines.push_back(ho_ellipse);

						//中心坐标(像素)
						results[4] = (double)hv_x;
						results[5] = (double)hv_y;
						results[6] = (double)hv_deAngle_line;
						results[7] = (double)hv_deAngle_line1;
					}
					break;
				case 1:
					{
						results[0] = (double)hv_x*hv_AhcX;
						results[1] = (double)hv_y*hv_AhcY;				
						results[2] = (double)hv_deAngle_line1;
						results[3] = (double)hv_Score*100;
						//直线2方向
						Tool.outlines.push_back(ho_Cross);
						Tool.outlines.push_back(ho_ellipse1);

						//中心坐标(像素)
						results[4] = (double)hv_x;
						results[5] = (double)hv_y;
						results[6] = (double)hv_deAngle_line1;
						results[7] = (double)hv_deAngle_line;
					}
					break;
				}	
			}
			//双圆弧匹配
			else if(L"Circles" == Parameter.check)
			{
				HObject  
					ho_Circle_out, ho_Circle_in, 
					ho_Difference_circle, ho_ImageReduced,
					ho_Edges, ho_UnionContours, ho_SelectedContours, ho_SortedContours, ho_edge_i, ho_ObjectSelected,
					ho_Circle, ho_SelectedXLD;
				HTuple 
					hv_Row_circle_out_tran, hv_Column_circle_out_tran, 
					hv_Row_circle_in_tran, hv_Column_circle_in_tran,
					hv_Number_edges, hv_radius_max_object, hv_radius_min_object, 
					hv_Index_max_obj, hv_Index_in_obj, hv_i, hv_Row2, 
					hv_Column2, hv_Radius_i, hv_StartPhi2, hv_EndPhi2, hv_PointOrder2, 
					hv_Row_circle_center, hv_Column_circle_center, 
					hv_Radius_circle, hv_StartPhi, hv_EndPhi, hv_PointOrder,
					hv_x, hv_y;

				//**********搜索目标区域1*************
				VectorAngleToRigid(Parameter.hv_Row, Parameter.hv_Column, Parameter.hv_Angle, 
					hv_Row, hv_Column, Parameter.hv_Angle+hv_Angle, &hv_HomMat2D);
				AffineTransPixel(hv_HomMat2D, 
					Parameter.circlesParameter.hv_Row_circle_out, 
					Parameter.circlesParameter.hv_Column_circle_out, 
					&hv_Row_circle_out_tran, 
					&hv_Column_circle_out_tran);
				AffineTransPixel(hv_HomMat2D, 
					Parameter.circlesParameter.hv_Row_circle_in, 
					Parameter.circlesParameter.hv_Column_circle_in, 
					&hv_Row_circle_in_tran, 
					&hv_Column_circle_in_tran);
				if (0 != (HTuple(HTuple(HTuple((0<hv_Row_circle_out_tran)<(double)hv_Width).TupleOr((0<hv_Column_circle_out_tran)<(double)hv_Height)).TupleOr((0<hv_Row_circle_in_tran)<(double)hv_Width)).TupleOr((0<hv_Column_circle_in_tran)<(double)hv_Height)))
				{
					//************************选定目标区域1********************
					GenCircle(&ho_Circle_out, hv_Row_circle_out_tran, hv_Column_circle_out_tran, Parameter.circlesParameter.hv_Radius_circle_out);
					GenCircle(&ho_Circle_in, hv_Row_circle_in_tran, hv_Column_circle_in_tran, Parameter.circlesParameter.hv_Radius_circle_in);
					SymmDifference(ho_Circle_out, ho_Circle_in, &ho_Difference_circle);

					//******提取目标区域1内的轮廓*********
					ReduceDomain(dstImage, ho_Difference_circle, &ho_ImageReduced);
					EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 15, 15);
				/*	UnionCocircularContoursXld(ho_Edges, &ho_UnionContours, HTuple(10).TupleRad(), 
						HTuple(3).TupleRad(), HTuple(10).TupleRad(), 50, 10, 10, "true", 1);*/
					UnionAdjacentContoursXld(ho_Edges,&ho_UnionContours,20,1,"attr_keep");
					SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 50, 10000, 0, 0);
		/*			SegmentContoursXld (ho_SelectedContours,  &ho_SelectedContours, "lines_circles", 5, 4, 2);
					SelectShapeXld(ho_SelectedContours, &ho_SelectedXLD, "circularity", "and", 0.05, 1);*/				
					//************排序选择最长的线并画直线*****************
					//排序
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
						"row");
					//求面积
					//area_center_points_xld (SortedContours, Area_SortedContours, Row_SortedContours, Column_SortedContours)
					//计算目标数
					CountObj(ho_SortedContours, &hv_Number_edges);
					if (0 != (hv_Number_edges==0))
					{
						//AfxMessageBox(L"区域1未提取到边缘");
						return false;
					}
					else
					{
						//拟合半径参数
						hv_radius_max_object = 0;
						hv_radius_min_object = 10000;
						//目标轮廓索引
						hv_Index_max_obj = HTuple();
						hv_Index_in_obj = HTuple();
						//找到半径最大和最小的索引
						HTuple end_val159 = hv_Number_edges-1;
						HTuple step_val159 = 1;
						for (hv_i=0; hv_i.Continue(end_val159, step_val159); hv_i += step_val159)
						{
							SelectObj(ho_SortedContours, &ho_edge_i, hv_i+1);
							FitCircleContourXld(ho_edge_i, "algebraic", -1, 0, 0, 3, 2, &hv_Row2, 
								&hv_Column2, &hv_Radius_i, &hv_StartPhi2, &hv_EndPhi2, &hv_PointOrder2);
							if(((Parameter.circlesParameter.hv_Radius_circle_out>hv_Radius_i)&&(hv_Radius_i>Parameter.circlesParameter.hv_Radius_circle_in))
								||((Parameter.circlesParameter.hv_Radius_circle_out<hv_Radius_i)&&(hv_Radius_i<Parameter.circlesParameter.hv_Radius_circle_in)))
							{
								if (0 != (hv_Radius_i>hv_radius_max_object))
								{
									hv_radius_max_object = hv_Radius_i;
									hv_Index_max_obj = hv_i+1;
								}
								if (0 != (hv_Radius_i<hv_radius_min_object))
								{
									hv_radius_min_object = hv_Radius_i;
									hv_Index_in_obj = hv_i+1;
								}
							}
							
						}
						if (0 != (Parameter.circlesParameter.hv_Radius_circle_out>Parameter.circlesParameter.hv_Radius_circle_in))
						{
							SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_max_obj);
						}
						else
						{
							SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_in_obj);
						}
						//拟合圆并计算参数
						FitCircleContourXld(ho_ObjectSelected, "atukey", -1, 0, 0, 3, 2, &hv_Row_circle_center, 
							&hv_Column_circle_center, &hv_Radius_circle, &hv_StartPhi, &hv_EndPhi, 
							&hv_PointOrder);

						//画出圆
						GenCircle(&ho_Circle, hv_Row_circle_center, hv_Column_circle_center, hv_Radius_circle);
					}

					HObject  ho_Circle_out1, ho_Circle_in1, ho_Difference_circle1, ho_ImageReduced1,
						ho_Edges1, ho_UnionContours1, ho_SelectedContours1, ho_SortedContours1, ho_edge_j, ho_ObjectSelected1,
						ho_Circle1, ho_SelectedXLD1;
					HTuple hv_Number_edges1, hv_radius_max_object1, hv_radius_min_object1, 
						hv_Index_max_obj1, hv_Index_in_obj1, hv_j,  
						hv_Radius_j, hv_Row_circle_center1, 
						hv_Column_circle_center1, hv_Radius_circle1, hv_StartPhi1, hv_EndPhi1, hv_PointOrder1;
					  HTuple 	   hv_Row_circle_out_tran1,	hv_Column_circle_out_tran1,	hv_Row_circle_in_tran1,	   hv_Column_circle_in_tran1;
					//************************选定目标区域2********************
 				AffineTransPixel(hv_HomMat2D, 
					Parameter.circlesParameter.hv_Row_circle_out1, 
					Parameter.circlesParameter.hv_Column_circle_out1, 
					&hv_Row_circle_out_tran1, 
					&hv_Column_circle_out_tran1);
				AffineTransPixel(hv_HomMat2D, 
					Parameter.circlesParameter.hv_Row_circle_in1, 
					Parameter.circlesParameter.hv_Column_circle_in1, 
					&hv_Row_circle_in_tran1, 
					&hv_Column_circle_in_tran1);
					GenCircle(&ho_Circle_out1, hv_Row_circle_out_tran1, hv_Column_circle_out_tran1, Parameter.circlesParameter.hv_Radius_circle_out1);
					GenCircle(&ho_Circle_in1, hv_Row_circle_in_tran1, hv_Column_circle_in_tran1, Parameter.circlesParameter.hv_Radius_circle_in1);
					SymmDifference(ho_Circle_in1, ho_Circle_out1, &ho_Difference_circle1);

					//******提取目标区域2内的轮廓*********
					ReduceDomain(dstImage, ho_Difference_circle1, &ho_ImageReduced1);
					EdgesSubPix(ho_ImageReduced1, &ho_Edges1, "canny", 1, 15, 15);
			/*		UnionCocircularContoursXld(ho_Edges1, &ho_UnionContours1, HTuple(10).TupleRad(), 
						HTuple(3).TupleRad(), HTuple(10).TupleRad(), 50, 10, 10, "true", 1);*/
					UnionAdjacentContoursXld(ho_Edges1,&ho_UnionContours1,20,1,"attr_keep");
					

					SelectContoursXld(ho_UnionContours1, &ho_SelectedContours1, "contour_length", 
						50, 10000, 0, 0);
				//SegmentContoursXld (ho_SelectedContours1,  &ho_SelectedContours1, "lines_circles", 5, 4, 2);
				//	SelectShapeXld(ho_SelectedContours1, &ho_SelectedXLD1, "circularity", "and", 
				//		0.05, 1);
					//************排序选择最长的线*****************
					//排序
					SortContoursXld(ho_SelectedContours1, &ho_SortedContours1, "upper_left", 
						"true", "row");

					//计算目标数
					CountObj(ho_SortedContours1, &hv_Number_edges1);
					if (0 != (hv_Number_edges1==0))
					{
						//AfxMessageBox(L"区域2未提取到边缘");
						ret = false;
					}
					else
					{
						//拟合半径参数
						hv_radius_max_object1 = 0;
						hv_radius_min_object1 = 1000;
						//目标轮廓索引
						hv_Index_max_obj1 = HTuple();
						hv_Index_in_obj1 = HTuple();
						//找到半径最大和最小的索引
						HTuple end_val234 = hv_Number_edges1-1;
						HTuple step_val234 = 1;
						for (hv_j=0; hv_j.Continue(end_val234, step_val234); hv_j += step_val234)
						{
							SelectObj(ho_SortedContours1, &ho_edge_j, hv_j+1);
							FitCircleContourXld(ho_edge_j, "algebraic", -1, 0, 0, 3, 2, &hv_Row2, 
								&hv_Column2, &hv_Radius_j, &hv_StartPhi2, &hv_EndPhi2, &hv_PointOrder2);
							if(((Parameter.circlesParameter.hv_Radius_circle_out>hv_Radius_j)&&(hv_Radius_j>Parameter.circlesParameter.hv_Radius_circle_in))
								||((Parameter.circlesParameter.hv_Radius_circle_out<hv_Radius_j)&&(hv_Radius_j<Parameter.circlesParameter.hv_Radius_circle_in)))
							{
								if (0 != (hv_Radius_j>hv_radius_max_object1))
								{
									hv_radius_max_object1 = hv_Radius_j;
									hv_Index_max_obj1 = hv_j+1;
								}
								if (0 != (hv_Radius_j<hv_radius_min_object1))
								{
									hv_radius_min_object1 = hv_Radius_j;
									hv_Index_in_obj1 = hv_j+1;
								}
							}		
						}
						if (0 != (Parameter.circlesParameter.hv_Radius_circle_out1>Parameter.circlesParameter.hv_Radius_circle_in1))
						{
							SelectObj(ho_SortedContours1, &ho_ObjectSelected1, hv_Index_max_obj1);
						}
						else
						{
							SelectObj(ho_SortedContours1, &ho_ObjectSelected1, hv_Index_in_obj1);
						}

						//计算目标参数
						FitCircleContourXld(ho_ObjectSelected1, "atukey", -1, 0, 0, 3, 2, &hv_Row_circle_center1, 
							&hv_Column_circle_center1, &hv_Radius_circle1, &hv_StartPhi1, &hv_EndPhi1, 
							&hv_PointOrder1);
						//根据参数画圆
						GenCircle(&ho_Circle1, hv_Row_circle_center1, hv_Column_circle_center1, 
							hv_Radius_circle1);
					}
					HObject ho_RegionLines;
					HTuple hv_Angle_circle;

					switch(Parameter.circlesParameter.basepoint)
					{
					case 0:
						{
							GenRegionLine(&ho_RegionLines, hv_Row_circle_center, hv_Column_circle_center, 
								hv_Row_circle_center1, hv_Column_circle_center1);
							AngleLx(hv_Row_circle_center, hv_Column_circle_center, hv_Row_circle_center1, 
								hv_Column_circle_center1, &hv_Angle_circle);
							 double intf=(double)hv_Angle_circle;
							//hv_Angle_circle = -hv_Angle_circle;
							//映射
							//VectorAngleToRigid(0, 0, 0, -hv_Height/2, hv_Width/2, HTuple(-90).TupleRad(), &hv_HomMat2D);
							//AffineTransPixel(hv_HomMat2D, hv_Row_circle_center, hv_Column_circle_center, &hv_x, &hv_y);
							//自计算							
							hv_x = hv_Column_circle_center-hv_Width/2;
							hv_y = hv_Height/2-hv_Row_circle_center;

							results[0] = (double)hv_x*hv_AhcX;
							results[1] = (double)hv_y*hv_AhcY;
							results[2] = (double)hv_Angle_circle.TupleDeg();
							results[3] = (double)hv_Score*100;

							//圆弧
							Union2(ho_Circle, ho_Circle1, &ho_Circle1);
							Tool.outlines.push_back(ho_Circle1);
							//搜索区域 连线
							Tool.outlines.push_back(Rect);
							//基准点
							//在重心画十字线
							HObject ho_Cross;
							GenCrossContourXld(&ho_Cross, hv_Row_circle_center, hv_Column_circle_center, 8, 0);
							//连线
							Tool.outlines.push_back(ho_Cross);
							Tool.outlines.push_back(ho_RegionLines);

							//中心坐标(像素)
							results[4] = (double)hv_x;
							results[5] = (double)hv_y;
							results[6] = (double)hv_Angle_circle.TupleDeg();

							//圆半径(像素)
							results[7] = (double)hv_Radius_circle*Parameter.hv_Ahc;

							//第二个圆数据
							//AffineTransPixel(hv_HomMat2D, hv_Row_circle_center1, hv_Column_circle_center1, &hv_x, &hv_y);
							hv_x = hv_Row_circle_center1-hv_Width/2;
							hv_y = hv_Height/2-hv_Column_circle_center1;
							results[8] = (double)hv_x;
							results[9] = (double)hv_y;
							results[10] = (double)hv_Radius_circle1*Parameter.hv_Ahc;
						}
						break;
					case 1:
						{
							GenRegionLine(&ho_RegionLines, hv_Row_circle_center1, hv_Column_circle_center1,
								hv_Row_circle_center, hv_Column_circle_center);
							AngleLx(hv_Row_circle_center1, hv_Column_circle_center1, 
								hv_Row_circle_center, hv_Column_circle_center,&hv_Angle_circle);
							//hv_Angle_circle = -hv_Angle_circle;
							//VectorAngleToRigid(0, 0, 0, -hv_Height/2, hv_Width/2, HTuple(-90).TupleRad(), &hv_HomMat2D);
							//AffineTransPixel(hv_HomMat2D, hv_Row_circle_center1, hv_Column_circle_center1, &hv_x, &hv_y);
							//自计算
							hv_x = hv_Column_circle_center1-hv_Width/2;
							hv_y = hv_Height/2-hv_Row_circle_center1;

							results[0] = (double)hv_Row_circle_center1*hv_AhcX;
							results[1] = (double)hv_Column_circle_center1*hv_AhcY;				
							results[2] = (double)hv_Angle_circle.TupleDeg();
							results[3] = (double)hv_Score*100;

							//圆弧
							Union2(ho_Circle, ho_Circle1, &ho_Circle1);					
							Tool.outlines.push_back(ho_Circle1);
							//搜索区域
							Tool.outlines.push_back(Rect);
							//基准点
							//在重心画十字线
							HObject ho_Cross;
							GenCrossContourXld(&ho_Cross, hv_Row_circle_center, hv_Column_circle_center, 8, 0);
							//连线
							Tool.outlines.push_back(ho_Cross);
							Tool.outlines.push_back(ho_RegionLines);

							//中心坐标(像素)
							results[4] = (double)hv_x;
							results[5] = (double)hv_y;
							results[6] = (double)hv_Angle_circle.TupleDeg();

							//圆半径(mm)
							results[7] = (double)hv_Radius_circle1*Parameter.hv_Ahc;
							
							//第二个圆数据
							//AffineTransPixel(hv_HomMat2D, hv_Row_circle_center, hv_Column_circle_center, &hv_x, &hv_y);
							hv_x = hv_Row_circle_center-hv_Width/2;
							hv_y = hv_Height/2-hv_Column_circle_center;
							results[8] = (double)hv_x;
							results[9] = (double)hv_y;
							results[10] = (double)hv_Radius_circle*Parameter.hv_Ahc;
						}
						break;
					}	
				}
			}
			//矩形匹配
			else if(L"Rects" == Parameter.check)
			{
				HObject  
					ho_Rectangle_tran_out, ho_Rectangle_tran_in, 
					ho_Difference_rectangle, ho_ImageReduced,
					ho_Edges, ho_UnionContours, ho_UnionContours1,
					ho_SelectedContours, ho_SelectedContours1,
					ho_SortedContours, ho_edge_i, ho_ObjectSelected, ho_Rectangle_obj,
					ho_Circle, ho_SelectedXLD;
				HTuple 
					hv_Phitran_rectangle_out, hv_Phitran_rectangle_in,
					hv_Rowtran_rectangle_out, hv_Coltran_rectangle_out,
					hv_Rowtran_rectangle_in, hv_Coltran_rectangle_in,
					hv_Number_edges, hv_Length_max_object, hv_Length_min_object, 
					hv_Index_max_obj, hv_Index_min_obj,
					hv_Index_in_obj, hv_i, hv_Row2, 
					hv_Column2, hv_Radius_i, hv_StartPhi2, hv_EndPhi2, hv_PointOrder2, 
					hv_Row_rec_obj, hv_Column_rec_obj, hv_Phi_rec_obj, hv_Length1_rec_obj, hv_Length2_rec_obj, hv_PointOrder,
					hv_Row_rec_fit, hv_Column_rec_fit, hv_Phi_rec_fit, hv_Length1_rec_fit, hv_Length2_rec_fit, hv_PointOrder3,
					hv_x, hv_y;

				//**********搜索目标区域1*************
				VectorAngleToRigid(Parameter.hv_Row, Parameter.hv_Column, Parameter.hv_Angle, 
					hv_Row, hv_Column, Parameter.hv_Angle+hv_Angle, &hv_HomMat2D);
				AffineTransPixel(hv_HomMat2D, Parameter.rectsParameter.hv_Row_rectangle_out, Parameter.rectsParameter.hv_Column_rectangle_out, 
					&hv_Rowtran_rectangle_out, &hv_Coltran_rectangle_out);
				AffineTransPixel(hv_HomMat2D, Parameter.rectsParameter.hv_Row_rectangle_in, Parameter.rectsParameter.hv_Column_rectangle_in, 
					&hv_Rowtran_rectangle_in, &hv_Coltran_rectangle_in);
				hv_Phitran_rectangle_out = Parameter.rectsParameter.hv_Phi_rectangle_out+hv_Angle;
				hv_Phitran_rectangle_in = Parameter.rectsParameter.hv_Phi_rectangle_in+hv_Angle;
				//目标区域中心在图像坐标系内
				if (0 != (HTuple(HTuple(HTuple((0<hv_Rowtran_rectangle_out)<(double)hv_Width).TupleOr((0<hv_Coltran_rectangle_out)<(double)hv_Height)).TupleOr((0<hv_Rowtran_rectangle_in)<(double)hv_Width)).TupleOr((0<hv_Coltran_rectangle_in)<(double)hv_Height)))
				{
					//************************选定目标区域1********************
					GenRectangle2(&ho_Rectangle_tran_out, hv_Rowtran_rectangle_out, hv_Coltran_rectangle_out, 
						hv_Phitran_rectangle_out, Parameter.rectsParameter.hv_Length1_rectangle_out, Parameter.rectsParameter.hv_Length2_rectangle_out);
					GenRectangle2(&ho_Rectangle_tran_in, hv_Rowtran_rectangle_in, hv_Coltran_rectangle_in, 
						hv_Phitran_rectangle_in, Parameter.rectsParameter.hv_Length1_rectangle_in, Parameter.rectsParameter.hv_Length2_rectangle_in);
					SymmDifference(ho_Rectangle_tran_out, ho_Rectangle_tran_in, &ho_Difference_rectangle);
					ReduceDomain(dstImage, ho_Difference_rectangle, &ho_ImageReduced);

					EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 20, 40);
					/////////////////////////20180528修改///////////////////////
					UnionAdjacentContoursXld(ho_Edges, &ho_Edges, 15, 1, "attr_keep");
					UnionCollinearContoursXld(ho_Edges, &ho_UnionContours, 10, 1, 2, 0.1, "attr_keep");
					UnionAdjacentContoursXld(ho_UnionContours, &ho_UnionContours1, 5, 1, "attr_keep");
					SelectContoursXld(ho_UnionContours1, &ho_SelectedContours, "contour_length", 
						50, 10000, 0, 0);
					SelectContoursXld(ho_SelectedContours, &ho_SelectedContours1, "closed", 0, 
						2, -0.5, 0.5);

					//************排序选择最长的线并画直线*****************
					//排序
					SortContoursXld(ho_SelectedContours1, &ho_SortedContours, "upper_left", "true", 
						"row");
					//计算目标数
					CountObj(ho_SortedContours, &hv_Number_edges);
					if (0 != (hv_Number_edges==0))
					{
						//AfxMessageBox(L"目标区域未提取到边缘");
						ret = false;
					}
					else
					{
						//拟合半径参数
						hv_Length_max_object = 0;
						hv_Length_min_object = 1000;
						//目标轮廓索引
						hv_Index_max_obj = HTuple();
						hv_Index_min_obj = HTuple();
						//找到半径最大和最小的索引
						HTuple end_val147 = hv_Number_edges-1;
						HTuple step_val147 = 1;
						for (hv_i=0; hv_i.Continue(end_val147, step_val147); hv_i += step_val147)
						{
							SelectObj(ho_SortedContours, &ho_edge_i, hv_i+1);
							FitRectangle2ContourXld(ho_edge_i, "regression", -1, 0, 0, 3, 2, &hv_Row_rec_fit, 
								&hv_Column_rec_fit, &hv_Phi_rec_fit, &hv_Length1_rec_fit, &hv_Length2_rec_fit, 
								&hv_PointOrder3);

							if((Parameter.rectsParameter.hv_Length1_rectangle_out>hv_Length1_rec_fit )&& (hv_Length1_rec_fit>Parameter.rectsParameter.hv_Length1_rectangle_in)
								||(Parameter.rectsParameter.hv_Length1_rectangle_out<hv_Length1_rec_fit )&& (hv_Length1_rec_fit<Parameter.rectsParameter.hv_Length1_rectangle_in))
							{
								if (0 != (hv_Length1_rec_fit>hv_Length_max_object))
								{
									hv_Length_max_object = hv_Length1_rec_fit;
									hv_Index_max_obj = hv_i+1;
								}
								if (0 != (hv_Length1_rec_fit<hv_Length_min_object))
								{
									hv_Length_min_object = hv_Length1_rec_fit;
									hv_Index_min_obj = hv_i+1;
								}
							}
						}
						if (0 != (Parameter.rectsParameter.hv_Length1_rectangle_out>Parameter.rectsParameter.hv_Length1_rectangle_in))
						{
							SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_max_obj);
						}
						else
						{
							SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_min_obj);
						}
						//拟合矩形并计算参数
						FitRectangle2ContourXld(ho_ObjectSelected, "regression", -1, 0, 0, 3, 2, 
							&hv_Row_rec_obj, &hv_Column_rec_obj, &hv_Phi_rec_obj, &hv_Length1_rec_obj, 
							&hv_Length2_rec_obj, &hv_PointOrder);

						//画出矩形
						GenRectangle2(&ho_Rectangle_obj, hv_Row_rec_obj, hv_Column_rec_obj, hv_Phi_rec_obj, 
							hv_Length1_rec_obj, hv_Length2_rec_obj);
					}
				
					//VectorAngleToRigid(0, 0, 0, -hv_Height/2, hv_Width/2, HTuple(-90).TupleRad(), &hv_HomMat2D);
					//AffineTransPixel(hv_HomMat2D, hv_Row_rec_obj, hv_Column_rec_obj, &hv_x, &hv_y);
					//自计算
					hv_x = (hv_Column_rec_obj-hv_Width/2);
					hv_y = (hv_Height/2-hv_Row_rec_obj);

					results[0] = (double)hv_x*hv_AhcX;
					results[1] = (double)hv_y*hv_AhcY;
					results[2] = (double)hv_Phi_rec_obj.TupleDeg();
					results[3] = (double)hv_Score*100;

					//中心坐标(像素)
					results[4] = (double)hv_x;
					results[5] = (double)hv_y;
					results[6] = (double)hv_Phi_rec_obj.TupleDeg();

					//矩形长短(mm)
					results[7] = (double)hv_Length1_rec_obj*Parameter.hv_Ahc;
					results[8] = (double)hv_Length2_rec_obj*Parameter.hv_Ahc;

					//圆弧				
					Tool.outlines.push_back(ho_Rectangle_obj);
					//搜索区域
					Tool.outlines.push_back(Rect);
					//基准点
					//在重心画十字线
					HObject ho_Cross;
					GenCrossContourXld(&ho_Cross, hv_Row_rec_obj, hv_Column_rec_obj, 8, 0);
					Tool.outlines.push_back(ho_Cross);
				}
			}
			//单直线匹配
			else if(L"Line" == Parameter.check)
			{
				// Local iconic variables 
				HObject  ho_Rectangle_model, ho_Domain_model;
				HObject  ho_Rectangle_obj, ho_image_obj, ho_Rectangle_obj0;
				HObject  ho_Image_median, ho_Domain_Ractangle, ho_Edges;
				HObject  ho_UnionContours, ho_SelectedContours, ho_SortedContours;
				HObject  ho_ObjectSelected, ho_Regions_line, ho_RegionIntersection;
				HObject  ho_Rectangle_obj1,ho_RegionIntersection1;
					
				// Local control variables 
				HTuple	hv_Area1, hv_Row_LineCenter1, hv_Column_LineCenter1;
				HTuple  hv_Width_model, hv_Height_model;
				HTuple  hv_mult_bool, hv_Parameter_mult, hv_Parameter_add;
				HTuple  hv_Row1_rec;
				HTuple  hv_Column1_rec, hv_Row2_rec, hv_Column2_rec, hv_Row_center_rec_model;
				HTuple  hv_Column_center_rec_model, hv_Angle_rec, hv_Length1_rec_model;
				HTuple  hv_Length2_rec_model, hv_Constract;
				HTuple  hv_ImageFiles, hv_Index, hv_Width_Image;
				HTuple  hv_Height_Image;
				HTuple  hv_HomMat2D_model, hv_RowTrans;
				HTuple  hv_ColTrans, hv_Number_edges, hv_RowBegin, hv_ColBegin;
				HTuple  hv_RowEnd, hv_ColEnd, hv_Nr, hv_Nc, hv_Dist, hv_angle_line;
				HTuple  hv_Area, hv_Row_LineCenter, hv_Column_LineCenter;
				HTuple  hv_Angle1, hv_DegAngle, hv_Angle_mark, hv_Row_mark;
				HTuple  hv_Row_corner, hv_Column_mark, hv_Column_corner;
				HTuple  hv_Angle_line;

				VectorAngleToRigid(Parameter.hv_Row, Parameter.hv_Column, Parameter.hv_Angle, hv_Row, hv_Column, 
					Parameter.hv_Angle+hv_Angle, &hv_HomMat2D_model);
				AffineTransPixel(hv_HomMat2D_model, Parameter.lineParameter.hv_Row_rec, Parameter.lineParameter.hv_Column_rec, &hv_RowTrans, 
					&hv_ColTrans);
				GenRectangle2(&ho_Rectangle_obj0, hv_RowTrans, hv_ColTrans, Parameter.lineParameter.hv_Phi_rec+hv_Angle, 
					Parameter.lineParameter.hv_Length1_rec, Parameter.lineParameter.hv_Length2_rec);
				GenRectangle2(&ho_Rectangle_obj1, 
					hv_RowTrans-(((Parameter.lineParameter.hv_Phi_rec+hv_Angle).TupleSin())*Parameter.lineParameter.hv_Length1_rec), 
					hv_ColTrans+(((Parameter.lineParameter.hv_Phi_rec+hv_Angle).TupleCos())*Parameter.lineParameter.hv_Length1_rec), 
					Parameter.lineParameter.hv_Phi_rec+hv_Angle, 
					Parameter.lineParameter.hv_Length2_rec, 
					Parameter.lineParameter.hv_Length2_rec);

				//******提取目标区域1内的轮廓*********
				MedianImage(imageRect, &ho_Image_median, "square", Parameter.hv_Parameter_median, "mirrored");
				ReduceDomain(ho_Image_median, ho_Rectangle_obj0, &ho_Domain_Ractangle);
				EdgesSubPix(ho_Domain_Ractangle, &ho_Edges, "canny", 1, 20, 40);
				UnionAdjacentContoursXld(ho_Edges, &ho_UnionContours, 15, 1, "attr_keep");
				/////////////////////////20180528修改///////////////////////
				//选择长度大于20的边缘
				SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 
					20, 10000, 0, 0);
				//选择头尾相差超过20的轮廓
				SelectContoursXld(ho_SelectedContours, &ho_SelectedContours, "open", 20, 10000, 
					0, 0);
				//*矩形框角度在-45到45之间
				if (0 != ((Parameter.lineParameter.hv_Phi_rec.TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
						"column");
					//求面积
				}
				//*矩形框角度在45到135之间
				if (0 != (((Parameter.lineParameter.hv_Phi_rec-(HTuple(90).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "false", 
						"row");
				}
				//*矩形框角度135到225之间
				if (0 != (((Parameter.lineParameter.hv_Phi_rec-(HTuple(180).TupleRad())).TupleCos())>=((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "false", 
						"column");
				}
				//*矩形框角度在225到315（-45）之间
				if (0 != (((Parameter.lineParameter.hv_Phi_rec-(HTuple(270).TupleRad())).TupleCos())>((HTuple(2).TuplePow(0.5))/2)))
				{
					SortContoursXld(ho_SelectedContours, &ho_SortedContours, "upper_left", "true", 
						"row");
				}

				CountObj(ho_SortedContours, &hv_Number_edges);
				if (0 != (hv_Number_edges==0))
				{
					//AfxMessageBox(L"区域1未提取到边缘");
					return false;
				}
				else
				{

					//选择序列中的第一个轮廓
					SelectObj(ho_SortedContours, &ho_ObjectSelected, 1);
					//拟合直线并计算参数
					FitLineContourXld(ho_ObjectSelected, "drop", -1, 10, 5, 1, &hv_RowBegin, 
						&hv_ColBegin, &hv_RowEnd, &hv_ColEnd, &hv_Nr, &hv_Nc, &hv_Dist);

					if (0 != (hv_Nc>0))
					{
						hv_angle_line = (hv_Nr/hv_Nc).TupleAtan();
					}
					else if (0 != (hv_Nc==0))
					{
						hv_angle_line = hv_Nr*(HTuple(90).TupleRad());
					}
					else
					{
						hv_angle_line = ((hv_Nr/hv_Nc).TupleAtan())+(HTuple(180).TupleRad());
					}
					//画出直线
					GenRegionHline(&ho_Regions_line, hv_angle_line, hv_Dist);
					Intersection(ho_Regions_line, ho_Rectangle_obj0, &ho_RegionIntersection);
					Intersection(ho_Regions_line, ho_Rectangle_obj1, &ho_RegionIntersection1);
				}

				//****求线中点
				AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Row_LineCenter, &hv_Column_LineCenter);
				
				AreaCenter(ho_RegionIntersection1, &hv_Area1, &hv_Row_LineCenter1, &hv_Column_LineCenter1);
				AngleLx(hv_Row_LineCenter, hv_Column_LineCenter, hv_Row_LineCenter1, hv_Column_LineCenter1, 
					&hv_Angle1);
				hv_Angle_mark = hv_Angle1;

				hv_Angle_mark = hv_Angle1.TupleDeg();

				//自计算
				HTuple hv_x = (hv_Column_LineCenter-hv_Width/2);
				HTuple hv_y = (hv_Height/2-hv_Row_LineCenter);

				//在重心画十字线
				HObject ho_Cross;
				GenCrossContourXld(&ho_Cross, hv_Row_LineCenter, hv_Column_LineCenter, 8, 0);
				//直线
				Tool.outlines.push_back(ho_RegionIntersection);
				//搜索区域
				Tool.outlines.push_back(Rect);
				//中心
				Tool.outlines.push_back(ho_Cross);
				results[0] = (double)hv_x*hv_AhcX;
				results[1] = (double)hv_y*hv_AhcY;
				results[2] = (double)hv_Angle_mark;
				results[3] = (double)hv_Score*100;
			}
			//实心圆匹配
			else if(L"SolidCircle" == Parameter.check)
			{
				HObject  
					ho_Circle_out, ho_Circle_in, 
					ho_Difference_circle, ho_ImageReduced,
					ho_Edges, ho_UnionContours, ho_SelectedContours, ho_SortedContours, ho_edge_i, ho_ObjectSelected,
					ho_Circle, ho_SelectedXLD;
				HTuple 
					hv_Row_circle_out_tran, hv_Column_circle_out_tran, 
					hv_Row_circle_in_tran, hv_Column_circle_in_tran,
					hv_Number_edges, hv_radius_max_object, hv_radius_min_object, 
					hv_Index_max_obj, hv_Index_in_obj, hv_i, hv_Row2, 
					hv_Column2, hv_Radius_i, hv_StartPhi2, hv_EndPhi2, hv_PointOrder2, 
					hv_Row_circle_center, hv_Column_circle_center, 
					hv_Radius_circle, hv_StartPhi, hv_EndPhi, hv_PointOrder,
					hv_x, hv_y;

				//**********搜索目标区域1*************
				VectorAngleToRigid(Parameter.hv_Row, Parameter.hv_Column, Parameter.hv_Angle, 
					hv_Row, hv_Column, Parameter.hv_Angle+hv_Angle, &hv_HomMat2D);
				AffineTransPixel(hv_HomMat2D, 
					Parameter.circleParameter.hv_Row_circle_out, 
					Parameter.circleParameter.hv_Column_circle_out, 
					&hv_Row_circle_out_tran, 
					&hv_Column_circle_out_tran);
				AffineTransPixel(hv_HomMat2D, 
					Parameter.circleParameter.hv_Row_circle_in, 
					Parameter.circleParameter.hv_Column_circle_in, 
					&hv_Row_circle_in_tran, 
					&hv_Column_circle_in_tran);
				if (0 != (HTuple(HTuple(HTuple((0<hv_Row_circle_out_tran)<(double)hv_Width).TupleOr((0<hv_Column_circle_out_tran)<(double)hv_Height)).TupleOr((0<hv_Row_circle_in_tran)<(double)hv_Width)).TupleOr((0<hv_Column_circle_in_tran)<(double)hv_Height)))
				{
					//************************选定目标区域1********************
					GenCircle(&ho_Circle_out, hv_Row_circle_out_tran, hv_Column_circle_out_tran, Parameter.circleParameter.hv_Radius_circle_out);
					GenCircle(&ho_Circle_in, hv_Row_circle_in_tran, hv_Column_circle_in_tran, Parameter.circleParameter.hv_Radius_circle_in);
					SymmDifference(ho_Circle_out, ho_Circle_in, &ho_Difference_circle);

					//******提取目标区域1内的轮廓*********
					ReduceDomain(dstImage, ho_Difference_circle, &ho_ImageReduced);
					EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 1, 20, 40);
					UnionAdjacentContoursXld(ho_Edges,&ho_Edges,15,1,"attr_keep");
					//选择长度大于20的边缘
					UnionCocircularContoursXld(ho_Edges, &ho_UnionContours, HTuple(10).TupleRad(), 
						HTuple(3).TupleRad(), HTuple(10).TupleRad(), 50, 10, 10, "true", 1);
					SelectContoursXld(ho_UnionContours, &ho_SelectedContours, "contour_length", 50, 10000, 0, 0);
					SegmentContoursXld (ho_SelectedContours,  &ho_SelectedContours, "lines_circles", 5, 4, 2);
					SelectShapeXld(ho_SelectedContours, &ho_SelectedXLD, "circularity", "and", 0.05, 1);				
					//************排序选择最长的线并画直线*****************
					//排序
					SortContoursXld(ho_SelectedXLD, &ho_SortedContours, "upper_left", "true", 
						"row");
					//求面积
					//area_center_points_xld (SortedContours, Area_SortedContours, Row_SortedContours, Column_SortedContours)
					//计算目标数
					CountObj(ho_SortedContours, &hv_Number_edges);
					if (0 != (hv_Number_edges==0))
					{
						//AfxMessageBox(L"区域1未提取到边缘");
						return false;
					}
					else
					{
						//拟合半径参数
						hv_radius_max_object = 0;
						hv_radius_min_object = 10000;
						//目标轮廓索引
						hv_Index_max_obj = HTuple();
						hv_Index_in_obj = HTuple();
						//找到半径最大和最小的索引
						HTuple end_val159 = hv_Number_edges-1;
						HTuple step_val159 = 1;
						for (hv_i=0; hv_i.Continue(end_val159, step_val159); hv_i += step_val159)
						{
							SelectObj(ho_SortedContours, &ho_edge_i, hv_i+1);
							FitCircleContourXld(ho_edge_i, "algebraic", -1, 0, 0, 3, 2, &hv_Row2, 
								&hv_Column2, &hv_Radius_i, &hv_StartPhi2, &hv_EndPhi2, &hv_PointOrder2);
							if(((Parameter.circleParameter.hv_Radius_circle_out>hv_Radius_i)&&(hv_Radius_i>Parameter.circleParameter.hv_Radius_circle_in))
								||((Parameter.circleParameter.hv_Radius_circle_out<hv_Radius_i)&&(hv_Radius_i<Parameter.circleParameter.hv_Radius_circle_in)))
							{
								if (0 != (hv_Radius_i>hv_radius_max_object))
								{
									hv_radius_max_object = hv_Radius_i;
									hv_Index_max_obj = hv_i+1;
								}
								if (0 != (hv_Radius_i<hv_radius_min_object))
								{
									hv_radius_min_object = hv_Radius_i;
									hv_Index_in_obj = hv_i+1;
								}
							}

						}
						if (0 != (Parameter.circleParameter.hv_Radius_circle_out>Parameter.circleParameter.hv_Radius_circle_in))
						{
							SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_max_obj);
						}
						else
						{
							SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_Index_in_obj);
						}
						//拟合圆并计算参数
						FitCircleContourXld(ho_ObjectSelected, "atukey", -1, 0, 0, 3, 2, &hv_Row_circle_center, 
							&hv_Column_circle_center, &hv_Radius_circle, &hv_StartPhi, &hv_EndPhi, 
							&hv_PointOrder);

						//画出圆
						GenCircle(&ho_Circle, hv_Row_circle_center, hv_Column_circle_center, hv_Radius_circle);

						hv_X_Trans = (hv_Column_circle_center-hv_Width/2);
						hv_Y_Trans = (hv_Height/2-hv_Row_circle_center);

						results[0] = (double)hv_X_Trans*hv_AhcX;
						results[1] = (double)hv_Y_Trans*hv_AhcY;
						results[2] = (double)0;
						results[3] = (double)100;

						//中心坐标(像素)
						results[4] = (double)hv_X_Trans;
						results[5] = (double)hv_Y_Trans;

						Tool.outlines.clear();
						Tool.outlines.push_back(ho_Circle);
						//在重心画十字线
						HObject ho_Cross;
						GenCrossContourXld(&ho_Cross, hv_Row_circle_center, hv_Column_circle_center, 8, 0);

						Tool.outlines.push_back(Rect);
						Tool.outlines.push_back(ho_Cross);
					}
				}
			}
			results[2] = LitmitAngle(results[2]);
			Tool.strReslut.Format(L"(X,Y,R)=(%.2f,%.2f,%.2f) \n匹配得分： %.4f，时间ms： %.4f \n结果：OK", 
				results[0], results[1], results[2], 
				results[3], (double)((hv_Seconds2-hv_Seconds1)*1000.0));	
			//Tool.ShowImage(dstImage);
			ret = true;
		}
		else
		{
			Tool.strReslut.Format(L"匹配得分： %.4f，时间ms： %.4f \n结果：NG", 
				(double)hv_Score*100, (double)((hv_Seconds2-hv_Seconds1)*1000.0));	
			ret = false;
		}
	}catch(...)
	{
		//AfxMessageBox(L"识别失败");
		return ret;
	}
	return ret;
}
bool CalgorithmRegion::FindUpModel(HalconTool &ToolUp, HObject &imageUp, vector<double> &results,
	ModelParameter &Parameter, bool SaveImage)
{
	return FindModel(ToolUp, imageUp, results,
		UpSetting, Parameter, SaveImage);
}
bool CalgorithmRegion::FindDownModel(HalconTool &ToolDown, HObject &imageDown, vector<double> &results,
	ModelParameter &Parameter, bool SaveImage)
{
	vector<CString> Models;
	return FindModel(ToolDown, imageDown, results,
		DownSetting, Parameter, SaveImage);
}

//保存贴片、底板参数：搜索开始角度 角度范围 合格分数 识别方式 基准线 基准点
bool CalgorithmRegion::SaveParameter(CString Setting, ModelParameter &Parameter)
{
	try{
		CMarkup xml;				//读写xml对象
		if (!xml.Load(Setting))
			AfxMessageBox(L"找不到该xml文件！");
		//第一层
		xml.ResetPos();
		//进入Setting
		if(xml.FindChildElem())
		{
			CString para;	
			//进入模板参数
			bool b = xml.FindChildElem(Parameter.model_Name);
			//进入
			xml.IntoElem();
			{
				xml.FindChildElem(L"hv_AStart");
				para.Format(L"%f", (double)Parameter.hv_AStart);
				xml.SetChildData(para);
				xml.FindChildElem(L"hv_ARange");
				para.Format(L"%f", (double)Parameter.hv_ARange);
				xml.SetChildData(para);
				xml.FindChildElem(L"hv_Score");
				para.Format(L"%f", (double)Parameter.hv_Score);
				xml.SetChildData(para);
				xml.FindChildElem(L"check");
				xml.SetChildData(Parameter.check);
			}

			//进入识别方式
			xml.FindChildElem(Parameter.check);
			xml.IntoElem();		
			xml.FindChildElem(L"baseline");
			para.Format(L"%f", (double)Parameter.linesParameter.baseline);
			xml.SetChildData(para);

			xml.ResetChildPos();
			xml.FindChildElem(L"basepoint");
			para.Format(L"%f", (double)Parameter.circlesParameter.basepoint);
			xml.SetChildData(para);

			//增强图像对比
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Mult");
					para.Format(L"%f", (double)Parameter.hv_Mult);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Add");
					para.Format(L"%f", (double)Parameter.hv_Add);
					xml.SetChildData(para);
					xml.FindChildElem(L"is_Mult");
					para.Format(L"%f", (double)Parameter.is_Mult);
					xml.SetChildData(para);
				}
			}

			//中值滤波器
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_median_bool");
					para.Format(L"%f", (double)Parameter.hv_median_bool);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Parameter_median");
					para.Format(L"%f", (double)Parameter.hv_Parameter_median);
					xml.SetChildData(para);
				}
			}

			//单圆弧匹配
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();

				//进入识别方式
				xml.FindChildElem(L"SolidCircle");
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Row_circle_out");
					para.Format(L"%f", (double)Parameter.circleParameter.hv_Row_circle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_circle_out");
					para.Format(L"%f", (double)Parameter.circleParameter.hv_Column_circle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Radius_circle_out");
					para.Format(L"%f", (double)Parameter.circleParameter.hv_Radius_circle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Row_circle_in");
					para.Format(L"%f", (double)Parameter.circleParameter.hv_Row_circle_in);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_circle_in");
					para.Format(L"%f", (double)Parameter.circleParameter.hv_Column_circle_in);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Radius_circle_in");
					para.Format(L"%f", (double)Parameter.circleParameter.hv_Radius_circle_in);
					xml.SetChildData(para);
				}
			}

			//单直线匹配
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();

				//进入识别方式
				xml.FindChildElem(L"Line");
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Row_rec");
					para.Format(L"%f", (double)Parameter.lineParameter.hv_Row_rec);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_rec");
					para.Format(L"%f", (double)Parameter.lineParameter.hv_Column_rec);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Phi_rec");
					para.Format(L"%f", (double)Parameter.lineParameter.hv_Phi_rec);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length1_rec");
					para.Format(L"%f", (double)Parameter.lineParameter.hv_Length1_rec);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length2_rec");
					para.Format(L"%f", (double)Parameter.lineParameter.hv_Length2_rec);
					xml.SetChildData(para);
				}
			}

			//矩形匹配
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();

				//进入识别方式
				xml.FindChildElem(L"Rects");
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Row_rectangle_out");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Row_rectangle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_rectangle_out");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Column_rectangle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Phi_rectangle_out");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Phi_rectangle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length1_rectangle_out");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Length1_rectangle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length2_rectangle_out");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Length2_rectangle_out);
					xml.SetChildData(para);

					xml.FindChildElem(L"hv_Row_rectangle_in");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Row_rectangle_in);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_rectangle_in");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Column_rectangle_in);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Phi_rectangle_in");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Phi_rectangle_in);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length1_rectangle_in");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Length1_rectangle_in);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length2_rectangle_in");
					para.Format(L"%f", (double)Parameter.rectsParameter.hv_Length2_rectangle_in);
					xml.SetChildData(para);
				}
			}

			//双圆弧匹配
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();

				//进入识别方式
				xml.FindChildElem(L"Circles");
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Row_circle_out");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Row_circle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_circle_out");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Column_circle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Radius_circle_out");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Radius_circle_out);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Row_circle_in");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Row_circle_in);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_circle_in");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Column_circle_in);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Radius_circle_in");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Radius_circle_in);
					xml.SetChildData(para);
				}
			}
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();

				//进入识别方式
				xml.FindChildElem(L"Circles");
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Row_circle_out1");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Row_circle_out1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_circle_out1");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Column_circle_out1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Radius_circle_out1");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Radius_circle_out1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Row_circle_in1");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Row_circle_in1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_circle_in1");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Column_circle_in1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Radius_circle_in1");
					para.Format(L"%f", (double)Parameter.circlesParameter.hv_Radius_circle_in1);
					xml.SetChildData(para);
				}
			}

			//双直线匹配
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();

				//进入识别方式
				xml.FindChildElem(L"Lines");
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Row_rec");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Row_rec);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_rec");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Column_rec);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Phi_rec");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Phi_rec);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length1_rec");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Length1_rec);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length2_rec");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Length2_rec);
					xml.SetChildData(para);
				}
			}
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();

				//进入识别方式
				xml.FindChildElem(L"Lines");
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Row_rec1");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Row_rec1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_rec1");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Column_rec1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Phi_rec1");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Phi_rec1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length1_rec1");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Length1_rec1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length2_rec1");
					para.Format(L"%f", (double)Parameter.linesParameter.hv_Length2_rec1);
					xml.SetChildData(para);
				}
			}

			//选择模板
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();
				{
					xml.FindChildElem(L"hv_Angle");
					para.Format(L"%f", (double)Parameter.hv_Angle);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Row");
					para.Format(L"%f", (double)Parameter.hv_Row);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column");
					para.Format(L"%f", (double)Parameter.hv_Column);
					xml.SetChildData(para);

					xml.FindChildElem(L"hv_Row_center_rec_model");
					para.Format(L"%f", (double)Parameter.hv_Row_center_rec_model);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_center_rec_model");
					para.Format(L"%f", (double)Parameter.hv_Column_center_rec_model);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length1_rec_model");
					para.Format(L"%f", (double)Parameter.hv_Length1_rec_model);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Length2_rec_model");
					para.Format(L"%f", (double)Parameter.hv_Length2_rec_model);
					xml.SetChildData(para);

					xml.FindChildElem(L"hv_Row_gravity_model");
					para.Format(L"%f", (double)Parameter.hv_Row_gravity_model);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column_gravity_model");
					para.Format(L"%f", (double)Parameter.hv_Column_gravity_model);
					xml.SetChildData(para);
				}
			}

			//搜索范围
			//第一层
			xml.ResetPos();
			//进入Setting
			if(xml.FindChildElem())
			{
				CString para;	
				//进入模板参数
				bool b = xml.FindChildElem(Parameter.model_Name);
				//进入
				xml.IntoElem();
				{
					b = xml.FindChildElem(L"hv_Row1");
					para.Format(L"%f", (double)Parameter.hv_Row1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column1");
					para.Format(L"%f", (double)Parameter.hv_Column1);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Row2");
					para.Format(L"%f", (double)Parameter.hv_Row2);
					xml.SetChildData(para);
					xml.FindChildElem(L"hv_Column2");
					para.Format(L"%f", (double)Parameter.hv_Column2);
					xml.SetChildData(para);
				}
			}

			//保存数据
			if(!xml.Save(Setting))
			{
				AfxMessageBox(L"保存失败！");
				return false;
			}

			//保存模板
			char *ch= CPublicTool::MyW2A(Parameter.model_Path);
			WriteShapeModel(Parameter.hv_ModelID,ch);			
			//释放模板变量
			ClearShapeModel(Parameter.hv_ModelID);
			delete []ch;
		}
	}catch(...)
	{
		return false;
	}
	return true;
}
bool CalgorithmRegion::SaveUpParameter(ModelParameter &Parameter)
{
	return SaveParameter(UpSetting, Parameter);
}
bool CalgorithmRegion::SaveDownParameter(ModelParameter &Parameter)
{	
	return SaveParameter(DownSetting, Parameter);
}

//读取所有模板名称
CString CalgorithmRegion::GetModels(CString Setting, vector<CString> &Models, ModelParameter &Parameter, CString dir)
{
	//CString Selected = L"";
	Models.clear();
	CMarkup xml;				//读写xml对象
	CString para;
	if (!xml.Load(Setting))
	{
		AfxMessageBox(L"找不到配置文件，请导入方程");
	}
	else
	{
		xml.ResetPos();
		if(xml.FindChildElem())
		{
			xml.IntoElem();
			//1
			do{
				para = xml.GetTagName();
				if( _T("Program") != para){
					//1.读取所有可选方案
					Models.push_back(para);
				}
				//else{
				//	//2.选中当前方案
				//	xml.FindChildElem(_T("Selected"));
				//	Selected = xml.GetChildData();
				//}
			}while(xml.FindElem());
		}

		//第一层
		xml.ResetPos();
		if(xml.FindChildElem())
		{
			CString para;	
			{
				bool b = xml.FindChildElem(Parameter.model_Name);
				xml.IntoElem();

				//模板路径
				para.Format(L"%s/CalgorithmRegion/%s/%s.shm", pro, dir, Parameter.model_Name);
				Parameter.model_Path = CPublicTool::GetFilePath(para);
				//单位像素长度
				xml.FindChildElem(L"hv_Ahc");
				para = xml.GetChildData();
				Parameter.hv_Ahc = _ttof(para.GetBuffer());

				//搜索区域
				xml.FindChildElem(L"hv_Row1");
				para = xml.GetChildData();
				Parameter.hv_Row1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column1");
				para = xml.GetChildData();
				Parameter.hv_Column1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Row2");
				para = xml.GetChildData();
				Parameter.hv_Row2 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column2");
				para = xml.GetChildData();
				Parameter.hv_Column2 = _ttof(para.GetBuffer());

				//开始角度 角度范围
				xml.FindChildElem(L"hv_AStart");
				para = xml.GetChildData();
				Parameter.hv_AStart = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_ARange");
				para = xml.GetChildData();
				Parameter.hv_ARange = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Score");
				para = xml.GetChildData();
				Parameter.hv_Score = _ttof(para.GetBuffer());

				//*模板参数*********
				//模板角度
				xml.FindChildElem(L"hv_Angle");
				para = xml.GetChildData();
				Parameter.hv_Angle = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Row");
				para = xml.GetChildData();
				Parameter.hv_Row = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column");
				para = xml.GetChildData();
				Parameter.hv_Column = _ttof(para.GetBuffer());

				xml.FindChildElem(L"hv_Row_center_rec_model");
				para = xml.GetChildData();
				Parameter.hv_Row_center_rec_model = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_center_rec_model");
				para = xml.GetChildData();
				Parameter.hv_Column_center_rec_model = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length1_rec_model");
				para = xml.GetChildData();
				Parameter.hv_Length1_rec_model = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length2_rec_model");
				para = xml.GetChildData();
				Parameter.hv_Length2_rec_model = _ttof(para.GetBuffer());

				xml.FindChildElem(L"hv_Row_gravity_model");
				para = xml.GetChildData();
				Parameter.hv_Row_gravity_model = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_gravity_model");
				para = xml.GetChildData();
				Parameter.hv_Column_gravity_model = _ttof(para.GetBuffer());

				xml.FindChildElem(L"hv_Mult");
				para = xml.GetChildData();
				Parameter.hv_Mult = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Add");
				para = xml.GetChildData();
				Parameter.hv_Add = _ttof(para.GetBuffer());
				xml.FindChildElem(L"is_Mult");
				para = xml.GetChildData();
				Parameter.is_Mult = _ttof(para.GetBuffer());

				xml.FindChildElem(L"hv_median_bool");
				para = xml.GetChildData();
				Parameter.hv_median_bool = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Parameter_median");
				para = xml.GetChildData();
				Parameter.hv_Parameter_median = _ttof(para.GetBuffer());
				
				xml.FindChildElem(L"check");
				Parameter.check = xml.GetChildData();
				//进入识别方式
				//双直线匹配
				xml.FindChildElem(L"Lines");
				xml.IntoElem();
				xml.FindChildElem(L"hv_Row_rec");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Row_rec = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_rec");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Column_rec = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Phi_rec");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Phi_rec = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length1_rec");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Length1_rec = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length2_rec");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Length2_rec = _ttof(para.GetBuffer());

				xml.FindChildElem(L"hv_Row_rec1");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Row_rec1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_rec1");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Column_rec1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Phi_rec1");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Phi_rec1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length1_rec1");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Length1_rec1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length2_rec1");
				para = xml.GetChildData();
				Parameter.linesParameter.hv_Length2_rec1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"baseline");
				para = xml.GetChildData();
				Parameter.linesParameter.baseline = _ttof(para.GetBuffer());
				xml.OutOfElem();

				//双圆弧匹配
				xml.FindChildElem(L"Circles");
				xml.IntoElem();
				xml.FindChildElem(L"hv_Row_circle_out");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Row_circle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_circle_out");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Column_circle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Radius_circle_out");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Radius_circle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Row_circle_in");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Row_circle_in = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_circle_in");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Column_circle_in = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Radius_circle_in");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Radius_circle_in = _ttof(para.GetBuffer());

				xml.FindChildElem(L"hv_Row_circle_out1");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Row_circle_out1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_circle_out1");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Column_circle_out1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Radius_circle_out1");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Radius_circle_out1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Row_circle_in1");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Row_circle_in1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_circle_in1");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Column_circle_in1 = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Radius_circle_in1");
				para = xml.GetChildData();
				Parameter.circlesParameter.hv_Radius_circle_in1 = _ttof(para.GetBuffer());

				xml.FindChildElem(L"basepoint");
				para = xml.GetChildData();
				Parameter.circlesParameter.basepoint = _ttof(para.GetBuffer());
				xml.OutOfElem();

				//矩形匹配
				xml.FindChildElem(L"Rects");
				xml.IntoElem();
				xml.FindChildElem(L"hv_Row_rectangle_out");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Row_rectangle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_rectangle_out");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Column_rectangle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Phi_rectangle_out");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Phi_rectangle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length1_rectangle_out");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Length1_rectangle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length2_rectangle_out");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Length2_rectangle_out = _ttof(para.GetBuffer());

				xml.FindChildElem(L"hv_Row_rectangle_in");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Row_rectangle_in = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_rectangle_in");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Column_rectangle_in = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Phi_rectangle_in");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Phi_rectangle_in = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length1_rectangle_in");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Length1_rectangle_in = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length2_rectangle_in");
				para = xml.GetChildData();
				Parameter.rectsParameter.hv_Length2_rectangle_in = _ttof(para.GetBuffer());
				xml.OutOfElem();

				//单直线匹配
				xml.FindChildElem(L"Line");
				xml.IntoElem();
				xml.FindChildElem(L"hv_Row_rec");
				para = xml.GetChildData();
				Parameter.lineParameter.hv_Row_rec = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_rec");
				para = xml.GetChildData();
				Parameter.lineParameter.hv_Column_rec = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Phi_rec");
				para = xml.GetChildData();
				Parameter.lineParameter.hv_Phi_rec = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length1_rec");
				para = xml.GetChildData();
				Parameter.lineParameter.hv_Length1_rec = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Length2_rec");
				para = xml.GetChildData();
				Parameter.lineParameter.hv_Length2_rec = _ttof(para.GetBuffer());
				xml.OutOfElem();

				//实心圆匹配
				xml.FindChildElem(L"SolidCircle");
				xml.IntoElem();
				xml.FindChildElem(L"hv_Row_circle_out");
				para = xml.GetChildData();
				Parameter.circleParameter.hv_Row_circle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_circle_out");
				para = xml.GetChildData();
				Parameter.circleParameter.hv_Column_circle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Radius_circle_out");
				para = xml.GetChildData();
				Parameter.circleParameter.hv_Radius_circle_out = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Row_circle_in");
				para = xml.GetChildData();
				Parameter.circleParameter.hv_Row_circle_in = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Column_circle_in");
				para = xml.GetChildData();
				Parameter.circleParameter.hv_Column_circle_in = _ttof(para.GetBuffer());
				xml.FindChildElem(L"hv_Radius_circle_in");
				para = xml.GetChildData();
				Parameter.circleParameter.hv_Radius_circle_in = _ttof(para.GetBuffer());

				//释放模板变量
				try{
					ClearShapeModel(Parameter.hv_ModelID);
				}catch(...)
				{
				}				
				//模板变量
				try{
					char *ch=CPublicTool::MyW2A(Parameter.model_Path);
					ReadShapeModel(ch, &Parameter.hv_ModelID);
					delete []ch;
				}catch(...)
				{

				}
			}
		}
	}
	return Parameter.model_Name;
}
CString CalgorithmRegion::GetUpModels(vector<CString> &upModels, ModelParameter &Parameter)
{
	return GetModels(UpSetting, upModels, Parameter, L"up");
}
CString CalgorithmRegion::GetDownModels(vector<CString> &DownModels, ModelParameter &Parameter)
{
	return GetModels(DownSetting, DownModels, Parameter, L"down");
}

//指定模板名称
bool CalgorithmRegion::SetModel(CString Setting, CString Selected, ModelParameter &Parameter)
{
	//模板名称不能以数字开头,开头添加
	if(CPublicTool::IsCStringNumStart(Selected))
	{
		//AfxMessageBox(L"模板名称不能以数字开头");
		Selected.Format(L"_%s", Selected);
	}
	Parameter.model_Name = Selected;

	CMarkup xml;				//读写xml对象
	CString para;
	if (!xml.Load(Setting))
	{
		AfxMessageBox(L"找不到配置文件，请导入方程");
		return false;
	}
	else
	{
		if(L"" != Parameter.model_Name)
		{
			if( !xml.FindChildElem(Parameter.model_Name) )
			{
				//AfxMessageBox(L"贴片方案不存在"); 
				return false;
			}
			else
			{
				//xml.ResetPos();
				//bool b = xml.FindChildElem(_T("Program"));
				//xml.IntoElem(); 
				//b = xml.FindChildElem(_T("Selected"));
				//b = xml.SetChildData(Selected);
				//if(!xml.Save(Setting))
				//{
				//	AfxMessageBox(L"应用方案失败"); 
				//	return false;
				//}
			}
		}
	}
	if(UpSetting == Setting)
	{
		vector<CString> upModels;
		CString upSelected = GetUpModels(upModels, Parameter);
	}

	if(DownSetting == Setting)
	{
		vector<CString> downModels;
		CString downSelected = GetDownModels(downModels, Parameter);
	}
	return true;
}
bool CalgorithmRegion::SetUpModel(CString upSelected, ModelParameter &Parameter)
{
	return SetModel(UpSetting, upSelected, Parameter);
}
bool CalgorithmRegion::SetDownModel(CString downSelected, ModelParameter &Parameter)
{
	return SetModel(DownSetting, downSelected, Parameter);
}

//新模板添加
bool CalgorithmRegion::AddModel(CString Setting, CString newModel, ModelParameter &Parameter, CString dir)
{
	//模板名称不能以数字开头,开头添加
	if(CPublicTool::IsCStringNumStart(newModel))
	{
		//AfxMessageBox(L"模板名称不能以数字开头");
		newModel.Format(L"_%s", newModel);
	}
	CMarkup xml;				//读写xml对象
	CString para;
	if (!xml.Load(Setting))
	{
		AfxMessageBox(L"找不到配置文件，请导入方程");
		return false;
	}

	xml.ResetPos();
	if(xml.FindChildElem())
	{
		xml.IntoElem();
		//1
		do{
			para = xml.GetTagName();
			if( _T("Program") != para){
				if(newModel == para)
				{
					//AfxMessageBox(L"贴片模板名称已存在"); 
					return false;
				}
			}
		}while(xml.FindElem());
	}

	//添加新模板参数
	xml.ResetPos();
	xml.FindElem(_T("Setting"));

	//修改当前选择模块
	bool b = xml.FindChildElem(_T("Program"));
	xml.IntoElem(); 
	xml.FindChildElem(_T("Selected"));
	xml.SetChildData(newModel);
	xml.OutOfElem();

	xml.AddChildElem(newModel);
	xml.IntoElem();

	//模板路径
	para.Format(L"%s/CalgorithmRegion/%s/%s.shm", pro, dir, newModel);
	Parameter.model_Path = CPublicTool::GetFilePath(para);
	xml.AddChildElem(L"hv_Ahc");
	para.Format(L"%f", (double)Parameter.hv_Ahc);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Row1");
	para.Format(L"%f", (double)Parameter.hv_Row1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column1");
	para.Format(L"%f", (double)Parameter.hv_Column1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Row2");
	para.Format(L"%f", (double)Parameter.hv_Row2);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column2");
	para.Format(L"%f", (double)Parameter.hv_Column2);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_AStart");
	para.Format(L"%f", (double)Parameter.hv_AStart);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_ARange");
	para.Format(L"%f", (double)Parameter.hv_ARange);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Score");
	para.Format(L"%f", (double)Parameter.hv_Score);
	xml.SetChildData(para);

	xml.AddChildElem(L"hv_Angle");
	para.Format(L"%f", (double)Parameter.hv_Angle);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Row");
	para.Format(L"%f", (double)Parameter.hv_Row);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column");
	para.Format(L"%f", (double)Parameter.hv_Column);
	xml.SetChildData(para);

	xml.AddChildElem(L"hv_Row_center_rec_model");
	para.Format(L"%f", (double)Parameter.hv_Row_center_rec_model);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_center_rec_model");
	para.Format(L"%f", (double)Parameter.hv_Column_center_rec_model);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length1_rec_model");
	para.Format(L"%f", (double)Parameter.hv_Length1_rec_model);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length2_rec_model");
	para.Format(L"%f", (double)Parameter.hv_Length2_rec_model);
	xml.SetChildData(para);

	xml.AddChildElem(L"hv_Row_gravity_model");
	para.Format(L"%f", (double)Parameter.hv_Row_gravity_model);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_gravity_model");
	para.Format(L"%f", (double)Parameter.hv_Column_gravity_model);
	xml.SetChildData(para);

	xml.AddChildElem(L"hv_Mult");
	para.Format(L"%f", (double)Parameter.hv_Mult);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Add");
	para.Format(L"%f", (double)Parameter.hv_Add);
	xml.SetChildData(para);
	xml.AddChildElem(L"is_Mult");
	para.Format(L"%f", (double)Parameter.is_Mult);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_median_bool");
	para.Format(L"%f", (double)Parameter.hv_median_bool);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Parameter_median");
	para.Format(L"%f", (double)Parameter.hv_Parameter_median);
	xml.SetChildData(para);

	xml.AddChildElem(L"check");
	xml.SetChildData(L"");
	xml.AddChildElem(L"Lines");
	//进入识别方式
	xml.IntoElem();

	xml.AddChildElem(L"hv_Row_rec");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Row_rec);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_rec");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Column_rec);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Phi_rec");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Phi_rec);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length1_rec");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Length1_rec);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length2_rec");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Length2_rec);
	xml.SetChildData(para);

	xml.AddChildElem(L"hv_Row_rec1");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Row_rec1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_rec1");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Column_rec1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Phi_rec1");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Phi_rec1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length1_rec1");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Length1_rec1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length2_rec1");
	para.Format(L"%f", (double)Parameter.linesParameter.hv_Length2_rec1);
	xml.SetChildData(para);
	xml.AddChildElem(L"baseline");
	para.Format(L"%f", (double)Parameter.linesParameter.baseline);
	xml.SetChildData(para);

	xml.OutOfElem();	
	xml.AddChildElem(L"Circles");
	//进入识别方式
	xml.IntoElem();

	xml.AddChildElem(L"hv_Row_circle_out");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Row_circle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_circle_out");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Column_circle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Radius_circle_out");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Radius_circle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Row_circle_in");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Row_circle_in);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_circle_in");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Column_circle_in);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Radius_circle_in");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Radius_circle_in);
	xml.SetChildData(para);

	xml.AddChildElem(L"hv_Row_circle_out1");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Row_circle_out1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_circle_out1");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Column_circle_out1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Radius_circle_out1");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Radius_circle_out1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Row_circle_in1");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Row_circle_in1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_circle_in1");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Column_circle_in1);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Radius_circle_in1");
	para.Format(L"%f", (double)Parameter.circlesParameter.hv_Radius_circle_in1);
	xml.SetChildData(para);
	xml.AddChildElem(L"basepoint");
	para.Format(L"%f", (double)Parameter.circlesParameter.basepoint);
	xml.SetChildData(para);

	xml.OutOfElem();	
	xml.AddChildElem(L"Rects");
	//进入识别方式
	xml.IntoElem();

	xml.AddChildElem(L"hv_Row_rectangle_out");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Row_rectangle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_rectangle_out");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Column_rectangle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Phi_rectangle_out");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Phi_rectangle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length1_rectangle_out");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Length1_rectangle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length2_rectangle_out");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Length2_rectangle_out);
	xml.SetChildData(para);

	xml.AddChildElem(L"hv_Row_rectangle_in");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Row_rectangle_in);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_rectangle_in");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Column_rectangle_in);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Phi_rectangle_in");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Phi_rectangle_in);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length1_rectangle_in");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Length1_rectangle_in);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length2_rectangle_in");
	para.Format(L"%f", (double)Parameter.rectsParameter.hv_Length2_rectangle_in);
	xml.SetChildData(para);

	xml.OutOfElem();	
	xml.AddChildElem(L"Line");
	//进入识别方式
	xml.IntoElem();

	xml.AddChildElem(L"hv_Row_rec");
	para.Format(L"%f", (double)Parameter.lineParameter.hv_Row_rec);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_rec");
	para.Format(L"%f", (double)Parameter.lineParameter.hv_Column_rec);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Phi_rec");
	para.Format(L"%f", (double)Parameter.lineParameter.hv_Phi_rec);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length1_rec");
	para.Format(L"%f", (double)Parameter.lineParameter.hv_Length1_rec);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Length2_rec");
	para.Format(L"%f", (double)Parameter.lineParameter.hv_Length2_rec);
	xml.SetChildData(para);

	xml.OutOfElem();	
	xml.AddChildElem(L"SolidCircle");
	//进入识别方式
	xml.IntoElem();

	xml.AddChildElem(L"hv_Row_circle_out");
	para.Format(L"%f", (double)Parameter.circleParameter.hv_Row_circle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_circle_out");
	para.Format(L"%f", (double)Parameter.circleParameter.hv_Column_circle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Radius_circle_out");
	para.Format(L"%f", (double)Parameter.circleParameter.hv_Radius_circle_out);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Row_circle_in");
	para.Format(L"%f", (double)Parameter.circleParameter.hv_Row_circle_in);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Column_circle_in");
	para.Format(L"%f", (double)Parameter.circleParameter.hv_Column_circle_in);
	xml.SetChildData(para);
	xml.AddChildElem(L"hv_Radius_circle_in");
	para.Format(L"%f", (double)Parameter.circleParameter.hv_Radius_circle_in);
	xml.SetChildData(para);

	if(!xml.Save(Setting))
	{
		AfxMessageBox(L"保存失败！");
	}  
	return true;
}
bool CalgorithmRegion::AddUpModel(CString newModel, ModelParameter &Parameter)
{
	//模板路径
	Parameter.model_Path = L"";			
	//贴片单位像素长度
	Parameter.hv_Ahc = 0.0631599599;
	//搜索区域
	Parameter.hv_Row1 = 0;
	Parameter.hv_Column1 = 0;
	Parameter.hv_Row2 = 1024;
	Parameter.hv_Column2 = 1024;

	//开始角度 角度范围
	Parameter.hv_AStart = -10;
	Parameter.hv_ARange = 10;
	//模板参数
	Parameter.hv_Score = 50;		//合格分数
	Parameter.hv_Angle = 0;		//模板角度
	Parameter.hv_Row = 512;		
	Parameter.hv_Column = 512;	
	//矩形参数
	Parameter.hv_Row_center_rec_model = 0;
	Parameter.hv_Column_center_rec_model = 0;
	Parameter.hv_Length1_rec_model = 0;
	Parameter.hv_Length2_rec_model = 0;
	//模板重心坐标
	Parameter.hv_Row_gravity_model = 0;
	Parameter.hv_Column_gravity_model = 0;

	//图像增强对比参数
	Parameter.hv_Mult = 0.01;
	Parameter.hv_Add = -100;
	Parameter.is_Mult = 0;

	//中值滤波
	Parameter.hv_median_bool = 0;
	Parameter.hv_Parameter_median = 1;

	//识别方式
	Parameter.check = L"";

	//直线
	//****目标区域1参数****
	Parameter.linesParameter.hv_Row_rec = 0;
	Parameter.linesParameter.hv_Column_rec = 0;
	Parameter.linesParameter.hv_Phi_rec = 0;
	Parameter.linesParameter.hv_Length1_rec = 0;
	Parameter.linesParameter.hv_Length2_rec = 0;
	//****目标区域1参数****
	Parameter.linesParameter.hv_Row_rec1 = 0;
	Parameter.linesParameter.hv_Column_rec1 = 0;
	Parameter.linesParameter.hv_Phi_rec1 = 0;
	Parameter.linesParameter.hv_Length1_rec1 = 0;
	Parameter.linesParameter.hv_Length2_rec1 = 0;
	//基准线
	Parameter.linesParameter.baseline = 0;

	//圆弧
	//****目标区域1参数****
	Parameter.circlesParameter.hv_Row_circle_out = 0;
	Parameter.circlesParameter.hv_Column_circle_out = 0;
	Parameter.circlesParameter.hv_Radius_circle_out = 0;
	Parameter.circlesParameter.hv_Row_circle_in = 0;
	Parameter.circlesParameter.hv_Column_circle_in = 0;
	Parameter.circlesParameter.hv_Radius_circle_in = 0;
	//****目标区域1参数****
	Parameter.circlesParameter.hv_Row_circle_out1 = 0;
	Parameter.circlesParameter.hv_Column_circle_out1 = 0;
	Parameter.circlesParameter.hv_Radius_circle_out1 = 0;
	Parameter.circlesParameter.hv_Row_circle_in1 = 0;
	Parameter.circlesParameter.hv_Column_circle_in1 = 0;
	Parameter.circlesParameter.hv_Radius_circle_in1 = 0;
	//基准点
	Parameter.circlesParameter.basepoint = 0;

	//矩形
	//****目标区域1参数****
	Parameter.rectsParameter.hv_Row_rectangle_out = 0;
	Parameter.rectsParameter.hv_Column_rectangle_out = 0;
	Parameter.rectsParameter.hv_Phi_rectangle_out = 0;
	Parameter.rectsParameter.hv_Length1_rectangle_out = 0;
	Parameter.rectsParameter.hv_Length2_rectangle_out = 0;
	Parameter.rectsParameter.hv_Row_rectangle_in = 0;
	Parameter.rectsParameter.hv_Column_rectangle_in = 0;
	Parameter.rectsParameter.hv_Phi_rectangle_in = 0;
	Parameter.rectsParameter.hv_Length1_rectangle_in = 0;
	Parameter.rectsParameter.hv_Length2_rectangle_in = 0;

	//单直线
	//****目标区域1参数****
	Parameter.lineParameter.hv_Row_rec = 0;
	Parameter.lineParameter.hv_Column_rec = 0;
	Parameter.lineParameter.hv_Phi_rec = 0;
	Parameter.lineParameter.hv_Length1_rec = 0;
	Parameter.lineParameter.hv_Length2_rec = 0;

	//实心圆匹配
	Parameter.circleParameter.hv_Row_circle_out = 0;
	Parameter.circleParameter.hv_Column_circle_out = 0;
	Parameter.circleParameter.hv_Radius_circle_out = 0;
	Parameter.circleParameter.hv_Row_circle_in = 0;
	Parameter.circleParameter.hv_Column_circle_in = 0;
	Parameter.circleParameter.hv_Radius_circle_in = 0;

	return AddModel(UpSetting, newModel, Parameter, L"up");
}
bool CalgorithmRegion::AddDownModel(CString newModel, ModelParameter &Parameter)
{
	//模板路径
	Parameter.model_Path = L"";			
	//底板单位像素长度
	Parameter.hv_Ahc = 0.0142067517257328;
	//搜索区域
	Parameter.hv_Row1 = 0;
	Parameter.hv_Column1 = 0;
	Parameter.hv_Row2 = 1024;
	Parameter.hv_Column2 = 1024;

	//开始角度 角度范围
	Parameter.hv_AStart = -10;
	Parameter.hv_ARange = 10;
	//模板参数
	Parameter.hv_Score = 50;	//合格分数
	Parameter.hv_Angle = 0;		//模板角度
	Parameter.hv_Row = 512;		
	Parameter.hv_Column = 512;	
	//矩形参数
	Parameter.hv_Row_center_rec_model = 0;
	Parameter.hv_Column_center_rec_model = 0;
	Parameter.hv_Length1_rec_model = 0;
	Parameter.hv_Length2_rec_model = 0;
	//模板重心坐标
	Parameter.hv_Row_gravity_model = 0;
	Parameter.hv_Column_gravity_model = 0;

	//图像增强对比参数
	Parameter.hv_Mult = 0.01;
	Parameter.hv_Add = -100;
	Parameter.is_Mult = 0;
		
	//中值滤波
	Parameter.hv_median_bool = 0;
	Parameter.hv_Parameter_median = 1;

	//识别方式
	Parameter.check = L"";

	//直线
	//****目标区域1参数****
	Parameter.linesParameter.hv_Row_rec = 0;
	Parameter.linesParameter.hv_Column_rec = 0;
	Parameter.linesParameter.hv_Phi_rec = 0;
	Parameter.linesParameter.hv_Length1_rec = 0;
	Parameter.linesParameter.hv_Length2_rec = 0;
	//****目标区域1参数****
	Parameter.linesParameter.hv_Row_rec1 = 0;
	Parameter.linesParameter.hv_Column_rec1 = 0;
	Parameter.linesParameter.hv_Phi_rec1 = 0;
	Parameter.linesParameter.hv_Length1_rec1 = 0;
	Parameter.linesParameter.hv_Length2_rec1 = 0;
	//基准线
	Parameter.linesParameter.baseline = 0;

	//圆弧
	//****目标区域1参数****
	Parameter.circlesParameter.hv_Row_circle_out = 0;
	Parameter.circlesParameter.hv_Column_circle_out = 0;
	Parameter.circlesParameter.hv_Radius_circle_out = 0;
	Parameter.circlesParameter.hv_Row_circle_in = 0;
	Parameter.circlesParameter.hv_Column_circle_in = 0;
	Parameter.circlesParameter.hv_Radius_circle_in = 0;
	//****目标区域1参数****
	Parameter.circlesParameter.hv_Row_circle_out1 = 0;
	Parameter.circlesParameter.hv_Column_circle_out1 = 0;
	Parameter.circlesParameter.hv_Radius_circle_out1 = 0;
	Parameter.circlesParameter.hv_Row_circle_in1 = 0;
	Parameter.circlesParameter.hv_Column_circle_in1 = 0;
	Parameter.circlesParameter.hv_Radius_circle_in1 = 0;
	//基准点
	Parameter.circlesParameter.basepoint = 0;

	//矩形
	//****目标区域1参数****
	Parameter.rectsParameter.hv_Row_rectangle_out = 0;
	Parameter.rectsParameter.hv_Column_rectangle_out = 0;
	Parameter.rectsParameter.hv_Phi_rectangle_out = 0;
	Parameter.rectsParameter.hv_Length1_rectangle_out = 0;
	Parameter.rectsParameter.hv_Length2_rectangle_out = 0;
	Parameter.rectsParameter.hv_Row_rectangle_in = 0;
	Parameter.rectsParameter.hv_Column_rectangle_in = 0;
	Parameter.rectsParameter.hv_Phi_rectangle_in = 0;
	Parameter.rectsParameter.hv_Length1_rectangle_in = 0;
	Parameter.rectsParameter.hv_Length2_rectangle_in = 0;

	//单直线
	//****目标区域1参数****
	Parameter.lineParameter.hv_Row_rec = 0;
	Parameter.lineParameter.hv_Column_rec = 0;
	Parameter.lineParameter.hv_Phi_rec = 0;
	Parameter.lineParameter.hv_Length1_rec = 0;
	Parameter.lineParameter.hv_Length2_rec = 0;

	//实心圆匹配
	Parameter.circleParameter.hv_Row_circle_out = 0;
	Parameter.circleParameter.hv_Column_circle_out = 0;
	Parameter.circleParameter.hv_Radius_circle_out = 0;
	Parameter.circleParameter.hv_Row_circle_in = 0;
	Parameter.circleParameter.hv_Column_circle_in = 0;
	Parameter.circleParameter.hv_Radius_circle_in = 0;

	return AddModel(DownSetting, newModel, Parameter, L"down");
}

//删除模板
bool CalgorithmRegion::DeleteModel(CString Setting, CString model, ModelParameter &Parameter, CString dir)
{
	CMarkup xml;				//读写xml对象
	if (!xml.Load(Setting))
	{
		AfxMessageBox(L"找不到配置文件，请导入方程");
		return false;
	}
	xml.ResetPos();
	if(xml.FindChildElem(model))
	{
		xml.IntoElem();
		if(!xml.RemoveElem())
		{
			AfxMessageBox(L"模板删除失败");
			return false;
		}
		xml.ResetPos();
		xml.FindChildElem(_T("Program"));
		xml.IntoElem();
		xml.FindChildElem(_T("Selected"));
		xml.SetChildData(_T(""));
		if(!xml.Save(Setting))
		{
			AfxMessageBox(L"保存失败！");
		}
		//删除模板
		DeleteFileW(Parameter.model_Path);
		DeleteFileW(Parameter.model_Path+L".bmp");
	}
	return true;
}
bool CalgorithmRegion::DeleteUpModel(CString model, ModelParameter &Parameter)
{
	return DeleteModel(UpSetting, model, Parameter, L"up");
}	
bool CalgorithmRegion::DeleteDownModel(CString model, ModelParameter &Parameter)
{
	return DeleteModel(DownSetting, model, Parameter, L"down");
}

//贴合时计算结果
bool CalgorithmRegion::GetResult(vector<double> &parameters, vector<double> &results)
{
	try{
		// Local control variables 
		HTuple  hv_X_baseplate_corner, hv_Y_baseplate_corner;
		HTuple  hv_Angle_baseplate, hv_X_offset, hv_Y_offset, hv_Angle_offset;
		HTuple  hv_X_paster_corner, hv_Y_paster_corner, hv_Angle_paster;
		HTuple  hv_Distance_Mark2Nozzle, hv_Angle_Mark2Nozzle, hv_Angle_baseplate_offset;
		HTuple  hv_X_baseplate_corner_offset, hv_Y_baseplate_corner_offset;
		HTuple  hv_Angle_obj, hv_X_obj, hv_Y_obj;

		//******************************输入参数像素单位********************************************
		//底板数据（机械坐标系）
		hv_X_baseplate_corner = parameters[0];
		hv_Y_baseplate_corner = parameters[1];
		//底板基准线角度
		hv_Angle_baseplate = parameters[2];

		//底板偏移数据（以底板直角为坐标系）
		hv_X_offset = parameters[3];
		hv_Y_offset = parameters[4];
		hv_Angle_offset = parameters[5];

		//贴片数据（吸嘴中心坐标系）
		hv_X_paster_corner = parameters[6];
		hv_Y_paster_corner = parameters[7];
		//贴片基准线角度
		hv_Angle_paster = parameters[8];

		//*************计算贴片上Mark点到吸嘴中心的关系（默认吸嘴在图像中心）***************
		hv_Distance_Mark2Nozzle = ((hv_X_paster_corner*hv_X_paster_corner)+(hv_Y_paster_corner*hv_Y_paster_corner)).TupleSqrt();
		if (0 != (hv_Y_paster_corner==0))
		{
			if (0 != ((0-hv_X_paster_corner)>=0))
			{
				hv_Angle_Mark2Nozzle = 0-hv_Angle_paster;
			}
			else
			{
				hv_Angle_Mark2Nozzle = 180-hv_Angle_paster;
			}
		}
		else
		{
			hv_Angle_Mark2Nozzle = ((((((0-hv_X_paster_corner)/hv_Distance_Mark2Nozzle).TupleAcos()).TupleDeg())*(0-hv_Y_paster_corner))/((0-hv_Y_paster_corner).TupleAbs()))-hv_Angle_paster;
		}
		//*************************************************************


		//******************************计算过程***********************************
		//求偏移后的底板数据
		hv_Angle_baseplate_offset = hv_Angle_baseplate+hv_Angle_offset;
		//if(CompenationStyle==1)//以模板方向补偿
		//	{
		//	hv_X_baseplate_corner_offset = (hv_X_baseplate_corner+(hv_X_offset*((hv_Angle_baseplate.TupleRad()).TupleCos())))+(hv_Y_offset*(((hv_Angle_baseplate+90).TupleRad()).TupleCos()));
		//	hv_Y_baseplate_corner_offset = (hv_Y_baseplate_corner+(hv_X_offset*((hv_Angle_baseplate.TupleRad()).TupleSin())))+(hv_Y_offset*(((hv_Angle_baseplate+90).TupleRad()).TupleSin()));
		//	}
		//else {//以机械坐标补偿
		//	hv_X_baseplate_corner_offset = hv_X_baseplate_corner+hv_X_offset;
		//	hv_Y_baseplate_corner_offset = hv_Y_baseplate_corner+hv_Y_offset;
		//	}
		//求结果
		hv_Angle_obj = hv_Angle_baseplate_offset-hv_Angle_paster;
		hv_X_obj = hv_X_baseplate_corner_offset+(hv_Distance_Mark2Nozzle*(((hv_Angle_baseplate_offset+hv_Angle_Mark2Nozzle).TupleRad()).TupleCos()));
		hv_Y_obj = hv_Y_baseplate_corner_offset+(hv_Distance_Mark2Nozzle*(((hv_Angle_baseplate_offset+hv_Angle_Mark2Nozzle).TupleRad()).TupleSin()));
		//*******************************输出参数**********************************

		//目标位置
		hv_X_obj = hv_X_obj;
		hv_Y_obj = hv_Y_obj;
		//机械手要旋转的角度
		hv_Angle_obj = hv_Angle_obj;
		//****************************************************************************

		//吸嘴需要调整的XYR
		results.clear();
		results.push_back((double)hv_X_obj);
		results.push_back((double)hv_Y_obj);
		results.push_back((double)hv_Angle_obj);
	}catch(...)
	{
		return false;
	}
	return true;
}

//添加方案
bool CalgorithmRegion::AddPro(CString pro)
{
	try{
		this->pro = pro;
		CString stemp;
		stemp.Format(L"%s/CalgorithmRegion/up", this->pro);
		if(PathIsDirectory(stemp))   
		{
			return false;
		}
		stemp.Format(L"%s/CalgorithmRegion/up/Setting.xml", this->pro);
		UpSetting = CPublicTool::GetFilePath(stemp);
		stemp.Format(L"%s/CalgorithmRegion/down/Setting.xml", this->pro);
		DownSetting = CPublicTool::GetFilePath(stemp);

		CreateDirectory(pro + L"\\CalgorithmRegion",NULL);
		CreateDirectory(pro + L"\\CalgorithmRegion\\up",NULL);
		CreateDirectory(pro + L"\\CalgorithmRegion\\down",NULL);
	
		CMarkup xml;				//读写xml对象
		CString para;
		xml.AddElem(L"Setting");

		xml.AddChildElem(L"Program");
		xml.IntoElem();

		xml.AddChildElem(L"Selected");
		para.Format(L"%s", L"");
		xml.SetChildData(para);

		if(!xml.Save(UpSetting))
		{
			AfxMessageBox(L"保存失败！");
			return false;
		}

		if(!xml.Save(DownSetting))
		{
			AfxMessageBox(L"保存失败！");
			return false;
		}

	}catch(...)
	{
		return false;
	}
	return true;
}

//选择方案
bool CalgorithmRegion::SetPro(CString pro)
{
	try{
		this->pro = pro;
		CString stemp;
		stemp.Format(L"%s/CalgorithmRegion/up/Setting.xml", this->pro);
		UpSetting = CPublicTool::GetFilePath(stemp);
		stemp.Format(L"%s/CalgorithmRegion/down/Setting.xml", this->pro);
		DownSetting = CPublicTool::GetFilePath(stemp);

	}catch(...)
	{
		return false;
	}
	return true;
}

//保存图像
bool CalgorithmRegion::SaveImage(CString dirPath, HObject image)
{
	try{
		SYSTEMTIME systm;
		::GetLocalTime(&systm);	
		CString pathName;
		pathName = L"D:\\图像备份";
		//检查指定的目录是否存在，入不存在则创建
		if (!PathIsDirectory(pathName) )   
		{
			if (!CreateDirectory(pathName, NULL) )
			{
				AfxMessageBox(L"Create Expection folder failed");
			}
		}
		pathName.Format(L"D:\\图像备份\\%04d-%02d-%02d",systm.wYear,systm.wMonth,systm.wDay);
		//检查指定的目录是否存在，入不存在则创建
		if (!PathIsDirectory(pathName) )   
		{
			if (!CreateDirectory(pathName, NULL) )
			{
				AfxMessageBox(L"Create Expection folder failed");
			}
		}
		pathName.Format(L"D:\\图像备份\\%04d-%02d-%02d\\%s", systm.wYear,systm.wMonth,systm.wDay,dirPath);
		//检查指定的目录是否存在，入不存在则创建
		if (!PathIsDirectory(pathName) )   
		{
			if (!CreateDirectory(pathName, NULL) )
			{
				AfxMessageBox(L"Create Expection folder failed");
			}
		}
		pathName.Format(L"D:\\图像备份\\%04d-%02d-%02d\\%s\\%d_%d",
			systm.wYear,systm.wMonth,systm.wDay, 
			dirPath,
			systm.wSecond,
			systm.wMilliseconds);
		USES_CONVERSION; 
		char* str = W2A(pathName);
		//存图
		WriteImage(image, "bmp", 0, str);
	}
	catch(...)
	{
		AfxMessageBox(L"保存图像失败");
		return false;
	}
	return true;
}

//寻找吸嘴中心位置
bool CalgorithmRegion::FindCenterPoint(vector<HObject> ho_ImageVector, vector<double> &results,
	HalconTool &Tool, HTuple hv_Ahc, bool isSaveImage)
{
	// Local iconic variables 
	HObject  ho_EmptyRegion, ho_Image, ho_ImageMirror;
	HObject  ho_RotateImage, ho_ImageMedian, ho_FirstMultImage;
	HObject  ho_SecondMultImage, ho_ImageInvert, ho_MultRegion;
	HObject  ho_MultConnectedRegions, ho_MarkRegions, ho_MarkTransRegion;
	HObject  ho_MarkSelectedRegions, ho_Contours, ho_CenterPoint;
	HObject  ho_Region, ho_LocationRotatingShaftCenter, ho_Cross;
	HObject  ho_NewCross;

	// Local control variables 
	HTuple  hv_ManipulatorX, hv_ManipulatorY;
	HTuple  hv_Ratio, hv_RatioVector, hv_CenterRow_Vextor;
	HTuple  hv_CenterColumn_Vextor, hv_ImageFiles, hv_Index;
	HTuple  hv_Width, hv_Height, hv_WindowHandle, hv_CenterRow;
	HTuple  hv_CenterColumn, hv_Phi, hv_LongLength, hv_ShortLength;
	HTuple  hv_MarkNumber, hv_Radius;
	HTuple  hv_LocationRotatingShaftCenterRow, hv_LocationRotatingShaftCenterColumn;
	HTuple  hv_LocationRotatingShaftCenterRadius, hv_MeanRatio;
	HTuple  hv_X_shift, hv_Y_shift, hv_ManipulatorShiftX, hv_ManipulatorShiftY;

	//比例值
	hv_Ratio = hv_Ahc;
	hv_RatioVector = HTuple();

	hv_CenterRow_Vextor = HTuple();
	hv_CenterColumn_Vextor = HTuple();

	GenEmptyRegion(&ho_EmptyRegion);

	try{
		for (int i = 0; i< ho_ImageVector.size(); i++)
		{
			if(isSaveImage)
				SaveImage(L"FindCenterPoint", ho_ImageVector[i]);
			GetImageSize(ho_ImageVector[i], &hv_Width, &hv_Height);

			MedianImage(ho_ImageVector[i], &ho_ImageMedian, "circle", 3, "mirrored");
			//增强对比度
			MultImage(ho_ImageMedian, ho_ImageMedian, &ho_FirstMultImage, 0.05, -800);
			MultImage(ho_FirstMultImage, ho_FirstMultImage, &ho_SecondMultImage, 0.1, -800);

			//自动阈值提取区域
			InvertImage(ho_SecondMultImage, &ho_ImageInvert);
			BinThreshold(ho_ImageInvert, &ho_MultRegion);

			//选取Mark点区域
			Connection(ho_MultRegion, &ho_MultConnectedRegions);
			SelectShapeStd(ho_MultConnectedRegions, &ho_MarkRegions, "max_area", 70);

			//转换为外接矩形，并计算其中心坐标和角度
			ShapeTrans(ho_MarkRegions, &ho_MarkTransRegion, "rectangle2");
			SmallestRectangle2(ho_MarkTransRegion, &hv_CenterRow, &hv_CenterColumn, &hv_Phi, 
				&hv_LongLength, &hv_ShortLength);
			SelectShape(ho_MarkTransRegion, &ho_MarkSelectedRegions, "area", "and", 6000, 
				99999999);

			CountObj(ho_MarkSelectedRegions, &hv_MarkNumber);
			if (0 != (hv_MarkNumber==1))
			{

				GenContourRegionXld(ho_MarkRegions, &ho_Contours, "center");
				SmallestCircleXld(ho_Contours, &hv_CenterRow, &hv_CenterColumn, &hv_Radius);

				hv_RatioVector = hv_RatioVector.TupleConcat(hv_Ratio);
				//角度换算
				GenCrossContourXld(&ho_CenterPoint, hv_CenterRow, hv_CenterColumn, 50, hv_Phi);

				hv_CenterRow_Vextor = hv_CenterRow_Vextor.TupleConcat(hv_CenterRow);
				hv_CenterColumn_Vextor = hv_CenterColumn_Vextor.TupleConcat(hv_CenterColumn);

				GenRegionPoints(&ho_Region, hv_CenterRow, hv_CenterColumn);
				Union2(ho_EmptyRegion, ho_Region, &ho_EmptyRegion);

			}
			else
			{
				//AfxMessageBox(L"寻找吸嘴中心位置计算异常");
			}
		}
		SmallestCircle(ho_EmptyRegion, &hv_LocationRotatingShaftCenterRow, &hv_LocationRotatingShaftCenterColumn, 
			&hv_LocationRotatingShaftCenterRadius);
		GenCrossContourXld(&ho_LocationRotatingShaftCenter, hv_LocationRotatingShaftCenterRow, 
			hv_LocationRotatingShaftCenterColumn, 10, hv_Phi);

		TupleMean(hv_RatioVector, &hv_MeanRatio);
		GenCrossContourXld(&ho_Cross, hv_LocationRotatingShaftCenterRow, hv_LocationRotatingShaftCenterColumn, 50, HTuple(45).TupleRad());
		//坐标偏移量计算
		hv_X_shift = hv_LocationRotatingShaftCenterColumn-hv_Width/2;
		hv_Y_shift = hv_Height/2-hv_LocationRotatingShaftCenterRow;
		Tool.outlines.clear();
		Tool.outlines.push_back(ho_EmptyRegion);
		Tool.outlines.push_back(ho_Cross);

		results.clear();
		results.push_back((double)(hv_X_shift*hv_Ratio));
		results.push_back((double)(hv_Y_shift*hv_Ratio));
	}catch(...)
	{
		return false;
	}
	return true;
}

//计算相机比率
bool CalgorithmRegion::FindCamScale(vector<HObject> ho_ImageVector, vector<double> &results,
	CString Setting, ModelParameter &Parameter, CString modelName,
	HalconTool &Tool, HTuple hv_X_move, HTuple hv_Y_move)
{
	try{
		// Local iconic variables 
		HObject  ho_Image, ho_Regions1, ho_RegionClose;
		HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_SelectedRegions2;


		// Local control variables 
		HTuple  hv_ImageFiles, hv_Index;
		HTuple  hv_Number, hv_Area, hv_Row_point;
		HTuple  hv_Column_point, hv_Row_move, hv_Column_move, hv_Length;

		//*********************输入数据
		//hv_X_move = 5;
		//hv_Y_move = 5;
		//**************读图
		for (hv_Index=0; hv_Index<=1; hv_Index+=1)
		{			
			ho_Image = ho_ImageVector[(int)hv_Index];

			vector<double> modelResults;
			FindModel(Tool, ho_Image, modelResults, Setting, Parameter, modelName);
			//AutoThreshold(ho_Image, &ho_Regions1, 9);
			//ClosingCircle(ho_Regions1, &ho_RegionClose, 9);
			//Connection(ho_RegionClose, &ho_ConnectedRegions);
			//SelectShape(ho_Regions1, &ho_SelectedRegions, "area", "and", 1000, 10000);
			//SelectShape(ho_SelectedRegions, &ho_SelectedRegions2, "rectangularity", "and", 
			//	0.5, 1);
			//CountObj(ho_SelectedRegions2, &hv_Number);
			//if (0 != (hv_Number==1))
			//{
			//	AreaCenter(ho_SelectedRegions2, &hv_Area, &hv_Row, &hv_Column);
			//}
			hv_Row_point[hv_Index] = modelResults[5];
			hv_Column_point[hv_Index] = modelResults[4];
		}
		hv_Row_move = HTuple(hv_Row_point[1])-HTuple(hv_Row_point[0]);
		hv_Column_move = HTuple(hv_Column_point[1])-HTuple(hv_Column_point[0]);
		double a = (double)hv_Row_move;
		double b = (double)hv_Column_move;
		if (0 != (HTuple(hv_Row_move==0).TupleOr(hv_Column_move==0)))
		{
		}
		else
		{
			hv_Length = (((hv_X_move/hv_Row_move).TupleAbs())+((hv_Y_move/hv_Column_move).TupleAbs()))/2;
		}

		//****************输出
		hv_Length = hv_Length;
		results.clear();
		results.push_back((double)hv_Length);
	}
	catch(...)
	{
		AfxMessageBox(L"相机比率计算失败");
		return false;
	}
	return true;
}

//模拟贴合位置
bool CalgorithmRegion::DrawUponDown(HalconTool &ToolDown, HObject &scrImage, 
	vector<double> &parameters, ModelParameter &DownParameter, ModelParameter &UpParameter)
{
	try{
		if(parameters.size() < 6)
		{
			AfxMessageBox(L"模拟贴合失败！");
			return false;
		}
		HTuple  hv_Width, hv_Height;
		GetImageSize(scrImage, &hv_Width, &hv_Height);
		if(L"" == UpParameter.check)
		{
			// Local iconic variables 
			HObject  ho_Image, ho_Regions, ho_RegionLines;
			HObject  ho_RegionLines1, ho_RegionLines_paster, ho_RegionLines_paster1;

			// Local control variables 
			HTuple  hv_Length_pixel, hv_Length_pixel_paste, hv_Ratio_pixel;
			HTuple  hv_X_gravity_template_baseplate, hv_Y_gravity_template_baseplate;
			HTuple  hv_X_center_template_baseplate, hv_Y_center_template_baseplate;
			HTuple  hv_Angle_template_beseplate, hv_Angle_template_beseplate1;
			HTuple  hv_X_mm_offset, hv_Y_mm_offset, hv_X_offset, hv_Y_offset;
			HTuple  hv_Angle_offset, hv_HomMat2DIdentity, hv_HomMat2D_offset;
			HTuple  hv_X_offset_gravity_template_baseplate, hv_Y_offset_gravity_template_baseplate;
			HTuple  hv_Angle_offset_template_beseplate, hv_Angle_offset_template_beseplate1;
			HTuple  hv_HomMat2D_offset2, hv_X_offset_center_template_baseplate;
			HTuple  hv_Y_offset_center_template_baseplate, hv_HomMat2D_toRC;
			HTuple  hv_Row_gravity_template_baseplate, hv_Col_gravity_template_baseplate;
			HTuple  hv_Angle_rc_template_beseplate, hv_Angle_rc_template_beseplate1;
			HTuple  hv_Row_offset_gravity_template_baseplate, hv_Col_offset_gravity_template_baseplate;
			HTuple  hv_Angle_rc_offset_template_beseplate, hv_Angle_rc_offset_template_beseplate1;
			HTuple  hv_Row_offset_center_template_baseplate, hv_Col_offset_center_template_baseplate;
			HTuple  hv_Row_end, hv_Col_end, hv_Row_end1;
			HTuple  hv_Col_end1, hv_Row_end_paster, hv_Col_end_paster;
			HTuple  hv_Row_end_paster1, hv_Col_end_paster1;

			//******************************图像参数********************************************
			//底板图像单位像素长度(毫米）
			hv_Length_pixel = DownParameter.hv_Ahc;
			//贴片图像的单位像素长度
			hv_Length_pixel_paste = UpParameter.hv_Ahc;
			hv_Ratio_pixel = hv_Length_pixel_paste/hv_Length_pixel;
			//******************************输入参数（底板图像中心坐标系）********************************************

			//底板模板重心位置
			hv_X_gravity_template_baseplate = parameters[20];
			hv_Y_gravity_template_baseplate = parameters[21];
			//底板模板中心位置
			hv_X_center_template_baseplate = parameters[23];
			hv_Y_center_template_baseplate = parameters[24];
			//底板模板基准线1角度
			hv_Angle_template_beseplate = parameters[22];
			//底板基准线2角度
			hv_Angle_template_beseplate1 = hv_Angle_template_beseplate+90;

			//底板偏移数据
			hv_X_mm_offset = parameters[0];
			hv_Y_mm_offset = parameters[1];
			hv_X_offset = hv_X_mm_offset/hv_Length_pixel;
			hv_Y_offset = hv_Y_mm_offset/hv_Length_pixel;
			hv_Angle_offset = parameters[2];

			//******************************计算过程***********************************
			//计算偏移矩阵
			HomMat2dIdentity(&hv_HomMat2DIdentity);
			HomMat2dTranslate(hv_HomMat2DIdentity, (hv_X_offset*((hv_Angle_template_beseplate.TupleRad()).TupleCos()))+(hv_Y_offset*((hv_Angle_template_beseplate1.TupleRad()).TupleCos())), 
				(hv_X_offset*((hv_Angle_template_beseplate.TupleRad()).TupleSin()))+(hv_Y_offset*((hv_Angle_template_beseplate1.TupleRad()).TupleSin())), 
				&hv_HomMat2D_offset);
			//求偏移后的底板数据
			//偏移后的模板中心坐标和角度
			AffineTransPixel(hv_HomMat2D_offset, hv_X_gravity_template_baseplate, hv_Y_gravity_template_baseplate, 
				&hv_X_offset_gravity_template_baseplate, &hv_Y_offset_gravity_template_baseplate);
			hv_Angle_offset_template_beseplate = hv_Angle_template_beseplate+hv_Angle_offset;
			hv_Angle_offset_template_beseplate1 = hv_Angle_template_beseplate1+hv_Angle_offset;

			VectorAngleToRigid(hv_X_gravity_template_baseplate, hv_Y_gravity_template_baseplate, 
				hv_Angle_template_beseplate.TupleRad(), hv_X_offset_gravity_template_baseplate, 
				hv_Y_offset_gravity_template_baseplate, (hv_Angle_template_beseplate+hv_Angle_offset).TupleRad(), 
				&hv_HomMat2D_offset2);
			//求偏移后的模板中心坐标_
			AffineTransPixel(hv_HomMat2D_offset2, hv_X_center_template_baseplate, hv_Y_center_template_baseplate, 
				&hv_X_offset_center_template_baseplate, &hv_Y_offset_center_template_baseplate);


			//********************************显示需要的参数************************************
			//底板模板重心位置
			hv_X_gravity_template_baseplate = hv_X_gravity_template_baseplate;
			hv_Y_gravity_template_baseplate = hv_Y_gravity_template_baseplate;
			hv_Angle_template_beseplate = hv_Angle_template_beseplate;
			hv_Angle_template_beseplate1 = hv_Angle_template_beseplate1;


			//偏移后的模板重心的坐标
			hv_X_offset_gravity_template_baseplate = hv_X_offset_gravity_template_baseplate;
			hv_Y_offset_gravity_template_baseplate = hv_Y_offset_gravity_template_baseplate;
			hv_Angle_offset_template_beseplate = hv_Angle_offset_template_beseplate;
			hv_Angle_offset_template_beseplate1 = hv_Angle_offset_template_beseplate1;
			//偏移后的模板中心坐标
			hv_X_offset_center_template_baseplate = hv_X_offset_center_template_baseplate;
			hv_Y_offset_center_template_baseplate = hv_Y_offset_center_template_baseplate;

			//*********************************参数转化到图像坐标系*****************************************

			//图像中心坐标系到图像rc坐标系转换矩阵
			VectorAngleToRigid((-hv_Width)/2, hv_Height/2, -(HTuple(90).TupleRad()), 0, 0, 
				0, &hv_HomMat2D_toRC);
			//偏移前的模板重心数据
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_gravity_template_baseplate, hv_Y_gravity_template_baseplate, 
				&hv_Row_gravity_template_baseplate, &hv_Col_gravity_template_baseplate);
			hv_Angle_rc_template_beseplate = -hv_Angle_template_beseplate;
			hv_Angle_rc_template_beseplate1 = -hv_Angle_template_beseplate1;
			//偏移后的模板重心数据
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_offset_gravity_template_baseplate, hv_Y_offset_gravity_template_baseplate, 
				&hv_Row_offset_gravity_template_baseplate, &hv_Col_offset_gravity_template_baseplate);
			hv_Angle_rc_offset_template_beseplate = -hv_Angle_offset_template_beseplate;
			hv_Angle_rc_offset_template_beseplate1 = -hv_Angle_offset_template_beseplate1;

			//偏移后的模板中心坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_offset_center_template_baseplate, hv_Y_offset_center_template_baseplate, 
				&hv_Row_offset_center_template_baseplate, &hv_Col_offset_center_template_baseplate);
			hv_Row_offset_center_template_baseplate = hv_Row_offset_center_template_baseplate;
			hv_Col_offset_center_template_baseplate = hv_Col_offset_center_template_baseplate;
			//**************************************显示****************************************
			//底板模板重心直角显示
			hv_Row_end = hv_Row_gravity_template_baseplate+(100*((hv_Angle_rc_template_beseplate.TupleRad()).TupleSin()));
			hv_Col_end = hv_Col_gravity_template_baseplate+(100*((hv_Angle_rc_template_beseplate.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines, hv_Row_gravity_template_baseplate, hv_Col_gravity_template_baseplate, 
				hv_Row_end, hv_Col_end);
			hv_Row_end1 = hv_Row_gravity_template_baseplate+(100*((hv_Angle_rc_template_beseplate1.TupleRad()).TupleSin()));
			hv_Col_end1 = hv_Col_gravity_template_baseplate+(100*((hv_Angle_rc_template_beseplate1.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines1, hv_Row_gravity_template_baseplate, hv_Col_gravity_template_baseplate, 
				hv_Row_end1, hv_Col_end1);
			//显示模板
			ToolDown.uplines.clear();
			dev_display_shape_matchinresults(ToolDown.uplines, UpParameter.hv_ModelID, hv_Row_offset_center_template_baseplate, 
				hv_Col_offset_center_template_baseplate, (-hv_Angle_rc_offset_template_beseplate).TupleRad()-UpParameter.hv_Angle, 
				hv_Ratio_pixel, hv_Ratio_pixel, 0);
			//贴片模板重心直角显示
			hv_Row_end_paster = hv_Row_offset_gravity_template_baseplate+(50*((hv_Angle_rc_offset_template_beseplate.TupleRad()).TupleSin()));
			hv_Col_end_paster = hv_Col_offset_gravity_template_baseplate+(50*((hv_Angle_rc_offset_template_beseplate.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines_paster, hv_Row_offset_gravity_template_baseplate, 
				hv_Col_offset_gravity_template_baseplate, hv_Row_end_paster, hv_Col_end_paster);
			hv_Row_end_paster1 = hv_Row_offset_gravity_template_baseplate+(50*((hv_Angle_rc_offset_template_beseplate1.TupleRad()).TupleSin()));
			hv_Col_end_paster1 = hv_Col_offset_gravity_template_baseplate+(50*((hv_Angle_rc_offset_template_beseplate1.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines_paster1, hv_Row_offset_gravity_template_baseplate, 
				hv_Col_offset_gravity_template_baseplate, hv_Row_end_paster1, hv_Col_end_paster1);

			ToolDown.uplines.push_back(ho_RegionLines_paster);
			ToolDown.uplines.push_back(ho_RegionLines_paster1);
		}
		else if(L"Lines" == UpParameter.check)
		{
			// Local iconic variables 
			HObject  ho_Image, ho_Regions, ho_RegionLines;
			HObject  ho_RegionLines1, ho_RegionLines_offset, ho_RegionLines1_offset;

			// Local control variables 
			HTuple  hv_X_baseplate_corner;
			HTuple  hv_Y_baseplate_corner, hv_Angle_baseplate, hv_Angle_baseplate1;
			HTuple  hv_Length_pixel, hv_X_offset, hv_Y_offset, hv_Angle_offset;
			HTuple  hv_HomMat2DIdentity, hv_HomMat2D_offset, hv_X_baseplate_corner1;
			HTuple  hv_Y_baseplate_corner1, hv_Angle1_baseplate, hv_Angle1_baseplate1;
			HTuple  hv_HomMat2D_toRC, hv_Row_baseplate_corner, hv_Col_baseplate_corner;
			HTuple  hv_Row_baseplate_corner1, hv_Col_baseplate_corner1;
			HTuple  hv_Angle_rc_baseplate, hv_Angle_rc_baseplate1, hv_Angle1_rc_baseplate;
			HTuple  hv_Angle1_rc_baseplate1, hv_Row_baseplate_corner_end;
			HTuple  hv_Col_baseplate_corner_end, hv_Row_baseplate_corner_end1;
			HTuple  hv_Col_baseplate_corner_end1, hv_Row_baseplate_corner1_end;
			HTuple  hv_Col_baseplate_corner1_end, hv_Row_baseplate_corner1_end1;
			HTuple  hv_Col_baseplate_corner1_end1, hv_Row_baseplate_corner_start1, hv_Col_baseplate_corner_start1;

			//******************************输入参数像素单位********************************************
			//底板数据
			hv_X_baseplate_corner = parameters[20];
			hv_Y_baseplate_corner = parameters[21];
			//底板基准线1角度
			hv_Angle_baseplate = parameters[22];
			//底板基准线2角度
			hv_Angle_baseplate1 = parameters[23];
			//单位像素长度(毫米）
			hv_Length_pixel = DownParameter.hv_Ahc;
			//底板偏移数据（以底板直角为坐标系）
			hv_X_offset = parameters[0]/hv_Length_pixel;
			hv_Y_offset = parameters[1]/hv_Length_pixel;
			hv_Angle_offset = parameters[2];

			//******************************计算过程***********************************
			//计算偏移矩阵
			HomMat2dIdentity(&hv_HomMat2DIdentity);
			HomMat2dTranslate(hv_HomMat2DIdentity, (hv_X_offset*((hv_Angle_baseplate.TupleRad()).TupleCos()))+(hv_Y_offset*((hv_Angle_baseplate.TupleRad()+90).TupleCos())), 
				(hv_X_offset*((hv_Angle_baseplate.TupleRad()).TupleSin()))+(hv_Y_offset*((hv_Angle_baseplate.TupleRad()+90).TupleSin())), 
				&hv_HomMat2D_offset);
			//求偏移后的底板数据
			AffineTransPixel(hv_HomMat2D_offset, hv_X_baseplate_corner, hv_Y_baseplate_corner, 
				&hv_X_baseplate_corner1, &hv_Y_baseplate_corner1);
			hv_Angle1_baseplate = hv_Angle_baseplate+hv_Angle_offset;
			hv_Angle1_baseplate1 = hv_Angle_baseplate1+hv_Angle_offset;

			//*******************************输出参数**********************************
			//偏移后的底板数据
			hv_X_baseplate_corner1 = hv_X_baseplate_corner1;
			hv_Y_baseplate_corner1 = hv_Y_baseplate_corner1;
			hv_Angle1_baseplate = hv_Angle1_baseplate;
			hv_Angle1_baseplate1 = hv_Angle1_baseplate1;

			//******************************显示需要的参数**************************************
			//偏移前的底板数据
			hv_X_baseplate_corner = hv_X_baseplate_corner;
			hv_Y_baseplate_corner = hv_Y_baseplate_corner;
			hv_Angle_baseplate = hv_Angle_baseplate;
			hv_Angle_baseplate1 = hv_Angle_baseplate1;
			//偏移后的底板数据
			hv_X_baseplate_corner1 = hv_X_baseplate_corner1;
			hv_Y_baseplate_corner1 = hv_Y_baseplate_corner1;
			hv_Angle1_baseplate = hv_Angle1_baseplate;
			hv_Angle1_baseplate1 = hv_Angle1_baseplate1;

			//*********************************参数转化到图像坐标系*****************************************

			//图像中心坐标系到图像rc坐标系转换矩阵
			VectorAngleToRigid((-hv_Height)/2, hv_Width/2, -(HTuple(90).TupleRad()), 0, 0, 
				0, &hv_HomMat2D_toRC);
			//偏移前的基准点坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_baseplate_corner, hv_Y_baseplate_corner, 
				&hv_Row_baseplate_corner, &hv_Col_baseplate_corner);
			//偏移后的基准点坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_baseplate_corner1, hv_Y_baseplate_corner1, 
				&hv_Row_baseplate_corner1, &hv_Col_baseplate_corner1);
			//偏移前的基准线角度（角度制）
			hv_Angle_rc_baseplate = -hv_Angle_baseplate;
			hv_Angle_rc_baseplate1 = -hv_Angle_baseplate1;
			//偏移后的基准线角度（角度制
			hv_Angle1_rc_baseplate = -hv_Angle1_baseplate;
			hv_Angle1_rc_baseplate1 = -hv_Angle1_baseplate1;

			//**************************************显示****************************************
			//画未偏移的线段
			hv_Row_baseplate_corner_end = hv_Row_baseplate_corner+(100*((hv_Angle_rc_baseplate.TupleRad()).TupleSin()));
			hv_Col_baseplate_corner_end = hv_Col_baseplate_corner+(100*((hv_Angle_rc_baseplate.TupleRad()).TupleCos()));
			hv_Row_baseplate_corner_end1 = hv_Row_baseplate_corner+(100*((hv_Angle_rc_baseplate1.TupleRad()).TupleSin()));
			hv_Col_baseplate_corner_end1 = hv_Col_baseplate_corner+(100*((hv_Angle_rc_baseplate1.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines, hv_Row_baseplate_corner, hv_Col_baseplate_corner, 
				hv_Row_baseplate_corner_end, hv_Col_baseplate_corner_end);
			GenRegionLine(&ho_RegionLines1, hv_Row_baseplate_corner, hv_Col_baseplate_corner, 
				hv_Row_baseplate_corner_end1, hv_Col_baseplate_corner_end1);

			//画偏移的线段
			hv_Row_baseplate_corner1_end = hv_Row_baseplate_corner1+(100*((hv_Angle1_rc_baseplate.TupleRad()).TupleSin()));
			hv_Col_baseplate_corner1_end = hv_Col_baseplate_corner1+(100*((hv_Angle1_rc_baseplate.TupleRad()).TupleCos()));

			//hv_Row_baseplate_corner_start1 = hv_Row_baseplate_corner-(100*((hv_Angle_rc_baseplate1.TupleRad()).TupleSin()));
			//hv_Col_baseplate_corner_start1 = hv_Col_baseplate_corner-(100*((hv_Angle_rc_baseplate1.TupleRad()).TupleCos()));
			hv_Row_baseplate_corner1_end1 = hv_Row_baseplate_corner1+(100*((hv_Angle1_rc_baseplate1.TupleRad()).TupleSin()));
			hv_Col_baseplate_corner1_end1 = hv_Col_baseplate_corner1+(100*((hv_Angle1_rc_baseplate1.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines_offset, hv_Row_baseplate_corner1, hv_Col_baseplate_corner1, 
				hv_Row_baseplate_corner1_end, hv_Col_baseplate_corner1_end);
			GenRegionLine(&ho_RegionLines1_offset, hv_Row_baseplate_corner1, hv_Col_baseplate_corner1, 
				hv_Row_baseplate_corner1_end1, hv_Col_baseplate_corner1_end1);

			//贴片
			ToolDown.uplines.clear();
			ToolDown.uplines.push_back(ho_RegionLines_offset);
			ToolDown.uplines.push_back(ho_RegionLines1_offset);
		}
		else if(L"Circles" == UpParameter.check)
		{
			// Local iconic variables 
			HObject  ho_Image, ho_Regions, ho_Circle, ho_Circle1;
			HObject  ho_RegionLines, ho_RegionLines1, ho_RegionLines_offset, ho_RegionLines_offset1;
			HObject  ho_Circle_offset, ho_Circle_offset1;

			// Local control variables 
			HTuple  hv_X_baseplate_center_circle;
			HTuple  hv_Y_baseplate_center_circle, hv_X_baseplate_center_circle1;
			HTuple  hv_Y_baseplate_center_circle1, hv_Radius_circle1;
			HTuple  hv_Angle_circles, hv_Angle_baseplate, hv_Angle_baseplate1;
			HTuple  hv_Length_pixel, hv_X_offset, hv_Y_offset, hv_Angle_offset;
			HTuple  hv_HomMat2DIdentity, hv_HomMat2D_offset, hv_X_baseplate_center_offset_circle;
			HTuple  hv_Y_baseplate_center_offset_circle, hv_X_baseplate_center_offset_circle1;
			HTuple  hv_Y_baseplate_center_offset_circle1, hv_Angle1_baseplate;
			HTuple  hv_Angle1_baseplate1, hv_Radius_circle, hv_Distance_circles;
			HTuple  hv_HomMat2D_toRC, hv_Row_baseplate_center_circle;
			HTuple  hv_Col_baseplate_center_circle, hv_Row_baseplate_center_circle1;
			HTuple  hv_Col_baseplate_center_circle1, hv_Row_baseplate_center_offset_circle;
			HTuple  hv_Col_baseplate_center_offset_circle, hv_Row_baseplate_center_offset_circle1;
			HTuple  hv_Col_baseplate_center_offset_circle1, hv_Angle_rc_baseplate;
			HTuple  hv_Angle_rc_baseplate1, hv_Angle1_rc_baseplate;
			HTuple  hv_Angle1_rc_baseplate1, hv_Radius_rc_circle, hv_Radius_rc_circle1;
			HTuple  hv_Distance_rc_circles, hv_Row_end, hv_Col_end;
			HTuple  hv_Row_end1, hv_Col_end1, hv_Row1_end, hv_Col1_end;
			HTuple  hv_Row1_end1, hv_Col1_end1;

			//******************************输入参数像素单位********************************************
			//底板数据
			//单位像素长度(毫米）
			hv_Length_pixel = DownParameter.hv_Ahc;

			//基准圆1的半径和圆心坐标
			hv_X_baseplate_center_circle = parameters[20];
			hv_Y_baseplate_center_circle = parameters[21];
			hv_Radius_circle = parameters[23]/hv_Length_pixel;

			//圆2的半径和圆心坐标
			hv_X_baseplate_center_circle1 = parameters[24];
			hv_Y_baseplate_center_circle1 = parameters[25];
			hv_Radius_circle1 = parameters[26]/hv_Length_pixel;

			//底板基准线1角度
			if (0 != ((hv_X_baseplate_center_circle1-hv_X_baseplate_center_circle)==0))
			{
				hv_Angle_baseplate = (90*(hv_Y_baseplate_center_circle1-hv_Y_baseplate_center_circle))/((hv_Y_baseplate_center_circle1-hv_Y_baseplate_center_circle).TupleAbs());
			}
			else if (0 != ((hv_X_baseplate_center_circle1-hv_X_baseplate_center_circle)<0))
			{
				hv_Angle_baseplate = ((((hv_Y_baseplate_center_circle1-hv_Y_baseplate_center_circle)/(hv_X_baseplate_center_circle1-hv_X_baseplate_center_circle)).TupleAtan()).TupleDeg())+180;
			}
			else
			{
				hv_Angle_baseplate = (((hv_Y_baseplate_center_circle1-hv_Y_baseplate_center_circle)/(hv_X_baseplate_center_circle1-hv_X_baseplate_center_circle)).TupleAtan()).TupleDeg();
			}

			//底板基准线2角度
			hv_Angle_baseplate1 = hv_Angle_baseplate+90;

			//底板偏移数据（以底板直角为坐标系）
			hv_X_offset = parameters[0]/hv_Length_pixel;
			hv_Y_offset = parameters[1]/hv_Length_pixel;
			hv_Angle_offset = parameters[2];

			//******************************计算过程***********************************
			//计算偏移矩阵
			HomMat2dIdentity(&hv_HomMat2DIdentity);
			HomMat2dTranslate(hv_HomMat2DIdentity, (hv_X_offset*((hv_Angle_baseplate.TupleRad()).TupleCos()))+(hv_Y_offset*((hv_Angle_baseplate1.TupleRad()).TupleCos())), 
				(hv_X_offset*((hv_Angle_baseplate.TupleRad()).TupleSin()))+(hv_Y_offset*((hv_Angle_baseplate1.TupleRad()).TupleSin())), 
				&hv_HomMat2D_offset);
			//求偏移后的底板数据
			//偏移后的基准圆的圆心坐标
			AffineTransPixel(hv_HomMat2D_offset, hv_X_baseplate_center_circle, hv_Y_baseplate_center_circle, 
				&hv_X_baseplate_center_offset_circle, &hv_Y_baseplate_center_offset_circle);
			//偏移后的圆2的圆心坐标
			AffineTransPixel(hv_HomMat2D_offset, hv_X_baseplate_center_circle1, hv_Y_baseplate_center_circle1, 
				&hv_X_baseplate_center_offset_circle1, &hv_Y_baseplate_center_offset_circle1);

			hv_Angle1_baseplate = hv_Angle_baseplate+hv_Angle_offset;
			hv_Angle1_baseplate1 = hv_Angle_baseplate1+hv_Angle_offset;

			//*******************************输出参数**********************************
			//偏移后的基准圆的圆心坐标
			hv_X_baseplate_center_offset_circle = hv_X_baseplate_center_offset_circle;
			hv_Y_baseplate_center_offset_circle = hv_Y_baseplate_center_offset_circle;
			//偏移后的圆2的圆心坐标
			hv_X_baseplate_center_offset_circle1 = hv_X_baseplate_center_offset_circle1;
			hv_Y_baseplate_center_offset_circle1 = hv_Y_baseplate_center_offset_circle1;
			//偏移后的角度
			hv_Angle1_baseplate = hv_Angle1_baseplate;
			hv_Angle1_baseplate1 = hv_Angle1_baseplate1;

			//*********************************显示需要的参数************************************
			//偏移前的基准圆的圆心坐标
			hv_X_baseplate_center_circle = hv_X_baseplate_center_circle;
			hv_Y_baseplate_center_circle = hv_Y_baseplate_center_circle;
			//偏移前的圆2的圆心坐标
			hv_X_baseplate_center_circle1 = hv_X_baseplate_center_circle1;
			hv_Y_baseplate_center_circle1 = hv_Y_baseplate_center_circle1;
			//偏移后的角度
			hv_Angle_baseplate = hv_Angle_baseplate;
			hv_Angle_baseplate1 = hv_Angle_baseplate1;
			//偏移后的基准圆的圆心坐标
			hv_X_baseplate_center_offset_circle = hv_X_baseplate_center_offset_circle;
			hv_Y_baseplate_center_offset_circle = hv_Y_baseplate_center_offset_circle;
			//偏移后的圆2的圆心坐标
			hv_X_baseplate_center_offset_circle1 = hv_X_baseplate_center_offset_circle1;
			hv_Y_baseplate_center_offset_circle1 = hv_Y_baseplate_center_offset_circle1;
			//偏移后的角度
			hv_Angle1_baseplate = hv_Angle1_baseplate;
			hv_Angle1_baseplate1 = hv_Angle1_baseplate1;

			//两圆心距离
			DistancePp(hv_X_baseplate_center_circle, hv_Y_baseplate_center_circle, hv_X_baseplate_center_circle1, 
				hv_Y_baseplate_center_circle1, &hv_Distance_circles);
			//*********************************参数转化到图像坐标系*****************************************
			//单位像素长度(毫米）
			hv_Length_pixel = 1;
			//图像中心坐标系到图像rc坐标系转换矩阵
			VectorAngleToRigid((-hv_Height)/2, hv_Width/2, -(HTuple(90).TupleRad()), 0, 0, 
				0, &hv_HomMat2D_toRC);
			//偏移前的基准圆圆心坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_baseplate_center_circle, hv_Y_baseplate_center_circle, 
				&hv_Row_baseplate_center_circle, &hv_Col_baseplate_center_circle);
			//偏移前的圆2圆心坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_baseplate_center_circle1, hv_Y_baseplate_center_circle1, 
				&hv_Row_baseplate_center_circle1, &hv_Col_baseplate_center_circle1);
			//偏移后的基准圆圆心坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_baseplate_center_offset_circle, hv_Y_baseplate_center_offset_circle, 
				&hv_Row_baseplate_center_offset_circle, &hv_Col_baseplate_center_offset_circle);
			//偏移后的圆2圆心坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_baseplate_center_offset_circle1, hv_Y_baseplate_center_offset_circle1, 
				&hv_Row_baseplate_center_offset_circle1, &hv_Col_baseplate_center_offset_circle1);
			//偏移前的基准线角度（角度制）
			hv_Angle_rc_baseplate = -hv_Angle_baseplate;
			hv_Angle_rc_baseplate1 = -hv_Angle_baseplate1;
			//偏移后的基准线角度（角度制
			hv_Angle1_rc_baseplate = -hv_Angle1_baseplate;
			hv_Angle1_rc_baseplate1 = -hv_Angle1_baseplate1;
			//基准圆半径
			hv_Radius_rc_circle = hv_Radius_circle;
			//圆2半径
			hv_Radius_rc_circle1 = hv_Radius_circle1;
			//两圆心距离
			hv_Distance_rc_circles = hv_Distance_circles;
			//**************************************显示****************************************

			//画未偏移的线段
			hv_Row_end = hv_Row_baseplate_center_circle+(hv_Distance_circles*((hv_Angle_rc_baseplate.TupleRad()).TupleSin()));
			hv_Col_end = hv_Col_baseplate_center_circle+(hv_Distance_circles*((hv_Angle_rc_baseplate.TupleRad()).TupleCos()));
			hv_Row_end1 = hv_Row_baseplate_center_circle+(hv_Distance_circles*((hv_Angle_rc_baseplate1.TupleRad()).TupleSin()));
			hv_Col_end1 = hv_Col_baseplate_center_circle+(hv_Distance_circles*((hv_Angle_rc_baseplate1.TupleRad()).TupleCos()));
			GenCircle(&ho_Circle, hv_Row_baseplate_center_circle, hv_Col_baseplate_center_circle, 
				hv_Radius_rc_circle);
			GenCircle(&ho_Circle1, hv_Row_end, hv_Col_end, hv_Radius_rc_circle1);
			GenRegionLine(&ho_RegionLines, hv_Row_baseplate_center_circle, hv_Col_baseplate_center_circle, 
				hv_Row_end, hv_Col_end);
			GenRegionLine(&ho_RegionLines1, hv_Row_baseplate_center_circle, hv_Col_baseplate_center_circle, 
				hv_Row_end1, hv_Col_end1);


			//画偏移的线
			hv_Row1_end = hv_Row_baseplate_center_offset_circle+(hv_Distance_circles*((hv_Angle1_rc_baseplate.TupleRad()).TupleSin()));
			hv_Col1_end = hv_Col_baseplate_center_offset_circle+(hv_Distance_circles*((hv_Angle1_rc_baseplate.TupleRad()).TupleCos()));
			hv_Row1_end1 = hv_Row_baseplate_center_offset_circle+(100*((hv_Angle1_rc_baseplate1.TupleRad()).TupleSin()));
			hv_Col1_end1 = hv_Col_baseplate_center_offset_circle+(100*((hv_Angle1_rc_baseplate1.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines_offset, hv_Row_baseplate_center_offset_circle, hv_Col_baseplate_center_offset_circle, 
				hv_Row1_end, hv_Col1_end);
			GenRegionLine(&ho_RegionLines_offset1, hv_Row_baseplate_center_offset_circle, hv_Col_baseplate_center_offset_circle, 
				hv_Row1_end1, hv_Col1_end1);
			GenCircle(&ho_Circle_offset, hv_Row_baseplate_center_offset_circle, hv_Col_baseplate_center_offset_circle, 
				hv_Radius_rc_circle);
			GenCircle(&ho_Circle_offset1, hv_Row1_end, hv_Col1_end, hv_Radius_rc_circle1);

			//贴片
			ToolDown.uplines.clear();
			ToolDown.uplines.push_back(ho_Circle_offset);
			ToolDown.uplines.push_back(ho_RegionLines_offset);
			ToolDown.uplines.push_back(ho_RegionLines_offset1);
		}
		else if(L"Rects" == UpParameter.check)
		{
			// Local iconic variables 
			HObject  ho_Regions, ho_Regions1, ho_Rectangle;
			HObject  ho_RegionLines, ho_RegionLines1, ho_Rectangle_offset;
			HObject  ho_RegionLines_offset, ho_RegionLines_offset1;


			// Local control variables 
			HTuple  hv_X_baseplate_center_rec;
			HTuple  hv_Y_baseplate_center_rec, hv_Angle_beseplate_rec;
			HTuple  hv_Angle_beseplate_rec1, hv_X_offset, hv_Y_offset;
			HTuple  hv_Angle_offset, hv_HomMat2DIdentity, hv_HomMat2D_offset;
			HTuple  hv_X_baseplate_center_offset_rec, hv_Y_baseplate_center_offset_rec;
			HTuple  hv_Angle_beseplate_offset_rec, hv_Angle_beseplate_offset_rec1;
			HTuple  hv_Length1_rec, hv_Length2_rec, hv_Length_pixel;
			HTuple  hv_HomMat2D_toRC, hv_Row_baseplate_center_rec, hv_Col_baseplate_center_rec;
			HTuple  hv_Row_baseplate_center_offset_rec, hv_Col_baseplate_center_offset_rec;
			HTuple  hv_Angle_rc_baseplate_rec, hv_Angle_rc_baseplate_rec1;
			HTuple  hv_Angle_rc_baseplate_offset_rec, hv_Angle_rc_baseplate_offset_rec1;
			HTuple  hv_Length1_rc_rec, hv_Length2_rc_rec, hv_WindowHandle;
			HTuple  hv_Row_end, hv_Col_end, hv_Row_end1, hv_Col_end1;
			HTuple  hv_Row_offset_end, hv_Col_offset_end, hv_Row_offset_end1;
			HTuple  hv_Col_offset_end1, hv_Length1_rec_offset, hv_Length2_rec_offset;


			//******************************输入参数像素单位********************************************						
			//单位像素长度(毫米）
			hv_Length_pixel = DownParameter.hv_Ahc;
			//底板矩形长短半长轴(像素)
			hv_Length1_rec = parameters[23]/hv_Length_pixel;
			hv_Length2_rec = parameters[24]/hv_Length_pixel;
			//贴片矩形长短半长轴(像素)
			hv_Length1_rec_offset = parameters[13]/hv_Length_pixel;
			hv_Length2_rec_offset = parameters[14]/hv_Length_pixel;

			//底板偏移数据（以底板直角为坐标系）(像素)
			hv_X_offset = parameters[0]/hv_Length_pixel;
			hv_Y_offset = parameters[1]/hv_Length_pixel;
			hv_Angle_offset = parameters[2];

			//底板数据
			//矩形中心位置(像素)
			hv_X_baseplate_center_rec = parameters[20];
			hv_Y_baseplate_center_rec = parameters[21];
			//底板基准线1角度
			hv_Angle_beseplate_rec = parameters[22];
			//底板基准线2角度
			hv_Angle_beseplate_rec1 = hv_Angle_beseplate_rec+90;

			//******************************计算过程***********************************
			//计算偏移矩阵
			HomMat2dIdentity(&hv_HomMat2DIdentity);
			HomMat2dTranslate(hv_HomMat2DIdentity, (hv_X_offset*((hv_Angle_beseplate_rec.TupleRad()).TupleCos()))+(hv_Y_offset*((hv_Angle_beseplate_rec1.TupleRad()).TupleCos())), 
				(hv_X_offset*((hv_Angle_beseplate_rec.TupleRad()).TupleSin()))+(hv_Y_offset*((hv_Angle_beseplate_rec1.TupleRad()).TupleSin())), 
				&hv_HomMat2D_offset);
			//求偏移后的底板数据
			//偏移后的矩形中心坐标和角度
			AffineTransPixel(hv_HomMat2D_offset, hv_X_baseplate_center_rec, hv_Y_baseplate_center_rec, 
				&hv_X_baseplate_center_offset_rec, &hv_Y_baseplate_center_offset_rec);
			hv_Angle_beseplate_offset_rec = hv_Angle_beseplate_rec+hv_Angle_offset;
			hv_Angle_beseplate_offset_rec1 = hv_Angle_beseplate_rec1+hv_Angle_offset;


			//*******************************输出参数**********************************
			//偏移后的矩形中心的坐标
			hv_X_baseplate_center_offset_rec = hv_X_baseplate_center_offset_rec;
			hv_Y_baseplate_center_offset_rec = hv_Y_baseplate_center_offset_rec;
			hv_Angle_beseplate_offset_rec = hv_Angle_beseplate_offset_rec;
			hv_Angle_beseplate_offset_rec1 = hv_Angle_beseplate_offset_rec1;

			//********************************显示需要的参数************************************
			//偏移前的矩形中心位置
			hv_X_baseplate_center_rec = hv_X_baseplate_center_rec;
			hv_Y_baseplate_center_rec = hv_Y_baseplate_center_rec;
			hv_Angle_beseplate_rec = hv_Angle_beseplate_rec;
			hv_Angle_beseplate_rec1 = hv_Angle_beseplate_rec1;
			//偏移后的矩形中心的坐标
			hv_X_baseplate_center_offset_rec = hv_X_baseplate_center_offset_rec;
			hv_Y_baseplate_center_offset_rec = hv_Y_baseplate_center_offset_rec;
			hv_Angle_beseplate_offset_rec = hv_Angle_beseplate_offset_rec;
			hv_Angle_beseplate_offset_rec1 = hv_Angle_beseplate_offset_rec1;


			//*********************************参数转化到图像坐标系*****************************************

			//图像中心坐标系到图像rc坐标系转换矩阵
			VectorAngleToRigid((-hv_Width)/2, hv_Height/2, -(HTuple(90).TupleRad()), 
				0, 0, 0, &hv_HomMat2D_toRC);
			//偏移前的矩形中心坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_baseplate_center_rec, hv_Y_baseplate_center_rec, 
				&hv_Row_baseplate_center_rec, &hv_Col_baseplate_center_rec);
			//偏移后的矩形中心坐标
			AffineTransPixel(hv_HomMat2D_toRC, hv_X_baseplate_center_offset_rec, hv_Y_baseplate_center_offset_rec, 
				&hv_Row_baseplate_center_offset_rec, &hv_Col_baseplate_center_offset_rec);
			//偏移前的基准线角度（角度制）
			hv_Angle_rc_baseplate_rec = -hv_Angle_beseplate_rec;
			hv_Angle_rc_baseplate_rec1 = -hv_Angle_beseplate_rec1;
			//偏移后的基准线角度（角度制
			hv_Angle_rc_baseplate_offset_rec = -hv_Angle_beseplate_offset_rec;
			hv_Angle_rc_baseplate_offset_rec1 = -hv_Angle_beseplate_offset_rec1;
			//矩形半长轴
			hv_Length1_rc_rec = hv_Length1_rec;
			hv_Length2_rc_rec = hv_Length2_rec;
			//**************************************显示****************************************
			//画未偏移的矩形和
			GenRectangle2(&ho_Rectangle, hv_Row_baseplate_center_rec, hv_Col_baseplate_center_rec, 
				(-hv_Angle_rc_baseplate_rec).TupleRad(), hv_Length1_rc_rec, hv_Length2_rc_rec);
			hv_Row_end = hv_Row_baseplate_center_rec+(hv_Length1_rc_rec*((hv_Angle_rc_baseplate_rec.TupleRad()).TupleSin()));
			hv_Col_end = hv_Col_baseplate_center_rec+(hv_Length1_rc_rec*((hv_Angle_rc_baseplate_rec.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines, hv_Row_baseplate_center_rec, hv_Col_baseplate_center_rec, 
				hv_Row_end, hv_Col_end);
			hv_Row_end1 = hv_Row_baseplate_center_rec+(hv_Length2_rc_rec*((hv_Angle_rc_baseplate_rec1.TupleRad()).TupleSin()));
			hv_Col_end1 = hv_Col_baseplate_center_rec+(hv_Length2_rc_rec*((hv_Angle_rc_baseplate_rec1.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines1, hv_Row_baseplate_center_rec, hv_Col_baseplate_center_rec, 
				hv_Row_end1, hv_Col_end1);

			//画偏移的矩形和
			GenRectangle2(&ho_Rectangle_offset, hv_Row_baseplate_center_offset_rec, hv_Col_baseplate_center_offset_rec, 
				(-hv_Angle_rc_baseplate_offset_rec).TupleRad(), hv_Length1_rec_offset, hv_Length2_rec_offset);
			hv_Row_offset_end = hv_Row_baseplate_center_offset_rec+(hv_Length1_rec_offset*((hv_Angle_rc_baseplate_offset_rec.TupleRad()).TupleSin()));
			hv_Col_offset_end = hv_Col_baseplate_center_offset_rec+(hv_Length1_rec_offset*((hv_Angle_rc_baseplate_offset_rec.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines_offset, hv_Row_baseplate_center_offset_rec, hv_Col_baseplate_center_offset_rec, 
				hv_Row_offset_end, hv_Col_offset_end);
			hv_Row_offset_end1 = hv_Row_baseplate_center_offset_rec+(hv_Length2_rec_offset*((hv_Angle_rc_baseplate_offset_rec1.TupleRad()).TupleSin()));
			hv_Col_offset_end1 = hv_Col_baseplate_center_offset_rec+(hv_Length2_rec_offset*((hv_Angle_rc_baseplate_offset_rec1.TupleRad()).TupleCos()));
			GenRegionLine(&ho_RegionLines_offset1, hv_Row_baseplate_center_offset_rec, hv_Col_baseplate_center_offset_rec, 
				hv_Row_offset_end1, hv_Col_offset_end1);

			//ToolDown.outlines.clear();		
			
			//贴片
			ToolDown.uplines.clear();
			ToolDown.uplines.push_back(ho_Rectangle_offset);
			ToolDown.uplines.push_back(ho_RegionLines_offset);

			//贴片没偏差
			//ToolDown.downlines.clear();
			//ToolDown.downlines.push_back(ho_Rectangle);
			//ToolDown.downlines.push_back(ho_RegionLines);
		}
	}catch(...)
	{
		AfxMessageBox(L"模拟贴合失败，请重新编辑底板贴片模板");
		return false;
	}
	return true;
}

//显示模板
void CalgorithmRegion::dev_display_shape_matchinresults (vector<HObject> &outlines, 
			HTuple hv_ModelID,
			HTuple hv_Row, HTuple hv_Column, HTuple hv_Angle, 
			HTuple hv_ScaleR, HTuple hv_ScaleC, HTuple hv_Model)
{
	// Local iconic variables 
  HObject  ho_ModelContours, ho_ContoursAffinTrans;


  // Local control variables 
  HTuple  hv_NumMatches, hv_Index, hv_Match, hv_HomMat2DIdentity;
  HTuple  hv_HomMat2DScale, hv_HomMat2DRotate, hv_HomMat2DTranslate;

  //This procedure displays the results of Shape-Based Matching.
  //
  hv_NumMatches = hv_Row.TupleLength();
  if (0 != (hv_NumMatches>0))
  {
    if (0 != ((hv_ScaleR.TupleLength())==1))
    {
      TupleGenConst(hv_NumMatches, hv_ScaleR, &hv_ScaleR);
    }
    if (0 != ((hv_ScaleC.TupleLength())==1))
    {
      TupleGenConst(hv_NumMatches, hv_ScaleC, &hv_ScaleC);
    }
    if (0 != ((hv_Model.TupleLength())==0))
    {
      TupleGenConst(hv_NumMatches, 0, &hv_Model);
    }
    else if (0 != ((hv_Model.TupleLength())==1))
    {
      TupleGenConst(hv_NumMatches, hv_Model, &hv_Model);
    }
    HTuple end_val15 = (hv_ModelID.TupleLength())-1;
    HTuple step_val15 = 1;
    for (hv_Index=0; hv_Index.Continue(end_val15, step_val15); hv_Index += step_val15)
    {
      GetShapeModelContours(&ho_ModelContours, HTuple(hv_ModelID[hv_Index]), 1);
      HTuple end_val18 = hv_NumMatches-1;
      HTuple step_val18 = 1;
      for (hv_Match=0; hv_Match.Continue(end_val18, step_val18); hv_Match += step_val18)
      {
        if (0 != (hv_Index==HTuple(hv_Model[hv_Match])))
        {
          HomMat2dIdentity(&hv_HomMat2DIdentity);
          HomMat2dScale(hv_HomMat2DIdentity, HTuple(hv_ScaleR[hv_Match]), HTuple(hv_ScaleC[hv_Match]), 
              0, 0, &hv_HomMat2DScale);
          HomMat2dRotate(hv_HomMat2DScale, HTuple(hv_Angle[hv_Match]), 0, 0, &hv_HomMat2DRotate);
          HomMat2dTranslate(hv_HomMat2DRotate, HTuple(hv_Row[hv_Match]), HTuple(hv_Column[hv_Match]), 
              &hv_HomMat2DTranslate);
          AffineTransContourXld(ho_ModelContours, &ho_ContoursAffinTrans, hv_HomMat2DTranslate);

		  outlines.push_back(ho_ContoursAffinTrans);
        }
      }
    }
  }
  return;
}

//0==mode当前模板、1==mode还原模板
bool CalgorithmRegion::RestoreModel(HalconTool &Tool, HObject &scrImage, 
	HObject &ho_Rectangle, HObject &ho_Rectangle_model, 
	HObject &ho_RubberRectangle, HObject &ho_Region_xld, int mode, 
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	HObject ho_RegionLines, ho_RegionLines1, ho_Region_gravity;//在重心画十字线
	HObject ho_ContoursAffinTrans, ho_SortedContours, ho_ObjectSelected, ho_Region_selected, ho_ModelContours;//模板轮廓

	HTuple hv_Row, hv_Column, hv_Angle, hv_Score;//模板
	HTuple hv_HomMat2D, hv_RowTrans, hv_ColTrans, hv_Row_gravity, hv_Column_gravity;//映射
	HTuple hv_Number_xld, hv_i, hv_Row_obj, hv_Col_obj;
	
	try{		
 		GenEmptyRegion(&ho_RubberRectangle);		
		//图像预处理
		dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
		Tool.ShowImage(dstImage);

		FindShapeModel(dstImage, Parameter.hv_ModelID, HTuple(Parameter.hv_AStart).TupleRad(), HTuple(Parameter.hv_ARange).TupleRad(), 
			0.1, 1, 0, "least_squares", 0, 0.1, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
		
		if((double)hv_Score*100 >= Parameter.hv_Score)
		{
			VectorAngleToRigid(Parameter.hv_Row, Parameter.hv_Column, Parameter.hv_Angle, hv_Row, hv_Column,  Parameter.hv_Angle+hv_Angle, &hv_HomMat2D);
			//矩形中心
			AffineTransPixel(hv_HomMat2D, Parameter.hv_Row_center_rec_model, Parameter.hv_Column_center_rec_model,
				&hv_RowTrans, &hv_ColTrans);
			//重心
			AffineTransPixel(hv_HomMat2D, Parameter.hv_Row_gravity_model, Parameter.hv_Column_gravity_model, 
				&hv_Row_gravity, &hv_Column_gravity);
			//在重心画十字线
			GenRegionLine(&ho_RegionLines, hv_Row_gravity+8, hv_Column_gravity, 
				hv_Row_gravity-8, hv_Column_gravity);
			GenRegionLine(&ho_RegionLines1, hv_Row_gravity, hv_Column_gravity-8, 
				hv_Row_gravity, hv_Column_gravity+8);
			Union2(ho_RegionLines, ho_RegionLines1, &ho_Region_gravity);

			//保存未擦除的模板数据
			GenRectangle2(&ho_Rectangle, hv_RowTrans, hv_ColTrans, 
				Parameter.hv_Angle+hv_Angle, Parameter.hv_Length1_rec_model, Parameter.hv_Length2_rec_model);
			//ho_Rectangle_model:擦除后的模板区域变量
			GenRectangle2(&ho_Rectangle_model, hv_RowTrans, hv_ColTrans, 
				Parameter.hv_Angle+hv_Angle, Parameter.hv_Length1_rec_model, Parameter.hv_Length2_rec_model);

			if(1 == mode)
			{
				//原模板区域
				HObject ho_model;
				ReduceDomain(dstImage, ho_Rectangle, &ho_model);					
				CreateShapeModel(ho_model, "auto", HTuple(Parameter.hv_AStart).TupleRad(), HTuple(Parameter.hv_ARange).TupleRad(), 
					"auto", "auto", "use_polarity",
					"auto", "auto", &Parameter.hv_ModelID);
				FindShapeModel(dstImage, Parameter.hv_ModelID, HTuple(Parameter.hv_AStart).TupleRad(), HTuple(Parameter.hv_ARange).TupleRad(), 
					0.1, 1, 0, "least_squares", 0, 0.1, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
							
				VectorAngleToRigid(0, 0, 0, hv_Row, hv_Column, hv_Angle, &hv_HomMat2D);
				GetShapeModelContours(&ho_ModelContours, Parameter.hv_ModelID, 1);
				AffineTransContourXld(ho_ModelContours, &ho_ContoursAffinTrans, hv_HomMat2D);
				GenEmptyObj(&ho_Region_xld);
				SortContoursXld(ho_ContoursAffinTrans, &ho_SortedContours, "upper_left", "true", "row");
				CountObj(ho_SortedContours, &hv_Number_xld);
				HTuple end_val49 = hv_Number_xld-1;
				HTuple step_val49 = 1;
				for (hv_i=0; hv_i.Continue(end_val49, step_val49); hv_i += step_val49)
				{
					SelectObj(ho_SortedContours, &ho_ObjectSelected, hv_i+1);
					GetContourXld(ho_ObjectSelected, &hv_Row_obj, &hv_Col_obj);
					GenRegionPoints(&ho_Region_selected, hv_Row_obj, hv_Col_obj);
					Union2(ho_Region_xld, ho_Region_selected, &ho_Region_xld);
				}
				Tool.outlines.clear();
				//有效轮廓
				Tool.outlines.push_back(ho_Region_xld);
				//模板区域
				Tool.outlines.push_back(ho_Rectangle);
				//空，为后存擦除掉轮廓
				Tool.outlines.push_back(ho_RubberRectangle);
				//重心
				Tool.uplines.clear();
				Tool.uplines.push_back(ho_Region_gravity);
			}
		}
		else
		{
			Tool.strReslut.Format(L"未搜索到模板，得分： %.2f", (double)hv_Score*100);
		}
	}
	catch(...)
	{
		//AfxMessageBox(L"搜索模板失败");
		return false;
	}
	return true;
}

bool CalgorithmRegion::RestoreUpModel(HalconTool &ToolUp, HObject &imageUp, 
	HObject &ho_Rectangle, HObject &ho_Rectangle_model, 
	HObject &ho_RubberRectangle, HObject &ho_Region_xld, int mode, 
	ModelParameter &Parameter)
{
	return RestoreModel(ToolUp, imageUp, ho_Rectangle, ho_Rectangle_model, 
		ho_RubberRectangle, ho_Region_xld, mode,
		UpSetting, Parameter, Parameter.model_Name);
}

bool CalgorithmRegion::RestoreDownModel(HalconTool &ToolDown, HObject &imageDown, 
	HObject &ho_Rectangle, HObject &ho_Rectangle_model, 
	HObject &ho_RubberRectangle, HObject &ho_Region_xld, int mode, 
	ModelParameter &Parameter)
{
	return RestoreModel(ToolDown, imageDown, ho_Rectangle, ho_Rectangle_model, 
		ho_RubberRectangle, ho_Region_xld, mode, 
		DownSetting, Parameter, Parameter.model_Name);
}

//擦除模板
bool CalgorithmRegion::EditModel(HalconTool &Tool, HObject &scrImage, 
	HObject &ho_Rectangle, HObject &ho_Rectangle_model, 
	HObject &ho_RubberRectangle, HObject &ho_Region_xld)
{
	try{
		HObject ho_Xld_erasure, ho_Xld_model;
		Tool.strReslut = L"擦除模板";

		Intersection(ho_Rectangle, ho_RubberRectangle, &ho_RubberRectangle);
		Intersection(ho_Region_xld, ho_RubberRectangle, &ho_Xld_erasure);

		//擦除后的模板区域Rectangle_model和内部的轮廓Xld_model
		SymmDifference(ho_Rectangle, ho_RubberRectangle, &ho_Rectangle_model);
		Intersection(ho_Region_xld, ho_Rectangle_model, &ho_Xld_model);

		if(Tool.outlines.size()>2)
		{
			//有效轮廓
			Tool.outlines[0] = ho_Xld_model;
			//擦除掉轮廓
			Tool.outlines[2] = ho_Xld_erasure;
		}
		else
		{
			AfxMessageBox(L"擦除模板失败，请重新选择模板");
			return false;
		}
	}
	catch(...)
	{
		AfxMessageBox(L"擦除模板失败，请重新选择模板");
		return false;
	}
	return true;
}

//保存模板
bool CalgorithmRegion::SaveModel(HalconTool &Tool, HObject &scrImage,
	HObject &ho_Rectangle_model,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	try{
		HObject dstImage, ho_Domain_model;
		HTuple	hv_Score;
		Tool.outlines.clear();
		Tool.strReslut = L"保存模板";
		//图像预处理
		dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
		Tool.ShowImage(dstImage);

		//生成擦除后的模板图像
		ReduceDomain(dstImage, ho_Rectangle_model, &ho_Domain_model);
		CreateShapeModel(ho_Domain_model, "auto", HTuple(Parameter.hv_AStart).TupleRad(), HTuple(Parameter.hv_ARange).TupleRad(), 
			"auto", "auto", "use_polarity",
			"auto", "auto", &Parameter.hv_ModelID);
		FindShapeModel(dstImage, Parameter.hv_ModelID, HTuple(Parameter.hv_AStart).TupleRad(), HTuple(Parameter.hv_ARange).TupleRad(), 
			0.1, 1, 0, "least_squares", 0, 0.1, &Parameter.hv_Row, &Parameter.hv_Column, &Parameter.hv_Angle, &hv_Score);

		Tool.outlines.clear();
		//显示模板
		dev_display_shape_matchinresults(Tool.outlines, Parameter.hv_ModelID,
			Parameter.hv_Row, 
			Parameter.hv_Column, 
			Parameter.hv_Angle, 
			1, 
			1, 
			0);
		Tool.ShowImage(dstImage);
	}
	catch(...)
	{
		AfxMessageBox(L"模板错误");
		return false;
	}
	return true;
}

bool CalgorithmRegion::SaveUpModel(HalconTool &ToolUp, HObject &imageUp,
	HObject &ho_Rectangle_model,
	ModelParameter &Parameter)
{
	return SaveModel(ToolUp, imageUp, ho_Rectangle_model,
		UpSetting, Parameter, Parameter.model_Name);
}

bool CalgorithmRegion::SaveDownModel(HalconTool &ToolDown, HObject &imageDown,
	HObject &ho_Rectangle_model,
	ModelParameter &Parameter)
{	
	return SaveModel(ToolDown, imageDown, ho_Rectangle_model,
		DownSetting, Parameter, Parameter.model_Name);
}

//提取重心
bool CalgorithmRegion::SaveGravity(HalconTool &Tool, HObject &scrImage, 
	HObject &ho_Rectangle, HObject &ho_Rectangle_model, HObject &ho_RubberRectangle, HObject &ho_Region_xld,
	CString Setting, ModelParameter &Parameter, CString modelName)
{
	HObject dstImage;
	HObject	ho_RegionLines, ho_RegionLines1, ho_Region_gravity;
	HTuple	hv_Area1, hv_Row_gravity_model, hv_Column_gravity_model;

	try{
		//图像预处理
		dstImage = SetPretreatImage(Tool, scrImage, Setting, Parameter, modelName);
		Tool.ShowImage(dstImage);

		HObject ho_Xld_erasure, ho_Xld_model;
		Intersection(ho_Rectangle, ho_RubberRectangle, &ho_RubberRectangle);
		Intersection(ho_Region_xld, ho_RubberRectangle, &ho_Xld_erasure);

		//擦除后的模板区域Rectangle_model和内部的轮廓Xld_model
		SymmDifference(ho_Rectangle, ho_RubberRectangle, &ho_Rectangle_model);
		Intersection(ho_Region_xld, ho_Rectangle_model, &ho_Xld_model);

		//求模板轮廓的重心
		AreaCenter(ho_Xld_model, &hv_Area1, &hv_Row_gravity_model, &hv_Column_gravity_model);
		//在重心画十字线
		GenRegionLine(&ho_RegionLines, hv_Row_gravity_model+8, hv_Column_gravity_model, 
			hv_Row_gravity_model-8, hv_Column_gravity_model);
		GenRegionLine(&ho_RegionLines1, hv_Row_gravity_model, hv_Column_gravity_model-8, 
			hv_Row_gravity_model, hv_Column_gravity_model+8);
		Union2(ho_RegionLines, ho_RegionLines1, &ho_Region_gravity);

		//求模板轮廓的重心
		AreaCenter(ho_Xld_model, &hv_Area1, &Parameter.hv_Row_gravity_model, &Parameter.hv_Column_gravity_model);
		//在重心画十字线
		GenRegionLine(&ho_RegionLines, Parameter.hv_Row_gravity_model+8, Parameter.hv_Column_gravity_model, 
			Parameter.hv_Row_gravity_model-8, Parameter.hv_Column_gravity_model);
		GenRegionLine(&ho_RegionLines1, Parameter.hv_Row_gravity_model, Parameter.hv_Column_gravity_model-8, 
			Parameter.hv_Row_gravity_model, Parameter.hv_Column_gravity_model+8);
		Union2(ho_RegionLines, ho_RegionLines1, &ho_Region_gravity);

		Tool.strReslut = L"提取重心";
		Tool.uplines.clear();
		Tool.uplines.push_back(ho_Region_gravity);
		Tool.ShowImage(dstImage);
	}
	catch(...)
	{
		AfxMessageBox(L"提取重心失败");
		return false;
	}
	return true;
}
bool CalgorithmRegion::SaveUpGravity(HalconTool &ToolUp, HObject &imageUp, 
	HObject &ho_Rectangle, HObject &ho_Rectangle_model, HObject &ho_RubberRectangle, HObject &ho_Region_xld,
	ModelParameter &Parameter)
{	
	return SaveGravity(ToolUp, imageUp, 
		ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld,
		UpSetting, Parameter, Parameter.model_Name);
}
bool CalgorithmRegion::SaveDownGravity(HalconTool &ToolDown, HObject &imageDown, 
	HObject &ho_Rectangle, HObject &ho_Rectangle_model, HObject &ho_RubberRectangle, HObject &ho_Region_xld,
	ModelParameter &Parameter)
{
	return SaveGravity(ToolDown, imageDown, 
		ho_Rectangle, ho_Rectangle_model, ho_RubberRectangle, ho_Region_xld,
		DownSetting, Parameter, Parameter.model_Name);
}

//像素转中心坐标mm
bool CalgorithmRegion::Pix2CenterMM(double &hv_AhcX, double &hv_AhcY, CString modelName)
{		
	//像素比率
	if(L"_1" == modelName)
	{
		hv_AhcX = this->hv_AhcX[1];
		hv_AhcY = this->hv_AhcY[1];
	}
	else if(L"_2" == modelName)
	{
		hv_AhcX = this->hv_AhcX[2];
		hv_AhcY = this->hv_AhcY[2];
	}
	else if(L"_3" == modelName)
	{
		hv_AhcX = this->hv_AhcX[3];
		hv_AhcY = this->hv_AhcY[3];
	}
	else if(L"_4" == modelName)
	{
		hv_AhcX = this->hv_AhcX[4];
		hv_AhcY = this->hv_AhcY[4];
	}
	else 
	{
		hv_AhcX = this->hv_AhcX[0];
		hv_AhcY = this->hv_AhcY[0];
	}
	return true;
}

//计算点在Mark坐标系中的位置
bool CalgorithmRegion::Machine2Board(HTuple hv_X_Mark, HTuple hv_Y_Mark, HTuple hv_R_Mark, HTuple hv_X_Point, HTuple hv_Y_Point,
	double &hv_X_Point2Mark, double &hv_Y_Point2Mark)
{
	// Local control variables 
	HTuple  hv_X_Pow, hv_Y_Pow;
	HTuple  hv_Distance_Point2Mark, hv_Angle_Point2Mark;

	////输入
	////基准点在机械坐标系中的坐标
	//hv_X_Mark = 214.31208254163008;
	//hv_Y_Mark = 168.03720642716138;
	//hv_R_Mark = 0.68742848646768528;

	////某个点在机械坐标系中的坐标
	//hv_X_Point = 213.6509351;
	//hv_Y_Point = 228.0055315;

	TuplePow(hv_X_Point-hv_X_Mark, 2, &hv_X_Pow);
	TuplePow(hv_Y_Point-hv_Y_Mark, 2, &hv_Y_Pow);
	hv_Distance_Point2Mark = (hv_X_Pow+hv_Y_Pow).TupleSqrt();
	if (0 != ((hv_Y_Point-hv_Y_Mark)==0))
	{
		if (0 != ((hv_X_Point-hv_X_Mark)<0))
		{
			hv_Angle_Point2Mark = 180-hv_R_Mark;
		}
		else
		{
			hv_Angle_Point2Mark = 0-hv_R_Mark;
		}
	}
	else
	{
		hv_Angle_Point2Mark = ((((((hv_X_Point-hv_X_Mark)/hv_Distance_Point2Mark).TupleAcos()).TupleDeg())*(hv_Y_Point-hv_Y_Mark))/((hv_Y_Point-hv_Y_Mark).TupleAbs()))-hv_R_Mark;
	}

	//此点相对于Mark坐标系中的位置
	hv_X_Point2Mark = hv_Distance_Point2Mark*((hv_Angle_Point2Mark.TupleRad()).TupleCos());
	hv_Y_Point2Mark = hv_Distance_Point2Mark*((hv_Angle_Point2Mark.TupleRad()).TupleSin());

	return true;
}

//计算点在机械坐标系中的位置
bool CalgorithmRegion::Board2Machine(vector<double> &marks, 
	HTuple hv_X_Point2Mark, HTuple hv_Y_Point2Mark,
	HTuple hv_NumberOfRow_Baseplate, HTuple hv_NumberOfCol_Baseplate,
	HTuple hv_GapOfRow_Baseplate, HTuple hv_GapOfCol_Baseplate,
	double &hv_X_Point, double &hv_Y_Point)
{
	double hv_Angle_Mark, hv_Angle_Mark1;
	GetMarksXYR(marks, hv_Angle_Mark, hv_Angle_Mark1);

	// Local control variables 
	HTuple  hv_X_Mark, hv_Y_Mark, hv_X_Mark3, hv_Y_Mark3;
	HTuple  hv_R_Mark;
	HTuple  hv_Length1_Baseplate, hv_Length2_Baseplate;
	HTuple  hv_X_Point2Mark3, hv_Y_Point2Mark3, hv_X_Pow, hv_Y_Pow;
	HTuple  hv_Distance_Point2Mark, hv_Angle_Point2Mark, hv_X_Point1;
	HTuple  hv_Y_Point1, hv_X_Pow3, hv_Y_Pow3, hv_Distance_Point2Mark3;
	HTuple  hv_Angle_Point2Mark3, hv_X_Point3, hv_Y_Point3;

	//*****************************输入************************
	//基准点在机械坐标系中的坐标
	//Mark1
	hv_X_Mark = marks[0];
	hv_Y_Mark = marks[1];
	//Mark3
	hv_X_Mark3 = marks[4];
	hv_Y_Mark3 = marks[5];
	//底板角度（Mark1和Mark3坐标系公用）
	hv_R_Mark = hv_Angle_Mark;

	////某个点在Mark1坐标系中的坐标
	//hv_X_Point2Mark = 100;
	//hv_Y_Point2Mark = 20;

	////底板的行数和列数
	//hv_NumberOfRow_Baseplate = 6;
	//hv_NumberOfCol_Baseplate = 8;
	////底板的行间距和列间距
	//hv_GapOfRow_Baseplate = 20;
	//hv_GapOfCol_Baseplate = 20;
	//计算底板的长和宽
	hv_Length1_Baseplate = (hv_NumberOfCol_Baseplate-1)*hv_GapOfCol_Baseplate;
	hv_Length2_Baseplate = (hv_NumberOfRow_Baseplate-1)*hv_GapOfRow_Baseplate;


	//**************************计算过程****************************************
	//计算该点点在Mark3坐标系中的坐标
	hv_X_Point2Mark3 = hv_X_Point2Mark-hv_Length1_Baseplate;
	hv_Y_Point2Mark3 = hv_Y_Point2Mark-hv_Length2_Baseplate;
	//***********************通过Mark1为基准点计算
	TuplePow(hv_X_Point2Mark, 2, &hv_X_Pow);
	TuplePow(hv_Y_Point2Mark, 2, &hv_Y_Pow);
	hv_Distance_Point2Mark = (hv_X_Pow+hv_Y_Pow).TupleSqrt();
	if (0 != (hv_Y_Point2Mark==0))
	{
		if (0 != (hv_X_Point2Mark<0))
		{
			hv_Angle_Point2Mark = 180;
		}
		else
		{
			hv_Angle_Point2Mark = 0;
		}
	}
	else
	{
		hv_Angle_Point2Mark = ((((hv_X_Point2Mark/hv_Distance_Point2Mark).TupleAcos()).TupleDeg())*hv_Y_Point2Mark)/(hv_Y_Point2Mark.TupleAbs());
	}
	//此点相对在机械坐标系中的位置(以Mark1计算）
	hv_X_Point1 = (hv_Distance_Point2Mark*(((hv_Angle_Point2Mark+hv_R_Mark).TupleRad()).TupleCos()))+hv_X_Mark;
	hv_Y_Point1 = (hv_Distance_Point2Mark*(((hv_Angle_Point2Mark+hv_R_Mark).TupleRad()).TupleSin()))+hv_Y_Mark;

	//**************************以Mark3为基准点计算
	TuplePow(hv_X_Point2Mark3, 2, &hv_X_Pow3);
	TuplePow(hv_Y_Point2Mark3, 2, &hv_Y_Pow3);
	hv_Distance_Point2Mark3 = (hv_X_Pow3+hv_Y_Pow3).TupleSqrt();
	if (0 != (hv_Y_Point2Mark3==0))
	{
		if (0 != (hv_X_Point2Mark3<0))
		{
			hv_Angle_Point2Mark3 = 180;
		}
		else
		{
			hv_Angle_Point2Mark3 = 0;
		}
	}
	else
	{
		hv_Angle_Point2Mark3 = ((((hv_X_Point2Mark3/hv_Distance_Point2Mark3).TupleAcos()).TupleDeg())*hv_Y_Point2Mark3)/(hv_Y_Point2Mark3.TupleAbs());
	}
	hv_X_Point3 = (hv_Distance_Point2Mark3*(((hv_Angle_Point2Mark3+hv_R_Mark).TupleRad()).TupleCos()))+hv_X_Mark3;
	hv_Y_Point3 = (hv_Distance_Point2Mark3*(((hv_Angle_Point2Mark3+hv_R_Mark).TupleRad()).TupleSin()))+hv_Y_Mark3;

	//*************************求平均*************************
	//hv_X_Point = (hv_X_Point3+hv_X_Point1)/2;
	//hv_Y_Point = (hv_Y_Point3+hv_Y_Point1)/2;
	hv_X_Point = hv_X_Point1;
	hv_Y_Point = hv_Y_Point1;

	return true;
}

bool CalgorithmRegion::Board2Machine(HTuple hv_X_Mark, HTuple hv_Y_Mark, 
	HTuple hv_R_Mark, HTuple hv_R_Mark1, 
	HTuple hv_X_Point2Mark, HTuple hv_Y_Point2Mark,
	double &hv_X_Point, double &hv_Y_Point)
{
	// Local control variables 
	HTuple  hv_X_Pow, hv_Y_Pow;
	HTuple  hv_Distance_Point2Mark, hv_Angle_Point2Mark;


	////输入
	////基准点在机械坐标系中的坐标
	//hv_X_Mark = 212.054;
	//hv_Y_Mark = 207.779;
	//hv_R_Mark = 1.30631;
	//hv_R_Mark1 = 91.30631;
	////某个点在Mark点系中的坐标
	//hv_X_Point2Mark = 10;
	//hv_Y_Point2Mark = 100;

	TuplePow(hv_X_Point2Mark, 2, &hv_X_Pow);
	TuplePow(hv_Y_Point2Mark, 2, &hv_Y_Pow);
	hv_Distance_Point2Mark = (hv_X_Pow+hv_Y_Pow).TupleSqrt();
	if (0 != (hv_Y_Point2Mark==0))
	{
		if (0 != (hv_X_Point2Mark<0))
		{
			hv_Angle_Point2Mark = 180;
		}
		else
		{
			hv_Angle_Point2Mark = 0;
		}
	}
	else
	{
		hv_Angle_Point2Mark = ((((hv_X_Point2Mark/hv_Distance_Point2Mark).TupleAcos()).TupleDeg())*hv_Y_Point2Mark)/(hv_Y_Point2Mark.TupleAbs());
	}
	//此点相对在机械坐标系中的位置
	hv_X_Point = (double)(hv_Distance_Point2Mark*(((hv_Angle_Point2Mark+hv_R_Mark).TupleRad()).TupleCos()))+hv_X_Mark;
	hv_Y_Point = (double)(hv_Distance_Point2Mark*(((180-hv_R_Mark1-hv_Angle_Point2Mark).TupleRad()).TupleCos()))+hv_Y_Mark;

	return true;
}

bool CalgorithmRegion::Board2Machine(vector<double> &parameters, vector<double> &results)
{
	// Local control variables 
	HTuple  hv_X_Mark1, hv_Y_Mark1, hv_X_Mark2, hv_Y_Mark2;
	HTuple  hv_X_Mark3, hv_Y_Mark3, hv_X_Mark4, hv_Y_Mark4;
	HTuple  hv_X_Point2Mark, hv_Y_Point2Mark, hv_X_Mark2InBaseplate;
	HTuple  hv_Y_Mark2InBaseplate, hv_X_Mark3InBaseplate, hv_Y_Mark3InBaseplate;
	HTuple  hv_X_Mark4InBaseplate, hv_Y_Mark4InBaseplate, hv_X_Pow12;
	HTuple  hv_Y_Pow12, hv_Distance_Mark12, hv_Angle_Mark12;
	HTuple  hv_X_Pow34, hv_Y_Pow34, hv_Distance_Mark34, hv_Angle_Mark34;
	HTuple  hv_Angle_Baseplate, hv_Distance_Mark2InBaseplate;
	HTuple  hv_Angle_Mark2InBaseplate, hv_Distance_Mark22Mark1;
	HTuple  hv_Angle_Mark22Mark1, hv_Distance_Compensation2;
	HTuple  hv_Angle_Compensation2, hv_Distance_Mark3InBaseplate;
	HTuple  hv_Angle_Mark3InBaseplate, hv_Distance_Mark32Mark1;
	HTuple  hv_Angle_Mark32Mark1, hv_Distance_Compensation3;
	HTuple  hv_Angle_Compensation3, hv_Distance_Mark4InBaseplate;
	HTuple  hv_Angle_Mark4InBaseplate, hv_Distance_Mark42Mark1;
	HTuple  hv_Angle_Mark42Mark1, hv_Distance_Compensation4;
	HTuple  hv_Angle_Compensation4, hv_X_Pow, hv_Y_Pow, hv_Distance_Point2Mark;
	HTuple  hv_Angle_Point2Mark, hv_Factor_D2, hv_Factor_D3;
	HTuple  hv_Factor_D4, hv_Distance_rad, hv_Factor_Distance;
	HTuple  hv_Distance_Compensation, hv_Factor_Angle, hv_Angle_Compensation;
	HTuple  hv_X_Point, hv_Y_Point;


	//***********************输入****************************
	//Mark点数据
	hv_X_Mark1 = parameters[0];
	hv_Y_Mark1 = parameters[1];
	hv_X_Mark2 = parameters[2];
	hv_Y_Mark2 = parameters[3];
	hv_X_Mark3 = parameters[4];
	hv_Y_Mark3 = parameters[5];
	hv_X_Mark4 = parameters[6];
	hv_Y_Mark4 = parameters[7];
	//*目标点相对于底板坐标系中的位置
	hv_X_Point2Mark = parameters[8];
	hv_Y_Point2Mark = parameters[9];
	//*Mark2,Mark3,Mark4相对于底板坐标系的位置
	hv_X_Mark2InBaseplate = parameters[10];
	hv_Y_Mark2InBaseplate = parameters[11];
	hv_X_Mark3InBaseplate = parameters[12];
	hv_Y_Mark3InBaseplate = parameters[13];
	hv_X_Mark4InBaseplate = parameters[14];
	hv_Y_Mark4InBaseplate = parameters[15];
	//********************计算过程***************************

	//**************计算底板角度R_Baseplate
	TuplePow(hv_X_Mark2-hv_X_Mark1, 2, &hv_X_Pow12);
	TuplePow(hv_Y_Mark2-hv_Y_Mark1, 2, &hv_Y_Pow12);
	hv_Distance_Mark12 = (hv_X_Pow12+hv_Y_Pow12).TupleSqrt();
	if (0 != ((hv_Y_Mark2-hv_Y_Mark1)==0))
	{
		if (0 != ((hv_X_Mark2-hv_X_Mark1)<0))
		{
			hv_Angle_Mark12 = 180;
		}
		else
		{
			hv_Angle_Mark12 = 0;
		}
	}
	else
	{
		hv_Angle_Mark12 = (((((hv_X_Mark2-hv_X_Mark1)/hv_Distance_Mark12).TupleAcos()).TupleDeg())*(hv_Y_Mark2-hv_Y_Mark1))/((hv_Y_Mark2-hv_Y_Mark1).TupleAbs());
	}
	TuplePow(hv_X_Mark3-hv_X_Mark4, 2, &hv_X_Pow34);
	TuplePow(hv_Y_Mark3-hv_Y_Mark4, 2, &hv_Y_Pow34);
	hv_Distance_Mark34 = (hv_X_Pow34+hv_Y_Pow34).TupleSqrt();
	if (0 != ((hv_Y_Mark3-hv_Y_Mark4)==0))
	{
		if (0 != ((hv_X_Mark3-hv_X_Mark4)<0))
		{
			hv_Angle_Mark34 = 180;
		}
		else
		{
			hv_Angle_Mark34 = 0;
		}
	}
	else
	{
		hv_Angle_Mark34 = (((((hv_X_Mark3-hv_X_Mark4)/hv_Distance_Mark34).TupleAcos()).TupleDeg())*(hv_Y_Mark3-hv_Y_Mark4))/((hv_Y_Mark3-hv_Y_Mark4).TupleAbs());
	}
	//hv_Angle_Baseplate = (hv_Angle_Mark12+hv_Angle_Mark34)/2;
	 hv_Angle_Baseplate = hv_Angle_Mark12;

	//********************计算误差
	///Mark2的计算误差
	hv_Distance_Mark2InBaseplate = ((hv_X_Mark2InBaseplate*hv_X_Mark2InBaseplate)+(hv_Y_Mark2InBaseplate*hv_Y_Mark2InBaseplate)).TupleSqrt();
	if (0 != (hv_Y_Mark2InBaseplate==0))
	{
		if (0 != (hv_X_Mark2InBaseplate<0))
		{
			hv_Angle_Mark2InBaseplate = 180;
		}
		else
		{
			hv_Angle_Mark2InBaseplate = 0;
		}
	}
	else
	{
		hv_Angle_Mark2InBaseplate = ((((hv_X_Mark2InBaseplate/hv_Distance_Mark2InBaseplate).TupleAcos()).TupleDeg())*hv_Y_Mark2InBaseplate)/(hv_Y_Mark2InBaseplate.TupleAbs());
	}
	hv_Distance_Mark22Mark1 = (((hv_X_Mark2-hv_X_Mark1)*(hv_X_Mark2-hv_X_Mark1))+((hv_Y_Mark2-hv_Y_Mark1)*(hv_Y_Mark2-hv_Y_Mark1))).TupleSqrt();
	if (0 != ((hv_Y_Mark2-hv_Y_Mark1)==0))
	{
		if (0 != ((hv_X_Mark2-hv_X_Mark1)<0))
		{
			hv_Angle_Mark22Mark1 = 180;
		}
		else
		{
			hv_Angle_Mark22Mark1 = 0;
		}
	}
	else
	{
		hv_Angle_Mark22Mark1 = (((((hv_X_Mark2-hv_X_Mark1)/hv_Distance_Mark22Mark1).TupleAcos()).TupleDeg())*(hv_Y_Mark2-hv_Y_Mark1))/((hv_Y_Mark2-hv_Y_Mark1).TupleAbs());
	}
	hv_Distance_Compensation2 = hv_Distance_Mark22Mark1-hv_Distance_Mark2InBaseplate;
	hv_Angle_Compensation2 = hv_Angle_Mark22Mark1-(hv_Angle_Mark2InBaseplate+hv_Angle_Baseplate);
	////Mark3的计算误差
	hv_Distance_Mark3InBaseplate = ((hv_X_Mark3InBaseplate*hv_X_Mark3InBaseplate)+(hv_Y_Mark3InBaseplate*hv_Y_Mark3InBaseplate)).TupleSqrt();
	if (0 != (hv_Y_Mark3InBaseplate==0))
	{
		if (0 != (hv_X_Mark3InBaseplate<0))
		{
			hv_Angle_Mark3InBaseplate = 180;
		}
		else
		{
			hv_Angle_Mark3InBaseplate = 0;
		}
	}
	else
	{
		hv_Angle_Mark3InBaseplate = ((((hv_X_Mark3InBaseplate/hv_Distance_Mark3InBaseplate).TupleAcos()).TupleDeg())*hv_Y_Mark3InBaseplate)/(hv_Y_Mark3InBaseplate.TupleAbs());
	}
	hv_Distance_Mark32Mark1 = (((hv_X_Mark3-hv_X_Mark1)*(hv_X_Mark3-hv_X_Mark1))+((hv_Y_Mark3-hv_Y_Mark1)*(hv_Y_Mark3-hv_Y_Mark1))).TupleSqrt();
	if (0 != ((hv_Y_Mark3-hv_Y_Mark1)==0))
	{
		if (0 != ((hv_X_Mark3-hv_X_Mark1)<0))
		{
			hv_Angle_Mark32Mark1 = 180;
		}
		else
		{
			hv_Angle_Mark32Mark1 = 0;
		}
	}
	else
	{
		hv_Angle_Mark32Mark1 = (((((hv_X_Mark3-hv_X_Mark1)/hv_Distance_Mark32Mark1).TupleAcos()).TupleDeg())*(hv_Y_Mark3-hv_Y_Mark1))/((hv_Y_Mark3-hv_Y_Mark1).TupleAbs());
	}
	hv_Distance_Compensation3 = hv_Distance_Mark32Mark1-hv_Distance_Mark3InBaseplate;
	hv_Angle_Compensation3 = hv_Angle_Mark32Mark1-(hv_Angle_Mark3InBaseplate+hv_Angle_Baseplate);
	/////Mark4的计算误差
	hv_Distance_Mark4InBaseplate = ((hv_X_Mark4InBaseplate*hv_X_Mark4InBaseplate)+(hv_Y_Mark4InBaseplate*hv_Y_Mark4InBaseplate)).TupleSqrt();
	if (0 != (hv_Y_Mark4InBaseplate==0))
	{
		if (0 != (hv_X_Mark4InBaseplate<0))
		{
			hv_Angle_Mark4InBaseplate = 180;
		}
		else
		{
			hv_Angle_Mark4InBaseplate = 0;
		}
	}
	else
	{
		hv_Angle_Mark4InBaseplate = ((((hv_X_Mark4InBaseplate/hv_Distance_Mark4InBaseplate).TupleAcos()).TupleDeg())*hv_Y_Mark4InBaseplate)/(hv_Y_Mark4InBaseplate.TupleAbs());
	}
	hv_Distance_Mark42Mark1 = (((hv_X_Mark4-hv_X_Mark1)*(hv_X_Mark4-hv_X_Mark1))+((hv_Y_Mark4-hv_Y_Mark1)*(hv_Y_Mark4-hv_Y_Mark1))).TupleSqrt();
	if (0 != ((hv_Y_Mark4-hv_Y_Mark1)==0))
	{
		if (0 != ((hv_X_Mark4-hv_X_Mark1)<0))
		{
			hv_Angle_Mark42Mark1 = 180;
		}
		else
		{
			hv_Angle_Mark42Mark1 = 0;
		}
	}
	else
	{
		hv_Angle_Mark42Mark1 = (((((hv_X_Mark4-hv_X_Mark1)/hv_Distance_Mark42Mark1).TupleAcos()).TupleDeg())*(hv_Y_Mark4-hv_Y_Mark1))/((hv_Y_Mark4-hv_Y_Mark1).TupleAbs());
	}
	hv_Distance_Compensation4 = hv_Distance_Mark42Mark1-hv_Distance_Mark4InBaseplate;
	hv_Angle_Compensation4 = hv_Angle_Mark42Mark1-(hv_Angle_Mark4InBaseplate+hv_Angle_Baseplate);

	//*******************计算目标点在Mark坐标系中的极坐标数据Distance_Point2Mark 和Angle_Point2Mark
	TuplePow(hv_X_Point2Mark, 2, &hv_X_Pow);
	TuplePow(hv_Y_Point2Mark, 2, &hv_Y_Pow);
	hv_Distance_Point2Mark = (hv_X_Pow+hv_Y_Pow).TupleSqrt();
	if (0 != (hv_Y_Point2Mark==0))
	{
		if (0 != (hv_X_Point2Mark<0))
		{
			hv_Angle_Point2Mark = 180;
		}
		else
		{
			hv_Angle_Point2Mark = 0;
		}
	}
	else
	{
		hv_Angle_Point2Mark = ((((hv_X_Point2Mark/hv_Distance_Point2Mark).TupleAcos()).TupleDeg())*hv_Y_Point2Mark)/(hv_Y_Point2Mark.TupleAbs());
	}
	//*************************补偿计算
	///计算补偿距离参数Factor_D2,Factor_D3,Factor_D4,Factor_Distance
	if (0 != (HTuple(hv_Angle_Point2Mark<hv_Angle_Mark3InBaseplate).TupleAnd(hv_Angle_Point2Mark<45)))
	{
		hv_Factor_D2 = (2*(hv_Angle_Point2Mark.TupleRad())).TupleCos();
		hv_Factor_D3 = (2*((hv_Angle_Point2Mark-hv_Angle_Mark3InBaseplate).TupleRad())).TupleCos();
		hv_Factor_D4 = 0;
	}
	else if (0 != (HTuple(HTuple(45<=hv_Angle_Point2Mark).TupleAnd(hv_Angle_Point2Mark<=hv_Angle_Mark3InBaseplate)).TupleOr(HTuple(45>=hv_Angle_Point2Mark).TupleAnd(hv_Angle_Point2Mark>=hv_Angle_Mark3InBaseplate))))
	{
		hv_Factor_D2 = 0;
		hv_Factor_D3 = (2*((hv_Angle_Point2Mark-hv_Angle_Mark3InBaseplate).TupleRad())).TupleCos();
		hv_Factor_D4 = 0;
	}
	else if (0 != (HTuple(45<hv_Angle_Point2Mark).TupleAnd(hv_Angle_Mark3InBaseplate<hv_Angle_Point2Mark)))
	{
		hv_Factor_D2 = 0;
		hv_Factor_D3 = (2*((hv_Angle_Point2Mark-hv_Angle_Mark3InBaseplate).TupleRad())).TupleCos();
		hv_Factor_D4 = (2*((hv_Angle_Point2Mark-90).TupleRad())).TupleCos();
	}

	if (0 != (hv_Angle_Point2Mark<hv_Angle_Mark3InBaseplate))
	{
		hv_Distance_rad = hv_X_Mark3InBaseplate/((hv_Angle_Point2Mark.TupleRad()).TupleCos());
	}
	else if (0 != (hv_Angle_Mark3InBaseplate<=hv_Angle_Point2Mark))
	{
		hv_Distance_rad = hv_Y_Mark3InBaseplate/((hv_Angle_Point2Mark.TupleRad()).TupleSin());
	}
	hv_Factor_Distance = hv_Distance_Point2Mark/hv_Distance_rad;
	////计算距离补偿值
	//hv_Distance_Compensation = (((hv_Distance_Compensation2*hv_Factor_D2)+(hv_Distance_Compensation3*hv_Factor_D3))+(hv_Distance_Compensation2*hv_Factor_D4))*hv_Factor_Distance;
	hv_Distance_Compensation=0;
	/////计算角度补偿参数
	hv_Factor_Angle = (hv_Angle_Point2Mark.TupleRad()).TupleSin();
	//////计算角度补偿值
	//hv_Angle_Compensation = hv_Angle_Compensation4*hv_Factor_D4+hv_Angle_Compensation3*hv_Factor_D3+hv_Angle_Compensation2*hv_Factor_D2;
	hv_Angle_Compensation=(hv_Angle_Compensation4+hv_Angle_Compensation3+hv_Angle_Compensation2)/3;
	//******************此点相对在机械坐标系中的位置 X_Point , Y_Point

	hv_Angle_Point2Mark += hv_Angle_Compensation;
	hv_Distance_Point2Mark += hv_Distance_Compensation;
	hv_X_Point = (hv_Distance_Point2Mark*(((hv_Angle_Point2Mark+hv_Angle_Baseplate).TupleRad()).TupleCos()))+hv_X_Mark1;
	hv_Y_Point = (hv_Distance_Point2Mark*(((hv_Angle_Point2Mark+hv_Angle_Baseplate).TupleRad()).TupleSin()))+hv_Y_Mark1;

	//*************************************输出************************************************
	hv_X_Point = hv_X_Point;
	hv_Y_Point = hv_Y_Point;

	results.clear();
	results.push_back((double)hv_X_Point);
	results.push_back((double)hv_Y_Point);
	return true;
}

//计算底板整体角度
bool CalgorithmRegion::GetMarksXYR(vector<double> &marks, double &hv_Angle_Mark, double &hv_Angle_Mark1)
{
	// Local control variables 
	HTuple  hv_X_Mark1, hv_Y_Mark1, hv_X_Mark2, hv_Y_Mark2;
	HTuple  hv_X_Mark3, hv_Y_Mark3, hv_X_Mark4, hv_Y_Mark4;
	HTuple  hv_X_Pow12, hv_Y_Pow12, hv_Distance_Mark12, hv_Angle_Mark12;
	HTuple  hv_X_Pow34, hv_Y_Pow34, hv_Distance_Mark34, hv_Angle_Mark34;
	HTuple  hv_X_Pow14, hv_Y_Pow14, hv_Distance_Mark14;
	HTuple  hv_Angle_Mark14, hv_X_Pow32, hv_Y_Pow32, hv_Distance_Mark32;
	HTuple  hv_Angle_Mark32;


	////输入
	////基准点在机械坐标系中的坐标
	hv_X_Mark1 = marks[0];
	hv_Y_Mark1 = marks[1];



	hv_X_Mark2 = marks[2];
	hv_Y_Mark2 = marks[3];


	hv_X_Mark3 = marks[4];
	hv_Y_Mark3 = marks[5];


	hv_X_Mark4 = marks[6];
	hv_Y_Mark4 = marks[7];


	TuplePow(hv_X_Mark2-hv_X_Mark1, 2, &hv_X_Pow12);
	TuplePow(hv_Y_Mark2-hv_Y_Mark1, 2, &hv_Y_Pow12);
	hv_Distance_Mark12 = (hv_X_Pow12+hv_Y_Pow12).TupleSqrt();
	if (0 != ((hv_Y_Mark2-hv_Y_Mark1)==0))
	{
		if (0 != ((hv_X_Mark2-hv_X_Mark1)<0))
		{
			hv_Angle_Mark12 = 180;
		}
		else
		{
			hv_Angle_Mark12 = 0;
		}
	}
	else
	{
		hv_Angle_Mark12 = (((((hv_X_Mark2-hv_X_Mark1)/hv_Distance_Mark12).TupleAcos()).TupleDeg())*(hv_Y_Mark2-hv_Y_Mark1))/((hv_Y_Mark2-hv_Y_Mark1).TupleAbs());
	}


	TuplePow(hv_X_Mark3-hv_X_Mark4, 2, &hv_X_Pow34);
	TuplePow(hv_Y_Mark3-hv_Y_Mark4, 2, &hv_Y_Pow34);
	hv_Distance_Mark34 = (hv_X_Pow34+hv_Y_Pow34).TupleSqrt();
	if (0 != ((hv_Y_Mark3-hv_Y_Mark4)==0))
	{
		if (0 != ((hv_X_Mark3-hv_X_Mark4)<0))
		{
			hv_Angle_Mark34 = 180;
		}
		else
		{
			hv_Angle_Mark34 = 0;
		}
	}
	else
	{
		hv_Angle_Mark34 = (((((hv_X_Mark3-hv_X_Mark4)/hv_Distance_Mark34).TupleAcos()).TupleDeg())*(hv_Y_Mark3-hv_Y_Mark4))/((hv_Y_Mark3-hv_Y_Mark4).TupleAbs());
	}


	//hv_Angle_Mark = (double)(hv_Angle_Mark34+hv_Angle_Mark12)/2;
	hv_Angle_Mark = hv_Angle_Mark12;
	//***********************************


	TuplePow(hv_X_Mark4-hv_X_Mark1, 2, &hv_X_Pow14);
	TuplePow(hv_Y_Mark4-hv_Y_Mark1, 2, &hv_Y_Pow14);
	hv_Distance_Mark14 = (hv_X_Pow14+hv_Y_Pow14).TupleSqrt();
	if (0 != ((hv_Y_Mark4-hv_Y_Mark1)==0))
	{
		if (0 != ((hv_X_Mark4-hv_X_Mark1)<0))
		{
			hv_Angle_Mark14 = 180;
		}
		else
		{
			hv_Angle_Mark14 = 0;
		}
	}
	else
	{
		hv_Angle_Mark14 = (((((hv_X_Mark4-hv_X_Mark1)/hv_Distance_Mark14).TupleAcos()).TupleDeg())*(hv_Y_Mark4-hv_Y_Mark1))/((hv_Y_Mark4-hv_Y_Mark1).TupleAbs());
	}


	TuplePow(hv_X_Mark3-hv_X_Mark2, 2, &hv_X_Pow32);
	TuplePow(hv_Y_Mark3-hv_Y_Mark2, 2, &hv_Y_Pow32);
	hv_Distance_Mark32 = (hv_X_Pow32+hv_Y_Pow32).TupleSqrt();
	if (0 != ((hv_Y_Mark3-hv_Y_Mark2)==0))
	{
		if (0 != ((hv_X_Mark3-hv_X_Mark2)<0))
		{
			hv_Angle_Mark32 = 180;
		}
		else
		{
			hv_Angle_Mark32 = 0;
		}
	}
	else
	{
		hv_Angle_Mark32 = (((((hv_X_Mark3-hv_X_Mark2)/hv_Distance_Mark32).TupleAcos()).TupleDeg())*(hv_Y_Mark3-hv_Y_Mark2))/((hv_Y_Mark3-hv_Y_Mark2).TupleAbs());
	}


	hv_Angle_Mark1 = (double)(hv_Angle_Mark32+hv_Angle_Mark14)/2;
	return true;
}

//限制角度范围0~180 0~-180
double CalgorithmRegion::LitmitAngle(double Angle)
{
	double l_Angle;
	l_Angle = Angle;
	while(l_Angle > 180)
	{
		l_Angle -= 360;
	}

	while(l_Angle < -180)
	{
		l_Angle += 360;
	}

	return l_Angle;
}

//求中值
double CalgorithmRegion::MedianVector(vector<double> &degAngle)
{ 
	int NumOfDegAngle =degAngle.size();
	sort(degAngle.begin(),degAngle.end());
	int IndexOfMedian = NumOfDegAngle/2;
	double result =degAngle.at(IndexOfMedian);
	return result;
}

//计算点在Mark坐标系中的位置
bool CalgorithmRegion::BulidBoard(vector<double> &parameters, vector<double> &resultXs, vector<double> &resultYs)
{
	try{
		// Local control variables 
		HTuple  hv_X_Mark1, hv_Y_Mark1, hv_X_Mark2, hv_Y_Mark2;
		HTuple  hv_X_Mark3, hv_Y_Mark3, hv_X_Mark4, hv_Y_Mark4;
		HTuple  hv_X_Corner_LeftDown, hv_Y_Corner_LeftDown, hv_X_Corner_RightUp;
		HTuple  hv_Y_Corner_RightUp, hv_Number_Row, hv_Number_Col;
		HTuple  hv_Distance, hv_DegAngle, hv_Distance1, hv_DegAngle1;
		HTuple  hv_R_Mark, hv_Distance_LeftDown, hv_DegAngle3, hv_DegAngle_LeftDown;
		HTuple  hv_X_Corner_LeftDown_InMark, hv_Y_Corner_LeftDown_InMark;
		HTuple  hv_Distance_RightUp, hv_DegAngle4, hv_DegAngle_RightUp;
		HTuple  hv_X_Corner_RightUp_InMark, hv_Y_Corner_RightUp_InMark;
		HTuple  hv_Gap_Row, hv_Gap_Col, hv_I, hv_J, hv_Index, hv_X_obj;
		HTuple  hv_Y_obj, l_Distance,l_DegAngle;


		//****************************输入*******************************
		//基准点在机械坐标系中的坐标
		hv_X_Mark1 = parameters[0];
		hv_Y_Mark1 = parameters[1];

		hv_X_Mark2 = parameters[2];
		hv_Y_Mark2 = parameters[3];

		hv_X_Mark3 = parameters[4];
		hv_Y_Mark3 = parameters[5];

		hv_X_Mark4 = parameters[6];
		hv_Y_Mark4 = parameters[7];
		//左下角机械坐标
		hv_X_Corner_LeftDown = parameters[8];
		hv_Y_Corner_LeftDown = parameters[9];
		//右上角机械坐标
		hv_X_Corner_RightUp = parameters[10];
		hv_Y_Corner_RightUp = parameters[11];
		//行列数
		hv_Number_Row = parameters[12];
		hv_Number_Col = parameters[13];
		////////////////////////////////////////////////////////////////
		//***********************计算过程********************************
		DistanceAndAngle(hv_X_Mark1, hv_Y_Mark1, hv_X_Mark2, hv_Y_Mark2, &hv_Distance, 
			&hv_DegAngle);
		hv_R_Mark=hv_DegAngle;

		//DistanceAndAngle(hv_X_Mark1, hv_Y_Mark1, hv_X_Corner_LeftDown, hv_Y_Corner_LeftDown, 
		//	&hv_Distance_LeftDown, &hv_DegAngle3);
		//hv_DegAngle_LeftDown = hv_DegAngle3-hv_R_Mark;
		//hv_X_Corner_LeftDown_InMark = hv_Distance_LeftDown*((hv_DegAngle_LeftDown.TupleRad()).TupleCos());
		//hv_Y_Corner_LeftDown_InMark = hv_Distance_LeftDown*((hv_DegAngle_LeftDown.TupleRad()).TupleSin());
		//DistanceAndAngle(hv_X_Mark1, hv_Y_Mark1, hv_X_Corner_RightUp, hv_Y_Corner_RightUp, 
		//	&hv_Distance_RightUp, &hv_DegAngle4);
		//hv_DegAngle_RightUp = hv_DegAngle4-hv_R_Mark;
		//hv_X_Corner_RightUp_InMark = hv_Distance_RightUp*((hv_DegAngle_RightUp.TupleRad()).TupleCos());
		//hv_Y_Corner_RightUp_InMark = hv_Distance_RightUp*((hv_DegAngle_RightUp.TupleRad()).TupleSin());
		//行列间距
		if(hv_Number_Row==1)
			{
			hv_Gap_Row=0;
			}
		else{
			hv_Gap_Row = ((hv_Y_Corner_RightUp-hv_Y_Corner_LeftDown).TupleAbs())/(hv_Number_Row-1);
			}

			if(hv_Number_Col==1)
			{
			hv_Gap_Col=0;
			}
		else{
			hv_Gap_Col = ((hv_X_Corner_RightUp-hv_X_Corner_LeftDown).TupleAbs())/(hv_Number_Col-1);
			}
	

		HTuple end_val45 = hv_Number_Row-1;
		HTuple step_val45 = 1;

		resultXs.clear();
		resultYs.clear();
		for (hv_I=0; hv_I.Continue(end_val45, step_val45); hv_I += step_val45)
		{
			HTuple end_val46 = hv_Number_Col-1;
			HTuple step_val46 = 1;
			for (hv_J=0; hv_J.Continue(end_val46, step_val46); hv_J += step_val46)
			{
				hv_Index = (hv_Number_Col*hv_I)+hv_J;
				double l_MechineX,l_MechineY;
				l_MechineX = hv_X_Corner_LeftDown+(hv_J*hv_Gap_Col);
				l_MechineY = hv_Y_Corner_LeftDown+(hv_I*hv_Gap_Row);
			    DistanceAndAngle(hv_X_Mark1, hv_Y_Mark1, l_MechineX, l_MechineY, 
			      &l_Distance, &l_DegAngle);		
				hv_X_obj[hv_Index]= l_Distance*(((l_DegAngle-hv_DegAngle).TupleRad()).TupleCos());
				hv_Y_obj[hv_Index] = l_Distance*(((l_DegAngle-hv_DegAngle).TupleRad()).TupleSin());
				resultXs.push_back((double)hv_X_obj[hv_Index]);
				resultYs.push_back((double)hv_Y_obj[hv_Index]);
			}
		}
		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////
	}catch(...)
	{
		AfxMessageBox(L"计算点在Mark坐标系中的位置失败");
		return false;
	}
	return true;
}

//计算点在Mark坐标系中的位置（两点相对关系）
void CalgorithmRegion::DistanceAndAngle (HTuple hv_X_Mark, HTuple hv_Y_Mark, HTuple hv_X_Point, HTuple hv_Y_Point, 
    HTuple *hv_Distance_Point2Mark, HTuple *hv_Angle_Point2Mark)
{

  // Local control variables 
  HTuple  hv_X_Pow, hv_Y_Pow;

  TuplePow(hv_X_Point-hv_X_Mark, 2, &hv_X_Pow);
  TuplePow(hv_Y_Point-hv_Y_Mark, 2, &hv_Y_Pow);
  (*hv_Distance_Point2Mark) = (hv_X_Pow+hv_Y_Pow).TupleSqrt();
  if (0 != ((hv_Y_Point-hv_Y_Mark)==0))
  {
    if (0 != ((hv_X_Point-hv_X_Mark)<0))
    {
      (*hv_Angle_Point2Mark) = 180;
    }
    else
    {
      (*hv_Angle_Point2Mark) = 0;
    }
  }
  else
  {
    (*hv_Angle_Point2Mark) = (((((hv_X_Point-hv_X_Mark)/(*hv_Distance_Point2Mark)).TupleAcos()).TupleDeg())*(hv_Y_Point-hv_Y_Mark))/((hv_Y_Point-hv_Y_Mark).TupleAbs());
  }
  return;
}

//手动生成数据
bool  CalgorithmRegion::GetAllDatas(vector<double> &parameters, vector<double> &resultXs, vector<double> &resultYs, vector<double> &resultRs)
{
	// Local control variables 
	HTuple  hv_X_Mark, hv_Y_Mark, hv_r_Mark, hv_i;
	HTuple  hv_j, hv_Index, hv_X_point, hv_Y_point, hv_R_point;

	//输入
	hv_X_Mark = parameters[0];
	hv_Y_Mark = parameters[1];
	hv_r_Mark = parameters[2];

	resultXs.clear();
	resultYs.clear();
	resultRs.clear();
	for (hv_i=0; hv_i<=5; hv_i+=1)
	{
		for (hv_j=0; hv_j<=7; hv_j+=1)
		{
			hv_Index = (hv_i*8)+hv_j;
			hv_X_point[hv_Index] = hv_Y_Mark+(20*hv_j);
			hv_Y_point[hv_Index] = hv_Y_Mark+(20*hv_i);
			hv_R_point[hv_Index] = hv_r_Mark;

			resultXs.push_back((double)hv_X_point[hv_Index]);
			resultYs.push_back((double)hv_Y_point[hv_Index]);
			resultRs.push_back((double)hv_R_point[hv_Index]);
		}
	}
	//输出
	hv_X_point = hv_X_point;
	hv_Y_point = hv_Y_point;
	hv_R_point = hv_R_point;
	return true;
}

//保存参数数据比较
bool CalgorithmRegion::CompareParameters(ModelParameter &saveParameter, ModelParameter &Parameter)
{
	//模板名称
	if(saveParameter.model_Name != Parameter.model_Name)
		return false;
	if(saveParameter.model_Path != Parameter.model_Path)
		return false;
	if(fabs((double)(saveParameter.hv_Row1 - Parameter.hv_Row1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Column1 - Parameter.hv_Column1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Row2 - Parameter.hv_Row2))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Column2 - Parameter.hv_Column2))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_AStart - Parameter.hv_AStart))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_ARange - Parameter.hv_ARange))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Angle - Parameter.hv_Angle ))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Row - Parameter.hv_Row ))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Column - Parameter.hv_Column))>0.000001)
		return false;
	//模板矩形参数
	if(fabs((double)(saveParameter.hv_Row_center_rec_model - Parameter.hv_Row_center_rec_model))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Column_center_rec_model - Parameter.hv_Column_center_rec_model))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Length1_rec_model - Parameter.hv_Length1_rec_model))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Length2_rec_model - Parameter.hv_Length2_rec_model))>0.000001)
		return false;
	//模板重心坐标
	if(fabs((double)(saveParameter.hv_Row_gravity_model - Parameter.hv_Row_gravity_model))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Column_gravity_model - Parameter.hv_Column_gravity_model))>0.000001)
		return false;
	//图像增强对比参数
	if(fabs((double)(saveParameter.hv_Mult - Parameter.hv_Mult))>0.000001)
		return false;
	if(fabs((double)(saveParameter.hv_Add - Parameter.hv_Add))>0.000001)
		return false;
	if(fabs((double)(saveParameter.is_Mult - Parameter.is_Mult))>0.000001)
		return false;
	//是否中值滤波(fabs((double)(去掉小孔，细缝))>0.000001)
	if(fabs((double)(saveParameter.hv_median_bool - Parameter.hv_median_bool))>0.000001)
		return false;
	//Parameter_median是滤波器掩码
	if(fabs((double)(saveParameter.hv_Parameter_median - Parameter.hv_Parameter_median))>0.000001)
		return false;
	//识别方式
	if(saveParameter.check != Parameter.check)
		return false;
	//矩形参数
	if(fabs((double)(saveParameter.rectsParameter.hv_Row_rectangle_out - Parameter.rectsParameter.hv_Row_rectangle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Row_rectangle_out - Parameter.rectsParameter.hv_Row_rectangle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Phi_rectangle_out - Parameter.rectsParameter.hv_Phi_rectangle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Length1_rectangle_out - Parameter.rectsParameter.hv_Length1_rectangle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Length2_rectangle_out - Parameter.rectsParameter.hv_Length2_rectangle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Row_rectangle_in - Parameter.rectsParameter.hv_Row_rectangle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Row_rectangle_in - Parameter.rectsParameter.hv_Row_rectangle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Phi_rectangle_in - Parameter.rectsParameter.hv_Phi_rectangle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Length1_rectangle_in - Parameter.rectsParameter.hv_Length1_rectangle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.rectsParameter.hv_Length2_rectangle_in - Parameter.rectsParameter.hv_Length2_rectangle_in))>0.000001)
		return false;
	//双圆弧参数
	if(fabs((double)(saveParameter.circlesParameter.hv_Row_circle_out - Parameter.circlesParameter.hv_Row_circle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Column_circle_out - Parameter.circlesParameter.hv_Column_circle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Radius_circle_out - Parameter.circlesParameter.hv_Radius_circle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Row_circle_in - Parameter.circlesParameter.hv_Row_circle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Column_circle_in - Parameter.circlesParameter.hv_Column_circle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Radius_circle_in - Parameter.circlesParameter.hv_Radius_circle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Row_circle_out1 - Parameter.circlesParameter.hv_Row_circle_out1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Column_circle_out1 - Parameter.circlesParameter.hv_Column_circle_out1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Radius_circle_out1 - Parameter.circlesParameter.hv_Radius_circle_out1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Row_circle_in1 - Parameter.circlesParameter.hv_Row_circle_in1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Column_circle_in1 - Parameter.circlesParameter.hv_Column_circle_in1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circlesParameter.hv_Radius_circle_in1 - Parameter.circlesParameter.hv_Radius_circle_in1))>0.000001)
		return false;
	//双直线参数
	if(fabs((double)(saveParameter.linesParameter.hv_Row_rec - Parameter.linesParameter.hv_Row_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Column_rec - Parameter.linesParameter.hv_Column_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Phi_rec - Parameter.linesParameter.hv_Phi_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Length1_rec - Parameter.linesParameter.hv_Length1_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Length2_rec - Parameter.linesParameter.hv_Length2_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Row_rec1 - Parameter.linesParameter.hv_Row_rec1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Column_rec1 - Parameter.linesParameter.hv_Column_rec1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Phi_rec1 - Parameter.linesParameter.hv_Phi_rec1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Length1_rec1 - Parameter.linesParameter.hv_Length1_rec1))>0.000001)
		return false;
	if(fabs((double)(saveParameter.linesParameter.hv_Length2_rec1 - Parameter.linesParameter.hv_Length2_rec1))>0.000001)
		return false;
	//单直线参数
	if(fabs((double)(saveParameter.lineParameter.hv_Row_rec - Parameter.lineParameter.hv_Row_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.lineParameter.hv_Column_rec - Parameter.lineParameter.hv_Column_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.lineParameter.hv_Phi_rec - Parameter.lineParameter.hv_Phi_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.lineParameter.hv_Length1_rec - Parameter.lineParameter.hv_Length1_rec))>0.000001)
		return false;
	if(fabs((double)(saveParameter.lineParameter.hv_Length2_rec - Parameter.lineParameter.hv_Length2_rec))>0.000001)
		return false;
	//单圆弧参数
	if(fabs((double)(saveParameter.circleParameter.hv_Row_circle_out - Parameter.circleParameter.hv_Row_circle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circleParameter.hv_Column_circle_out - Parameter.circleParameter.hv_Column_circle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circleParameter.hv_Radius_circle_out - Parameter.circleParameter.hv_Radius_circle_out))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circleParameter.hv_Row_circle_in - Parameter.circleParameter.hv_Row_circle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circleParameter.hv_Column_circle_in - Parameter.circleParameter.hv_Column_circle_in))>0.000001)
		return false;
	if(fabs((double)(saveParameter.circleParameter.hv_Radius_circle_in - Parameter.circleParameter.hv_Radius_circle_in))>0.000001)
		return false;

	return true;
}
bool CalgorithmRegion::CompareUpParameters(ModelParameter &Parameter)
{
	ModelParameter saveParameter;
	SetUpModel(Parameter.model_Name, saveParameter);	
	return CompareParameters(saveParameter, Parameter);
}
bool CalgorithmRegion::CompareDownParameters(ModelParameter &Parameter)
{
	ModelParameter saveParameter;
	SetDownModel(Parameter.model_Name, saveParameter);	
	return CompareParameters(saveParameter, Parameter);
}