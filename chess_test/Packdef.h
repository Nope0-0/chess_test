#pragma once

#define GRID_NUM    15 //ÿһ��(��)�����̽�����
#define GRID_COUNT  225//�����Ͻ�������
#define BLACK        0 //������0��ʾ
#define WHITE        1 //������1��ʾ
#define NOSTONE     '+'  //û������
//����궨�������Դ��������͵�����
#define STWO        1  //�߶�
#define STHREE        2  //����
#define SFOUR        3  //����
#define TWO        4  //���
#define THREE        5  //����
#define FOUR        6  //����
#define FIVE        7  //����
#define NOTYPE        11 //δ����
#define ANALSISED   255//�ѷ�������
#define TOBEANALSIS 0  //�ѷ�������
//��������Լ��ĳһ�����Ƿ��������ϵ���Ч���ӵ�
#define IsValidPos(x,y) ((x>=0 && x<GRID_NUM) && (y>=0 && y<GRID_NUM)