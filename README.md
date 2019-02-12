# PSAdemo

## 工程简介

### 机器视觉定位：基于模板匹配，指定标志模板的相对区域，对直线、矩形、圆弧的二次识别定位实例

## 使用演示

- **模板匹配**
   ![](https://github.com/HQlin/PSAdemo/blob/master/gif/模板匹配.gif "模板匹配")
- **双直线匹配**
   ![](https://github.com/HQlin/PSAdemo/blob/master/gif/双直线匹配.gif "双直线匹配")
- **矩形匹配**
   ![](https://github.com/HQlin/PSAdemo/blob/master/gif/矩形匹配.gif "矩形匹配")
- **单直线匹配**
   ![](https://github.com/HQlin/PSAdemo/blob/master/gif/单直线匹配.gif "单直线匹配")
- **单直线匹配**
   ![](https://github.com/HQlin/PSAdemo/blob/master/gif/单直线匹配.gif "单直线匹配")
- **单圆弧匹配**
   ![](https://github.com/HQlin/PSAdemo/blob/master/gif/单圆弧匹配.gif "单圆弧匹配")

## 工程详解

- **开发环境**
	
>Halcon11.0.1(win32) + vs2010

- **目录结构** 

>--Halcon

        |-CalgorithmRegion                              算法类	
        |-HalconBmp                                     读图类
        |-HalconTool                                    显示图像文字类
		
>--mfcDlg

        |-MassageDlg                                    提示对话框		
        |-ModelBuildDlg                                 编辑模板对话框		
        |-readme.h                                      工程使用说明
	
- **使用说明** 

  **一.管理模板**
	
       1.所有模板：指显示已存在的模板。
		
       2.当前模板：指当前操作模板。

  **二,获取图像**
	
       1.保存编辑模板的原图像。

  **三.编辑模板**
	
       1.增强图像（非必选）。
		
       2.中值滤波（非必选）。
		
       3.匹配方式（必选）：其他匹配方式都是在模板匹配基础上，进行指定标志模板的相对区域，对直线、矩形、圆弧的二次识别；所有匹配
       方式都基于一基准点与基准线进行识别，并在底板图像与贴片图像上进行定位贴合（本工程以编辑底板模板为例，贴片相同）。
			
       4.匹配参数（模板匹配）
		
         4.1搜索范围：指在图像中固定的矩形区域进行识别模板，一般为全图范围。
			
         4.2选择模板：指被识别的标志模板（如果匹配方式为模板匹配，则需要勾选角度，使得标志模板具有角度属性；如果不勾选角度，标
         志模板都为零度）。
			
         4.3擦除：指对标志模板进行编辑，可擦除掉非标志模板部分（即擦除部分不进行识别），滑动条可控制橡皮擦大小。
			
         4.4开始角度、角度范围：指识别模板的角度范围（一般选择模板角度为中点，开始角度绝对值为角度范围的一半）。举例：选择模板
         为零度，开始角度为-10度，角度范围为20度，则识别模板角度范围为-10~10度，识别角度必须在该范围才算识别成功。
			
         4.5合格分数：指识别模板分数，识别分数必须大于合格分数才算识别成功。
			
         4.6保存模板：最后保存编辑模板所有数据，并显示原图像的识别效果。	

  **四.识别模板**
	
       1.使用已保存的编辑模板，获取一图像识别模板（0K为识别成功，NG为识别失败）。

## 联系信息

> Address：     **广州**  
> Email:        [**SwimYanglin@foxmail.com**][email-addr]  
> Github:       [**github.com/HQlin**][github-site]  

[email-addr]: mailto:SwimYanglin@foxmail.com
[github-site]: https://github.com/HQlin
