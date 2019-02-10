#include "StdAfx.h"
#include "HalconTool.h"

HalconTool::HalconTool(void)
{
	strReslut=L"";
	m_scale = 1;
}

HalconTool::~HalconTool(void)
{

}

void HalconTool::ShowImage(HObject& ho_Image){ 
	//图片长宽
	HTuple width, height;
	try{
		//旋转180度
		//RotateImage(ho_Image, &ho_Image, 0, "constant");
		//镜像
		//MirrorImage(ho_Image, &ho_Image, "row");
		GetImageSize(ho_Image, &width, &height); 

		if(m_scale>1 || m_scale<0)
			m_scale = 1;

		double height2 = height*m_scale;
		double width2 = width*m_scale;

		/*m_X = m_X * m_scale;
		m_Y = m_Y * m_scale;*/
		//计算左边剩余
		double leiflast = 0 - (width/2 - width2/2);
		if(m_X < leiflast)  
			m_X = leiflast;
		//计算右边剩余
		double rightlast = width - (width/2 + width2/2);
		if(m_X > rightlast) 
			m_X = rightlast;
		//计算上边剩余
		double toplast = 0 - (height/2 - height2/2);
		if(m_Y < toplast)  
			m_Y = toplast;
		//计算下边剩余
		double bottomlast = height - (height/2 + height2/2);
		if(m_Y > bottomlast) 
			m_Y = bottomlast;

		PartRow0 = height/2 - height2/2+ m_Y;
		PartCol0 = width/2 - width2/2 + m_X;
		PartRow1 = height/2 + height2/2 + m_Y;
		PartCol1 = width/2 + width2/2 + m_X;


		/*double PartRow0 = 0;
		double PartCol0 = 0;
		double PartRow1 = height;
		double PartCol1 = width;*/


		SetPart(hv_WindowHandle, PartRow0, PartCol0, PartRow1, PartCol1);
		DispObj(ho_Image, hv_WindowHandle);	
	}catch(...)
	{
	}
	try{
		//绘画十字架 
		SetColor(hv_WindowHandle,"yellow");
		HObject ho_Cross; 
		GenCrossContourXld(&ho_Cross, height/2, width/2, width, 0);
		DispObj(ho_Cross, hv_WindowHandle);	
	}catch(...)
	{
	}
	try{
		//绘画轮廓			
		for(int i=0;i<outlines.size();i++)
		{
			switch(i)
			{
			case 0:
				SetColor(hv_WindowHandle,"green");
				break;
			case 1:
				SetColor(hv_WindowHandle,"red");
				break;
			case 2:
				SetColor(hv_WindowHandle,"blue");
				break;
			default:
				SetColor(hv_WindowHandle,"yellow");
				break;
			}
			if(outlines.size()>0)
				DispObj(outlines[i], hv_WindowHandle);
		}	
	}catch(...)
	{
	}
	try{
		//绘画模拟底板轮廓			
		for(int i=0;i<downlines.size();i++)
		{
			SetColor(hv_WindowHandle,"green");
			DispObj(downlines[i], hv_WindowHandle);
		}	
		//绘画模拟贴片轮廓	
		for(int i=0;i<uplines.size();i++)
		{
			SetColor(hv_WindowHandle,"red");
			DispObj(uplines[i], hv_WindowHandle);
		}	
	}catch(...)
	{
	}
	SetColor(hv_WindowHandle,"red");
	//显示文字
	DisplayMessage(hv_WindowHandle, strReslut);
}

void HalconTool::SetWindow(CWnd * pCWnd){ 
	//定义显示的起点和长宽高      
	HTuple HWindowRow,HWindowColumn,HWindowWidth,HWindowHeight;     
	//定义窗口ID     
	HTuple HWindowID; 

	CRect rect;  
	pCWnd->GetClientRect(&rect);
	HWindowRow = rect.top;
	HWindowColumn = rect.left;
	HWindowWidth = rect.Width();
	HWindowHeight = rect.Height();
	HWindowID = (Hlong)pCWnd->m_hWnd;
	SetWindowAttr("background_color","black");
	SetCheck("~father");	
	OpenWindow(HWindowRow,HWindowColumn,HWindowWidth,HWindowHeight,HWindowID,"visible","",&hv_WindowHandle);
	SetCheck("father");
	SetDraw(hv_WindowHandle,"margin");
}

