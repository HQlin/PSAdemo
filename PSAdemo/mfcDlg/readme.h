#pragma once

//��ֲ�赼��ȫ�ֱ�������h�ļ�
#include "../PSAdemo.h"

//1#����˵��
//�ļ���PSAdemo.h
//1.ȫ�ֱ�������
//extern HalconBmp *g_pBmp;								//��ͼ����
//extern HalconTool g_ToolDown;							//��ʾͼ�񹤾�
//extern bool isRunPic;									//�Ƿ���ͼ
//extern int g_SaveImage;								//ʶ���Ƿ񱣴�ͼ��(D:/ͼ�񱸷�/)
//extern CalgorithmRegion *g_pCalgorithmRegion;			//�����㷨����


//�ļ���PSAdemo.cpp
//2.ȫ�ֱ�����ʼ��
//CalgorithmRegion *g_pCalgorithmRegion;			//�����㷨����
//HalconBmp *g_pBmp;								//��ͼ����
//HalconTool g_ToolDown;							//��ʾͼ�񹤾�
////true ������ͼģʽ
////false ���ػ�ģʽ
//bool isRunPic = true;								//�Ƿ���ͼ
//int g_SaveImage = 0;								//ʶ���Ƿ񱣴�ͼ��


//�ļ���PSAdemoDlg.h
//3.���Ա����
//ģ�����
//CalgorithmRegion::ModelParameter DownParameter;


//�ļ���PSAdemoDlg.cpp

//2#��������
//Halcon11.0.1(win32) + vs2010

//3#���̽ṹ
/*
--Halcon
	|-CalgorithmRegion	�㷨��
	|-HalconBmp			��ͼ��
	|-HalconTool		��ʾͼ��������
--mfcDlg
	|-MassageDlg		��ʾ�Ի���
	|-ModelBuildDlg		�༭ģ��Ի���
	|-readme.h			����ʹ��˵��
*/


//4#ʹ��˵��
/*
һ.����ģ��
1.����ģ�壺ָ��ʾ�Ѵ��ڵ�ģ�塣
2.��ǰģ�壺ָ��ǰ����ģ�塣

��,��ȡͼ��
1.����༭ģ���ԭͼ��

��.�༭ģ��
1.��ǿͼ�񣨷Ǳ�ѡ����
2.��ֵ�˲����Ǳ�ѡ����
3.ƥ�䷽ʽ����ѡ����
	����ƥ�䷽ʽ������ģ��ƥ������ϣ�����ָ����־ģ���������򣬶�ֱ�ߡ����Ρ�Բ���Ķ���ʶ��
	����ƥ�䷽ʽ������һ��׼����һ��׼�߽���ʶ�𣬲��ڵװ�ͼ������Ƭͼ���Ͻ��ж�λ���ϣ��������Ա༭�װ�ģ��Ϊ������Ƭ��ͬ����
4.ƥ�������ģ��ƥ�䣩
4.1������Χ��ָ��ͼ���й̶��ľ����������ʶ��ģ�壬һ��Ϊȫͼ��Χ��
4.2ѡ��ģ�壺ָ��ʶ��ı�־ģ�壨���ƥ�䷽ʽΪģ��ƥ�䣬����Ҫ��ѡ�Ƕȣ�ʹ�ñ�־ģ����нǶ����ԣ��������ѡ�Ƕȣ���־ģ�嶼Ϊ��ȣ���
4.3������ָ�Ա�־ģ����б༭���ɲ������Ǳ�־ģ�岿�֣����������ֲ�����ʶ�𣩣��������ɿ�����Ƥ����С��
4.4��ʼ�Ƕȡ��Ƕȷ�Χ��ָʶ��ģ��ĽǶȷ�Χ��һ��ѡ��ģ��Ƕ�Ϊ�е㣬��ʼ�ǶȾ���ֵΪ�Ƕȷ�Χ��һ�룩��������ѡ��ģ��Ϊ��ȣ���ʼ�Ƕ�Ϊ-10�ȣ��Ƕȷ�ΧΪ20�ȣ���ʶ��ģ��Ƕȷ�ΧΪ-10~10�ȣ�ʶ��Ƕȱ����ڸ÷�Χ����ʶ��ɹ���
4.5�ϸ������ָʶ��ģ�������ʶ�����������ںϸ��������ʶ��ɹ���
4.6����ģ�壺��󱣴�༭ģ���������ݣ�����ʾԭͼ���ʶ��Ч����

��.ʶ��ģ��
1.ʹ���ѱ���ı༭ģ�壬��ȡһͼ��ʶ��ģ�壨0KΪʶ��ɹ���NGΪʶ��ʧ�ܣ���
*/