void HalconTool::SetWindow(CWnd * pCWnd0, CWnd * pCWnd1)
{	
	CRect rect0;  	
	//定义显示的起点和长宽高      
	HTuple HWindowRow0,HWindowColumn0,HWindowWidth0,HWindowHeight0;     
	//定义窗口ID     
	HTuple HWindowID0; 
	pCWnd0->GetClientRect(&rect0);
	HWindowRow0 = rect0.top;
	HWindowColumn0 = rect0.left;
	HWindowWidth0 = rect0.Width();
	HWindowHeight0 = rect0.Height();
	HWindowID0 = (Hlong)pCWnd0->m_hWnd;

	CRect rect1; 
	HTuple HWindowRow1,HWindowColumn1,HWindowWidth1,HWindowHeight1;         
	HTuple HWindowID1;
	pCWnd1->GetClientRect(&rect1);
	HWindowRow1 = rect1.top;
	HWindowColumn1 = rect1.left;
	HWindowWidth1 = rect1.Width();
	HWindowHeight1 = rect1.Height();
	HWindowID1 = (Hlong)pCWnd1->m_hWnd;

	SetWindowAttr("background_color","white");
	SetCheck("~father");	

	OpenWindow(HWindowRow0,HWindowColumn0,HWindowWidth0,HWindowHeight0,HWindowID0,"visible","",&WindowHandle0);
	OpenWindow(HWindowRow1,HWindowColumn1,HWindowWidth1,HWindowHeight1,HWindowID1,"visible","",&WindowHandle1);

	SetCheck("father");

	SetDraw(WindowHandle0,"margin");
	SetDraw(WindowHandle1,"margin");
}

void HalconTool::ShowLinearImage(const HObject& ho_Image){ 
	try{
		//图片长宽
		HTuple width, height;
		GetImageSize(ho_Image, &height, &width); 

		//分图显示
		HObject ho_ImageRotate;
		RotateImage(ho_Image, &ho_ImageRotate,90, "nearest_neighbor");
		GetImageSize(ho_ImageRotate, &height, &width); 
		SetPart(WindowHandle0,0, 0, width, height/2);
		SetPart(WindowHandle1,0, height/2, width, height);
		DispObj(ho_ImageRotate, WindowHandle0);
		DispObj(ho_ImageRotate, WindowHandle1);
	}catch( HException& except){
		CString ProcName(except.ProcName().Text());
		CString ErrorText(except.ErrorText().Text());
		TRACE(L"lin: " + ProcName + L", " + ErrorText);		
	}
}

//在窗口显示文字
void HalconTool::DisplayMessage(HTuple hv_WindowHandle, CString cString, HTuple hv_CoordSystem, 
	HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box)
{
	USES_CONVERSION;
	//进行转换
	HTuple hv_String = T2A(cString.GetBuffer(0));
	cString.ReleaseBuffer();

	// Local control variables 
	HTuple  hv_Red, hv_Green, hv_Blue, hv_Row1Part;
	HTuple  hv_Column1Part, hv_Row2Part, hv_Column2Part, hv_RowWin;
	HTuple  hv_ColumnWin, hv_WidthWin, hv_HeightWin, hv_MaxAscent;
	HTuple  hv_MaxDescent, hv_MaxWidth, hv_MaxHeight, hv_R1;
	HTuple  hv_C1, hv_FactorRow, hv_FactorColumn, hv_Width;
	HTuple  hv_Index, hv_Ascent, hv_Descent, hv_W, hv_H, hv_FrameHeight;
	HTuple  hv_FrameWidth, hv_R2, hv_C2, hv_DrawMode, hv_Exception;
	HTuple  hv_CurrentColor;

	//This procedure displays text in a graphics window.
	//
	//Input parameters:
	//WindowHandle: The WindowHandle of the graphics window, where
	//   the message should be displayed
	//String: A tuple of strings containing the text message to be displayed
	//CoordSystem: If set to 'window', the text position is given
	//   with respect to the window coordinate system.
	//   If set to 'image', image coordinates are used.
	//   (This may be useful in zoomed images.)
	//Row: The row coordinate of the desired text position
	//   If set to -1, a default value of 12 is used.
	//Column: The column coordinate of the desired text position
	//   If set to -1, a default value of 12 is used.
	//Color: defines the color of the text as string.
	//   If set to [], '' or 'auto' the currently set color is used.
	//   If a tuple of strings is passed, the colors are used cyclically
	//   for each new textline.
	//Box: If set to 'true', the text is written within a white box.
	//
	//prepare window
	GetRgb(hv_WindowHandle, &hv_Red, &hv_Green, &hv_Blue);
	GetPart(hv_WindowHandle, &hv_Row1Part, &hv_Column1Part, &hv_Row2Part, &hv_Column2Part);
	GetWindowExtents(hv_WindowHandle, &hv_RowWin, &hv_ColumnWin, &hv_WidthWin, &hv_HeightWin);
	SetPart(hv_WindowHandle, 0, 0, hv_HeightWin-1, hv_WidthWin-1);
	//
	//default settings
	if (0 != (hv_Row==-1))
	{
		hv_Row = 12;
	}
	if (0 != (hv_Column==-1))
	{
		hv_Column = 12;
	}
	if (0 != (hv_Color==HTuple()))
	{
		hv_Color = "";
	}
	//
	hv_String = ((""+hv_String)+"").TupleSplit("\n");
	//
	//Estimate extentions of text depending on font size.
	GetFontExtents(hv_WindowHandle, &hv_MaxAscent, &hv_MaxDescent, &hv_MaxWidth, &hv_MaxHeight);
	if (0 != (hv_CoordSystem==HTuple("window")))
	{
		hv_R1 = hv_Row;
		hv_C1 = hv_Column;
	}
	else
	{
		//transform image to window coordinates
		hv_FactorRow = (1.*hv_HeightWin)/((hv_Row2Part-hv_Row1Part)+1);
		hv_FactorColumn = (1.*hv_WidthWin)/((hv_Column2Part-hv_Column1Part)+1);
		hv_R1 = ((hv_Row-hv_Row1Part)+0.5)*hv_FactorRow;
		hv_C1 = ((hv_Column-hv_Column1Part)+0.5)*hv_FactorColumn;
	}
	//
	//display text box depending on text size
	if (0 != (hv_Box==HTuple("true")))
	{
		//calculate box extents
		hv_String = (" "+hv_String)+" ";
		hv_Width = HTuple();
		HTuple end_val57 = (hv_String.TupleLength())-1;
		HTuple step_val57 = 1;
		for (hv_Index=0; hv_Index.Continue(end_val57, step_val57); hv_Index += step_val57)
		{
			GetStringExtents(hv_WindowHandle, HTuple(hv_String[hv_Index]), &hv_Ascent, 
				&hv_Descent, &hv_W, &hv_H);
			hv_Width = hv_Width.TupleConcat(hv_W);
		}
		hv_FrameHeight = hv_MaxHeight*(hv_String.TupleLength());
		hv_FrameWidth = (HTuple(0).TupleConcat(hv_Width)).TupleMax();
		hv_R2 = hv_R1+hv_FrameHeight;
		hv_C2 = hv_C1+hv_FrameWidth;
		//display rectangles
		GetDraw(hv_WindowHandle, &hv_DrawMode);
		SetDraw(hv_WindowHandle, "fill");
		SetColor(hv_WindowHandle, "light gray");
		DispRectangle1(hv_WindowHandle, hv_R1+3, hv_C1+3, hv_R2+3, hv_C2+3);
		SetColor(hv_WindowHandle, "white");
		DispRectangle1(hv_WindowHandle, hv_R1, hv_C1, hv_R2, hv_C2);
		SetDraw(hv_WindowHandle, hv_DrawMode);
	}
	else if (0 != (hv_Box!=HTuple("false")))
	{
		hv_Exception = "Wrong value of control parameter Box";
		throw HalconCpp::HException(hv_Exception);
	}
	//Write text.
	HTuple end_val78 = (hv_String.TupleLength())-1;
	HTuple step_val78 = 1;
	for (hv_Index=0; hv_Index.Continue(end_val78, step_val78); hv_Index += step_val78)
	{
		hv_CurrentColor = ((const HTuple&)hv_Color)[hv_Index%(hv_Color.TupleLength())];
		if (0 != (HTuple(hv_CurrentColor!=HTuple("")).TupleAnd(hv_CurrentColor!=HTuple("auto"))))
		{
			SetColor(hv_WindowHandle, hv_CurrentColor);
		}
		else
		{
			SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
		}
		hv_Row = hv_R1+(hv_MaxHeight*hv_Index);
		SetTposition(hv_WindowHandle, hv_Row, hv_C1);
		WriteString(hv_WindowHandle, HTuple(hv_String[hv_Index]));
	}
	//reset changed window settings
	SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
	SetPart(hv_WindowHandle, hv_Row1Part, hv_Column1Part, hv_Row2Part, hv_Column2Part);
	return;
}