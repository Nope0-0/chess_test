#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Packdef.h"
using namespace std;

//������ö���͵��������ͣ���ȷ���±߽磬�ϱ߽�
enum ENTRY_TYPE{exact,lower_bound,upper_bound};
//��ϣ����Ԫ�صĽṹ����
typedef struct HASHITEM
{
	__int64 checksum;		//64λУ����
	ENTRY_TYPE entry_type;	//��������
	short depth;			//ȡ�ô�ֵʱ�Ĳ��
	short eval;				//�ڵ��ֵ
}HashItem;

typedef struct Node
{
	int x;
	int y;
}POINT;

//���Ա�ʾ����λ�õĽṹ
typedef struct _stoneposition
{
	unsigned char x;
	unsigned char y;
}STONEPOS;

typedef struct _movestone
{
	unsigned char nRenjuID;
	POINT ptMovePoint;
}MOVESTONE;
//����ṹ���Ա�ʾ�߷�

typedef struct _stonemove
{
	STONEPOS StonePos;	//����λ��
	int Score;			//�߷��ķ���
}STONEMOVE;
//=================================================================//
int AnalysisLine(unsigned char* position,int GridNum,int StonePos);
int AnalysisRight(unsigned char position[][GRID_NUM],int i,int j);
int AnalysisLeft(unsigned char position[][GRID_NUM],int i,int j);
int AnalysisVertical(unsigned char position[][GRID_NUM],int i,int j);
int AnalysisHorizon(unsigned char position[][GRID_NUM],int i,int j);
int Eveluate(unsigned int position[][GRID_NUM],bool bIsWhiteTurn);
int AddMove(int nToX, int nToY, int nPly);
int CreatePossibleMove(unsigned char position[][GRID_NUM], int nPly, int nSide);
void MergeSort(STONEMOVE* source,int n,bool direction);
void MergePass(STONEMOVE* source,STONEMOVE* target,const int s,const int n,const bool direction);
void Merge_A(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);
void Merge(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);
void EnterHistoryScore(STONEMOVE* move,int depth);
int GetHistoryScore(STONEMOVE* move);
void ResetHistoryTable();
int NegaScout(int depth,int alpha,int beta);
void SearchAGoodMove(unsigned char position[][GRID_NUM],int Type);
int IsGameOver(unsigned char position[][GRID_NUM],int nDepth);
void UnMakeMove(STONEMOVE* move);
unsigned char MakeMove(STONEMOVE* move,int type);
void _CSearchEngine();
void InitializeHashKey();
void EnterHashTable(ENTRY_TYPE entry_type, short eval, short depth, int TableNo);
int LookUpHashTable(int alpha, int beta, int depth, int TableNo);
void Hash_UnMakeMove(STONEMOVE *move,unsigned char CurPosition[][GRID_NUM]);
void Hash_MakeMove(STONEMOVE *move,unsigned char CurPosition[][GRID_NUM]);
void CalculateInitHashKey(unsigned char CurPosition[][GRID_NUM]);
__int64 rand64();
long rand32();
void CTranspositionTable();
void _CTranspositionTable();
bool OnInitDialog();
//=================================================================//
int m_HistoryTable[GRID_NUM][GRID_NUM];			//��ʷ�÷ֱ�
STONEMOVE m_TargetBuff[225];					//�����õĻ������
unsigned int m_nHashKey32[15][10][9];			//32λ������飬��������32λ��ϣֵ
unsigned __int64 m_ulHashKey64[15][10][9];		//64λ������飬��������64λ��ϣֵ
HashItem *m_pTT[10];							//�û���ͷָ��
unsigned int m_HashKey32;						//32λ��ϣֵ
__int64 m_HashKey64;							//64 λ��ϣֵ
STONEMOVE m_MoveList[10][225];					//���Լ�¼�߷�������
unsigned char m_LineRecord[30];					//���AnalysisLine�������������
int TypeRecord[GRID_NUM ][GRID_NUM][4];			//���ȫ���������������,������ά��,���ڴ��ˮƽ����ֱ����б����б 4 ���������������ͷ������
int TypeCount[2][20];							//���ͳ�ǹ��ķ������������
int m_nMoveCount;								//�˱������Լ�¼�߷�������
unsigned char CurPosition[GRID_NUM][GRID_NUM];	//����ʱ���ڵ�ǰ�ڵ�����״̬������
STONEMOVE m_cmBestMove;							//��¼����߷��ı���        
//CMoveGenerator* m_pMG;        //�߷�������ָ��        
//CEveluation* m_pEval;        //��ֵ����ָ��        
int m_nSearchDepth;								//����������
int m_nMaxDepth;								//��ǰ����������������
unsigned char m_RenjuBoard[GRID_NUM][GRID_NUM];	//�������飬������ʾ����
int m_nUserStoneColor;							//�û����ӵ���ɫ
//CSearchEngine* m_pSE;         //��������ָ��
int X,Y;										//AI�������λ�� 
int SL;											//ʤ����� 
//λ����Ҫ�Լ�ֵ��,�˱���м�����,Խ�����ֵԽ��
int PosValue[GRID_NUM][GRID_NUM]=
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,1,2,2,2,2,2,2,2,2,2,2,2,1,0},
	{0,1,2,3,3,3,3,3,3,3,3,3,2,1,0},
	{0,1,2,3,4,4,4,4,4,4,4,3,2,1,0},
	{0,1,2,3,4,5,5,5,5,5,4,3,2,1,0},
	{0,1,2,3,4,5,6,6,6,5,4,3,2,1,0},
	{0,1,2,3,4,5,6,7,6,5,4,3,2,1,0},
	{0,1,2,3,4,5,6,6,6,5,4,3,2,1,0},
	{0,1,2,3,4,5,5,5,5,5,4,3,2,1,0},
	{0,1,2,3,4,4,4,4,4,4,4,3,2,1,0},
	{0,1,2,3,3,3,3,3,3,3,3,3,2,1,0},
	{0,1,2,2,2,2,2,2,2,2,2,2,2,1,0},
	{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

char mark[20]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E'};

//ȫ�ֱ���,����ͳ�ƹ�ֵ������ִ�б���
int icount=0;

int Eveluate(unsigned char position[][GRID_NUM],bool bIsWhiteTurn)
{
	int i,j,k;
	unsigned char nStoneType;
	icount++;//�������ۼ�

	//������ͷ������
	memset(TypeRecord,TOBEANALSIS,GRID_COUNT*4*4);
	memset(TypeCount,0,40*4);

	for(i=0;i<GRID_NUM;i++)
		for(j=0;j<GRID_NUM;j++)
		{
			if(position[i][j]!=NOSTONE)
			{
				//���ˮƽ������û�з�����
				if(TypeRecord[i][j][0]==TOBEANALSIS)
					AnalysisHorizon(position,i,j);

				//�����ֱ������û�з�����
				if(TypeRecord[i][j][1]==TOBEANALSIS)
					AnalysisVertical(position,i,j);

				//�����б������û�з�����
				if(TypeRecord[i][j][2]==TOBEANALSIS)
					AnalysisLeft(position,i,j);

				//�����б������û�з�����
				if(TypeRecord[i][j][3]==TOBEANALSIS)
					AnalysisRight(position,i,j);
			}
		}

		//�Է����������ͳ��,�õ�ÿ�����͵�����
		for(i=0;i<GRID_NUM;i++)
			for(j=0;j<GRID_NUM;j++)
				for(k =0;k<4;k++)
				{
					nStoneType=position[i][j];
					if(nStoneType!=NOSTONE)
					{
						switch(TypeRecord[i][j][k])
						{
						case FIVE://����
							TypeCount[nStoneType][FIVE]++;
							break;
						case FOUR://����
							TypeCount[nStoneType][FOUR]++;
							break;
						case SFOUR://����
							TypeCount[nStoneType][SFOUR]++;
							break;
						case THREE://����
							TypeCount[nStoneType][THREE]++;
							break;
						case STHREE://����
							TypeCount[nStoneType][STHREE]++;
							break;
						case TWO://���
							TypeCount[nStoneType][TWO]++;
							break;
						case STWO://�߶�
							TypeCount[nStoneType][STWO]++;
							break;
						default:
							break;
						}  
					}
				}

				//���������,���ؼ�ֵ
				if(bIsWhiteTurn)
				{
					if(TypeCount[BLACK][FIVE])
						return -9999;
					if(TypeCount[WHITE][FIVE])
						return 9999;
				}
				else
				{
					if(TypeCount[BLACK][FIVE])
						return 9999;
					if(TypeCount[WHITE][FIVE])
						return -9999;
				}
				//�������ĵ���һ������
				if(TypeCount[WHITE][SFOUR]>1)
					TypeCount[WHITE][FOUR]++;
				if(TypeCount[BLACK][SFOUR]>1)
					TypeCount[BLACK][FOUR]++;
				int WValue=0,BValue=0;

				if(bIsWhiteTurn)//�ֵ�������
				{        
					if(TypeCount[WHITE][FOUR])    
						return 9990;//����,��ʤ���ؼ�ֵ
					if(TypeCount[WHITE][SFOUR])  
						return 9980;//����,��ʤ���ؼ�ֵ
					if(TypeCount[BLACK][FOUR])
						return -9970;//���޳��Ļ���,�����л���,��ʤ���ؼ�ֵ
					if(TypeCount[BLACK][SFOUR] && TypeCount[BLACK][THREE])
						return -9960;//�����г��ĺͻ���,��ʤ���ؼ�ֵ
					if(TypeCount[WHITE][THREE] && TypeCount[BLACK][SFOUR]== 0)    
						return 9950;//���л�������û����,��ʤ���ؼ�ֵ
					if(TypeCount[BLACK][THREE]>1 && TypeCount[WHITE][SFOUR]==0 && TypeCount[WHITE][THREE]==0 && TypeCount[WHITE][STHREE]==0)
						return -9940;//�ڵĻ�������һ��,�������ĺ���,��ʤ���ؼ�ֵ
					if(TypeCount[WHITE][THREE]>1)
						WValue+=2000;//�׻�������һ��,�����ֵ��2000
					else
						//��������ֵ��200
						if(TypeCount[WHITE][THREE])
							WValue+=200;
					if(TypeCount[BLACK][THREE]>1)
						BValue+=500;//�ڵĻ�������һ��,�����ֵ��500
					else
						//��������ֵ��100
						if(TypeCount[BLACK][THREE])
							BValue+=100;
					//ÿ��������10
					if(TypeCount[WHITE][STHREE])
						WValue+=TypeCount[WHITE][STHREE]*10;
					//ÿ��������10
					if(TypeCount[BLACK][STHREE])
						BValue+=TypeCount[BLACK][STHREE]*10;
					//ÿ�������4
					if(TypeCount[WHITE][TWO])
						WValue+=TypeCount[WHITE][TWO]*4;
					//ÿ�������4
					if(TypeCount[BLACK][STWO])
						BValue+=TypeCount[BLACK][TWO]*4;
					//ÿ���߶���1
					if(TypeCount[WHITE][STWO])
						WValue+=TypeCount[WHITE][STWO];
					//ÿ���߶���1
					if(TypeCount[BLACK][STWO])
						BValue+=TypeCount[BLACK][STWO];
				}
				else//�ֵ�������
				{        
					if(TypeCount[BLACK][FOUR])
						return 9990;//����,��ʤ���ؼ�ֵ
					if(TypeCount[BLACK][SFOUR])
						return 9980;//����,��ʤ���ؼ�ֵ
					if(TypeCount[WHITE][FOUR])
						return -9970;//����,��ʤ���ؼ�ֵ
					if(TypeCount[WHITE][SFOUR] && TypeCount[WHITE][THREE])        
						return -9960;//���Ĳ�����,��ʤ���ؼ�ֵ

					if(TypeCount[BLACK][THREE] && TypeCount[WHITE][SFOUR]==0)
						return 9950;//�ڻ���,�����ġ���ʤ���ؼ�ֵ

					if(TypeCount[WHITE][THREE]>1 && TypeCount[BLACK][SFOUR]==0 && TypeCount[BLACK][THREE]==0 && TypeCount[BLACK][STHREE]==0)
						return -9940;//�׵Ļ�������һ��,�������ĺ���,��ʤ���ؼ�ֵ

					//�ڵĻ�������һ��,�����ֵ��2000
					if(TypeCount[BLACK][THREE]>1)
						BValue+=2000;
					else
						//��������ֵ��200
						if(TypeCount[BLACK][THREE])
							BValue+=200;

					//�׵Ļ�������һ��,�����ֵ�� 500
					if(TypeCount[WHITE][THREE]>1)
						WValue+=500;
					else
						//��������ֵ��100
						if(TypeCount[WHITE][THREE])
							WValue+=100;

					//ÿ��������10
					if(TypeCount[WHITE][STHREE])
						WValue+=TypeCount[WHITE][STHREE]*10;
					//ÿ��������10
					if(TypeCount[BLACK][STHREE])
						BValue+=TypeCount[BLACK][STHREE]*10;

					//ÿ�������4
					if(TypeCount[WHITE][TWO])
						WValue+=TypeCount[WHITE][TWO]*4;
					//ÿ�������4
					if(TypeCount[BLACK][STWO])
						BValue+=TypeCount[BLACK][TWO]*4;

					//ÿ���߶���1
					if(TypeCount[WHITE][STWO])
						WValue+=TypeCount[WHITE][STWO];
					//ÿ���߶���1
					if(TypeCount[BLACK][STWO])
						BValue+=TypeCount[BLACK][STWO];
				}

				//�����������ӵ�λ�ü�ֵ
				for(i=0;i<GRID_NUM;i++)
					for(j=0;j<GRID_NUM;j++)
					{
						nStoneType=position[i][j];
						if(nStoneType!=NOSTONE)
							if(nStoneType==BLACK)
								BValue+=PosValue[i][j];
							else
								WValue+=PosValue[i][j];
					}

					//���ع�ֵ
					if(!bIsWhiteTurn)
						return BValue-WValue;
					else
						return WValue-BValue;
}

//����������ĳ����ˮƽ�����ϵ�����
int AnalysisHorizon(unsigned char position[][GRID_NUM],int i,int j)
{
	//����ֱ�߷�����������
	AnalysisLine(position[i],15,j);
	//ʰȡ�������
	for(int s=0;s<15;s++)
		if(m_LineRecord[s]!=TOBEANALSIS)
			TypeRecord[i][s][0]= m_LineRecord[s];

	return TypeRecord[i][j][0];
}

//����������ĳ���ڴ�ֱ�����ϵ�����
int AnalysisVertical(unsigned char position[][GRID_NUM],int i,int j)
{
	unsigned char tempArray[GRID_NUM];
	//����ֱ�����ϵ�����ת��һά����
	for(int k=0;k<GRID_NUM;k++)
		tempArray[k]=position[k][j];
	//����ֱ�߷�����������
	AnalysisLine(tempArray,GRID_NUM,i);
	//ʰȡ�������
	for(int s=0;s<GRID_NUM;s++)
		if(m_LineRecord[s]!=TOBEANALSIS)
			TypeRecord[s][j][1]=m_LineRecord[s];

	return TypeRecord[i][j][1];
}

//����������ĳ������б�����ϵ�����
int AnalysisLeft(unsigned char position[][GRID_NUM],int i,int j)
{
	unsigned char tempArray[GRID_NUM];
	int x,y;
	int k;
	if(i<j)
	{
		y=0;
		x=j-i;
	}
	else
	{
		x=0;
		y=i-j;
	}
	//��б�����ϵ�����ת��һά����
	for(k=0;k<GRID_NUM;k++)
	{
		if(x+k>14 || y+k>14)
			break;
		tempArray[k]=position[y+k][x+k];
	}
	//����ֱ�߷�����������
	AnalysisLine(tempArray,k,j-x);
	//ʰȡ�������
	for(int s=0;s<k;s++)
		if(m_LineRecord[s]!=TOBEANALSIS)
			TypeRecord[y+s][x+s][2]=m_LineRecord[s];

	return TypeRecord[i][j][2];
}

//����������ĳ������б�����ϵ�����
int AnalysisRight(unsigned char position[][GRID_NUM],int i,int j)
{
	unsigned char tempArray[GRID_NUM];
	int x,y,realnum;
	int k;
	if(14-i<j)
	{
		y=14;
		x=j-14+i;
		realnum=14-i;
	}
	else
	{
		x=0;
		y=i+j;
		realnum=j;
	}
	//��б�����ϵ�����ת��һά����
	for(k=0;k<GRID_NUM;k++)
	{
		if(x+k>14 || y-k<0)
			break;
		tempArray[k]=position[y-k][x+k];
	}
	//����ֱ�߷�����������
	AnalysisLine(tempArray,k,j-x);
	//ʰȡ�������
	for(int s=0;s<k;s++)
		if(m_LineRecord[s]!=TOBEANALSIS)
			TypeRecord[y-s][x+s][3]=m_LineRecord[s];

	return TypeRecord[i][j][3];
}

int AnalysisLine(unsigned char* position,int GridNum,int StonePos)
{
	unsigned char StoneType;
	unsigned char AnalyLine[30];
	int nAnalyPos;
	int LeftEdge,RightEdge;
	int LeftRange,RightRange;

	if(GridNum<5)
	{
		//���鳤��С��5û������
		memset(m_LineRecord,ANALSISED,GridNum);
		return 0;
	}
	nAnalyPos=StonePos;
	memset(m_LineRecord,TOBEANALSIS,30);
	memset(AnalyLine,0x0F,30);
	//����������װ��AnalyLine;
	memcpy(&AnalyLine,position,GridNum);
	GridNum--;
	StoneType=AnalyLine[nAnalyPos];
	LeftEdge=nAnalyPos;
	RightEdge=nAnalyPos;
	//������������߽�
	while(LeftEdge>0)
	{
		if(AnalyLine[LeftEdge-1]!=StoneType)
			break;
		LeftEdge--;
	}

	//�����������ұ߽�
	while(RightEdge<GridNum)
	{
		if(AnalyLine[RightEdge+1]!=StoneType)
			break;
		RightEdge++;
	}
	LeftRange=LeftEdge;
	RightRange=RightEdge;
	//��������ѭ��������ӿ��µķ�Χ
	while(LeftRange>0)
	{
		if(AnalyLine[LeftRange -1]==!StoneType)
			break;
		LeftRange--;
	}
	while(RightRange<GridNum)
	{
		if(AnalyLine[RightRange+1]==!StoneType)
			break;
		RightRange++;
	}
	//����˷�ΧС��4�����û������
	if(RightRange-LeftRange<4)
	{
		for(int k=LeftRange;k<=RightRange;k++)
			m_LineRecord[k]=ANALSISED;
		return false;
	}
	//������������Ϊ��������,��ֹ�ظ�������һ����
	for(int k=LeftEdge;k<=RightEdge;k++)
		m_LineRecord[k]=ANALSISED;
	if(RightEdge-LeftEdge>3)
	{
		//���������������Ϊ����
		m_LineRecord[nAnalyPos]=FIVE;
		return FIVE;
	}

	if(RightEdge-LeftEdge== 3)
	{
		//���������������Ϊ����
		bool Leftfour=false;
		if(LeftEdge>0)
			if(AnalyLine[LeftEdge-1]==NOSTONE)
				Leftfour=true;//�������

		if(RightEdge<GridNum)
			//�ұ�δ���߽�
				if(AnalyLine[RightEdge+1]==NOSTONE)
					//�ұ�����
						if(Leftfour==true)//���������
							m_LineRecord[nAnalyPos]=FOUR;//����
						else
							m_LineRecord[nAnalyPos]=SFOUR;//����
				else
					if(Leftfour==true)//���������
						m_LineRecord[nAnalyPos]=SFOUR;//����
					else
						if(Leftfour==true)//���������
							m_LineRecord[nAnalyPos]=SFOUR;//����

		return m_LineRecord[nAnalyPos];
	}

	if(RightEdge-LeftEdge==2)
	{
		//���������������Ϊ����
		bool LeftThree=false;

		if(LeftEdge>1)
			if(AnalyLine[LeftEdge-1]==NOSTONE)
				//�������
					if(LeftEdge>1 && AnalyLine[LeftEdge-2]==AnalyLine[LeftEdge])
					{
						//��߸�һ�հ��м�������
						m_LineRecord[LeftEdge]=SFOUR;//����
						m_LineRecord[LeftEdge-2]=ANALSISED;
					}
					else
						LeftThree=true;

		if(RightEdge<GridNum)
			if(AnalyLine[RightEdge+1]==NOSTONE)
				//�ұ�����
					if(RightEdge<GridNum-1 && AnalyLine[RightEdge+2]==AnalyLine[RightEdge])
					{
						//�ұ߸�1����������
						m_LineRecord[RightEdge]=SFOUR;//����
						m_LineRecord[RightEdge+2]=ANALSISED;
					}
					else
						if(LeftThree==true)//���������
							m_LineRecord[RightEdge]=THREE;//����
						else
							m_LineRecord[RightEdge]=STHREE; //����
			else
			{
				if(m_LineRecord[LeftEdge]==SFOUR)//�������
					return m_LineRecord[LeftEdge];//����

				if(LeftThree==true)//���������
					m_LineRecord[nAnalyPos]=STHREE;//����
			}
		else
		{
			if(m_LineRecord[LeftEdge]==SFOUR)//�������
				return m_LineRecord[LeftEdge];//����
			if(LeftThree==true)//���������
				m_LineRecord[nAnalyPos]=STHREE;//����
		}

		return m_LineRecord[nAnalyPos];
	}

	if(RightEdge-LeftEdge==1)
	{
		//���������������Ϊ����
		bool Lefttwo=false;
		bool Leftthree=false;

		if(LeftEdge>2)
			if(AnalyLine[LeftEdge-1]==NOSTONE)
				//�������
					if(LeftEdge-1>1 && AnalyLine[LeftEdge-2]==AnalyLine[LeftEdge])
						if(AnalyLine[LeftEdge-3]==AnalyLine[LeftEdge])
						{
							//��߸�2����������
							m_LineRecord[LeftEdge-3]=ANALSISED;
							m_LineRecord[LeftEdge-2]=ANALSISED;
							m_LineRecord[LeftEdge]=SFOUR;//����
						}
						else
							if(AnalyLine[LeftEdge-3]==NOSTONE)
							{
								//��߸�1����������
								m_LineRecord[LeftEdge-2]=ANALSISED;
								m_LineRecord[LeftEdge]=STHREE;//����
							}
							else
								Lefttwo=true;

		if(RightEdge<GridNum-2)
			if(AnalyLine[RightEdge+1]==NOSTONE)
				//�ұ�����
					if(RightEdge+1<GridNum-1 && AnalyLine[RightEdge+2]==AnalyLine[RightEdge])
						if(AnalyLine[RightEdge+3]==AnalyLine[RightEdge])
						{
							//�ұ߸�������������
							m_LineRecord[RightEdge+3]=ANALSISED;
							m_LineRecord[RightEdge+2]=ANALSISED;
							m_LineRecord[RightEdge]=SFOUR;//����
						}
						else
							if(AnalyLine[RightEdge+3]==NOSTONE)
							{
								//�ұ߸� 1 ����������
								m_LineRecord[RightEdge+2]=ANALSISED;
								m_LineRecord[RightEdge]=STHREE;//����
							}
							else
							{
								if(m_LineRecord[LeftEdge]==SFOUR)//��߳���
									return m_LineRecord[LeftEdge];//����

								if(m_LineRecord[LeftEdge]==STHREE)//�������        
									return m_LineRecord[LeftEdge];

								if(Lefttwo==true)
									m_LineRecord[nAnalyPos]=TWO;//���ػ��
								else
									m_LineRecord[nAnalyPos]=STWO;//�߶�
							}
					else
					{
						if(m_LineRecord[LeftEdge]==SFOUR)//���ķ���
							return m_LineRecord[LeftEdge];

						if(Lefttwo==true)//�߶�
							m_LineRecord[nAnalyPos]=STWO;
					}

					return m_LineRecord[nAnalyPos];
	}

	return 0;
}

//����ʷ��¼����������Ŀȫ��Ϊ��ֵ
void ResetHistoryTable()
{
	memset(m_HistoryTable,10,GRID_COUNT*sizeof(int));
}

//����ʷ�÷ֱ���ȡ�����߷�����ʷ�÷�
int GetHistoryScore(STONEMOVE* move)
{
	return m_HistoryTable[move->StonePos.x][move->StonePos.y];
}

//��һ����߷�������ʷ��¼
void EnterHistoryScore(STONEMOVE* move,int depth)
{
	m_HistoryTable[move->StonePos.x][move->StonePos.y]+=2<<depth;
}

//���߷����д�С��������
//STONEMOVE* sourceԭʼ����
//STONEMOVE* targetĿ�����
//�ϲ�source[l��m]�� source[m +1��r]��target[l��r]
void Merge(STONEMOVE* source,STONEMOVE* target,int l,int m,int r)
{
	//��С��������
	int i=l;
	int j=m+1;
	int k=l;
	while(i<=m && j<=r)
		if(source[i].Score<=source[j].Score)
			target[k++]=source[i++];
		else
			target[k++]=source[j++];
	if(i>m)
		for(int q=j;q<=r;q++)
			target[k++]=source[q];
	else
		for(int q=i;q<=m;q++)
			target[k++]=source[q];
}

void Merge_A(STONEMOVE* source,STONEMOVE* target,int l,int m,int r)
{
	//�Ӵ�С����
	int i=l;
	int j=m+1;
	int k=l;
	while(i<=m &&j<=r)
		if(source[i].Score>=source[j].Score)
			target[k++]=source[i++];
		else
			target[k++]=source[j++];
	if(i>m)
		for(int q=j;q<=r;q++)
			target[k++]=source[q];
	else
		for(int q=i;q<=m;q++)
			target[k++]=source[q];
}

//�ϲ���СΪ S ������������
//direction �Ǳ�־,ָ���ǴӴ�С���Ǵ�С��������
void MergePass(STONEMOVE* source,STONEMOVE* target,const int s,const int n,const bool direction)
{ 
	int i=0;
	while(i<=n-2*s)
	{
		//�ϲ���СΪ s�����ڶ���������
		if(direction)
			Merge(source,target,i,i+s-1,i+2*s-1);
		else
			Merge_A(source,target,i,i+s-1,i+2*s-1);
		i=i+2*s;
	}
	if(i+s<n)//ʣ���Ԫ�ظ���С��2s
	{
		if(direction)
			Merge(source,target,i,i+s-1,n-1);
		else
			Merge_A(source,target,i,i+s-1,n-1);
	}
	else
		for(int j=i;j<=n-1;j++)
			target[j]=source[j];
}

void MergeSort(STONEMOVE* source,int n,bool direction)
{
	int s=1;
	while(s<n)
	{
		MergePass(source,m_TargetBuff,s,n,direction);
		s+=s;
		MergePass(m_TargetBuff,source,s,n,direction);
		s+=s;
	}
}

int CreatePossibleMove(unsigned char position[][GRID_NUM], int nPly, int nSide)
{
	int i,j;
	m_nMoveCount=0;
	for(i=0;i<GRID_NUM;i++)
		for(j=0;j<GRID_NUM;j++)
		{
			if(position[i][j]==(unsigned char)NOSTONE)
				AddMove(j,i,nPly);
		}

		//ʹ����ʷ�������еľ�̬�鲢���������߷����н�������
		//����Ϊ����߼�֦Ч��
		//        CHistoryHeuristic history;
		MergeSort(m_MoveList[nPly],m_nMoveCount,0);

		return m_nMoveCount;//���غϷ��߷�����
}

//��m_MoveList�в���һ���߷�
//nToX��Ŀ��λ�ú�����
//nToY��Ŀ��λ��������
//nPly�Ǵ��߷����ڵĲ��
int AddMove(int nToX, int nToY, int nPly)
{
	m_MoveList[nPly][m_nMoveCount].StonePos.x=nToX;
	m_MoveList[nPly][m_nMoveCount].StonePos.y=nToY;

	m_nMoveCount++;
	m_MoveList[nPly][m_nMoveCount].Score=PosValue[nToY][nToX];//ʹ��λ�ü�ֵ��������ǰ�߷��ļ�ֵ
	return m_nMoveCount;
}

void CNegaScout_TT_HH()
{
	ResetHistoryTable();
	//m_pThinkProgress=NULL;
}

void SearchAGoodMove(unsigned char position[][GRID_NUM],int Type)
{
	int Score;

	memcpy(CurPosition,position,GRID_COUNT);
	m_nMaxDepth=m_nSearchDepth;
	CalculateInitHashKey(CurPosition);
	ResetHistoryTable();
	Score=NegaScout(m_nMaxDepth,-20000,20000);
	X=m_cmBestMove.StonePos.y;
	Y=m_cmBestMove.StonePos.x;
	MakeMove(&m_cmBestMove,Type);
	memcpy(position,CurPosition,GRID_COUNT);
}

int IsGameOver(unsigned char position[][GRID_NUM],int nDepth)
{
	int score,i;//����Ҫ�µ�������ɫ
	i=(m_nMaxDepth-nDepth)%2;
	score=Eveluate(position,i);//���ù�ֵ����
	if(abs(score)>8000)//�����ֵ�������ؼ�ֵ������������Ϸ����
		return score;//���ؼ�ֵ
	return 0;//����δ����
}

int NegaScout(int depth,int alpha,int beta)
{
	int Count,i;
	unsigned char type;
	int a,b,t;
	int side;
	int score;
	/*        if(depth>0)
	{
	i= IsGameOver(CurPosition,depth);
	if(i!=0)
	return i;  //�ѷ�ʤ�������ؼ�ֵ
	}
	*/
	side=(m_nMaxDepth-depth)%2;//���㵱ǰ�ڵ������,����0/��С1
	score=LookUpHashTable(alpha,beta,depth,side); 
	if(score!=66666) 
		return score;
	if(depth<=0)//Ҷ�ӽڵ�ȡ��ֵ
	{
		score=Eveluate(CurPosition,side);
		EnterHashTable(exact,score,depth,side);//����ֵ�����û���

		return score;
	}
	Count=CreatePossibleMove(CurPosition,depth,side);
	for(i=0;i<Count;i++) 
		m_MoveList[depth][i].Score=GetHistoryScore(&m_MoveList[depth][i]);

	MergeSort(m_MoveList[depth],Count,0);
	int bestmove=-1;
	a=alpha;
	b=beta;

	int eval_is_exact=0;

	for(i=0;i<Count;i++) 
	{
		type=MakeMove(&m_MoveList[depth][i],side);
		Hash_MakeMove(&m_MoveList[depth][i],CurPosition);
		t=-NegaScout(depth-1,-b,-a);//�ݹ������ӽڵ㣬�Ե� 1 ���ڵ���ȫ���ڣ�����ǿմ�̽��
		if(t>a && t<beta && i>0) 
		{
			//���ڵ�һ����Ľڵ�,������������failhigh
			a=-NegaScout(depth-1,-beta,-t);//re-search
			eval_is_exact=1;//����������Ϊ��ȷֵ
			if(depth==m_nMaxDepth)
				m_cmBestMove=m_MoveList[depth][i];
			bestmove=i;
		}
		Hash_UnMakeMove(&m_MoveList[depth][i],CurPosition); 
		UnMakeMove(&m_MoveList[depth][i]); 
		if(a<t)
		{
			eval_is_exact=1;
			a=t;
			if(depth==m_nMaxDepth)
				m_cmBestMove=m_MoveList[depth][i];
		}
		if(a>= beta) 
		{
			EnterHashTable(lower_bound,a,depth,side);
			EnterHistoryScore(&m_MoveList[depth][i],depth);
			return a;
		}
		b=a+1;                      /* set new null window */
	}
	if(bestmove!=-1)
		EnterHistoryScore(&m_MoveList[depth][bestmove], depth);
	if(eval_is_exact) 
		EnterHashTable(exact,a,depth,side);
	else 
		EnterHashTable(upper_bound,a,depth,side);

	return a;
}

unsigned char MakeMove(STONEMOVE* move,int type)
{
	CurPosition[move->StonePos.y][move->StonePos.x]=type;
	return 0;
}

void UnMakeMove(STONEMOVE* move)
{
	CurPosition[move->StonePos.y][move->StonePos.x]=NOSTONE;
}

__int64 rand64(void)
{
	return rand()^((__int64)rand()<<15)^((__int64)rand()<<30)^
		((__int64)rand()<<45)^((__int64)rand()<<60);
}

//����32λ�����
long rand32(void)
{
	return rand()^((long)rand()<<15)^((long)rand()<<30);
}

void CTranspositionTable()
{
	InitializeHashKey();//������ϣ�������������
}

void _CTranspositionTable()
{
	//�ͷŹ�ϣ�����ÿռ�
	delete m_pTT[0];
	delete m_pTT[1];
}

void CalculateInitHashKey(unsigned char CurPosition[][GRID_NUM])
{
	int j,k,nStoneType;

	m_HashKey32=0;
	m_HashKey64=0;
	//���������Ӷ�Ӧ�Ĺ�ϣ������
	for(j=0;j<GRID_NUM;j++)
		for(k=0;k<GRID_NUM;k++)
		{
			nStoneType=CurPosition[j][k];
			if(nStoneType!=0xFF)
			{
				m_HashKey32=m_HashKey32^m_nHashKey32[nStoneType][j][k]; 
				m_HashKey64=m_HashKey64^m_ulHashKey64[nStoneType][j][k]; 
			}
		}
}

void Hash_MakeMove(STONEMOVE *move,unsigned char CurPosition[][GRID_NUM])
{
	int type;

	type=CurPosition[move->StonePos.y][move->StonePos.x];//��������Ŀ��λ�õ����������
	m_HashKey32=m_HashKey32^m_nHashKey32[type][move->StonePos.y][move->StonePos.x];
	m_HashKey64=m_HashKey64^m_ulHashKey64[type][move->StonePos.y][move->StonePos.x];
}

void Hash_UnMakeMove(STONEMOVE *move,unsigned char CurPosition[][GRID_NUM])
{
	int type;
	type=CurPosition[move->StonePos.y][move->StonePos.x];//����������λ���ϵ�������ӹ�ϣֵ����ȥ��
	m_HashKey32=m_HashKey32^m_nHashKey32[type][move->StonePos.y][move->StonePos.x];
	m_HashKey64=m_HashKey64^m_ulHashKey64[type][move->StonePos.y][move->StonePos.x];
}

int LookUpHashTable(int alpha, int beta, int depth, int TableNo)
{
	int x;
	HashItem* pht;

	//�����ʮλ��ϣ��ַ����������趨�Ĺ�ϣ���С���� 1M*2 ��,
	//���� TableSize*2��TableSizeΪ�����趨�Ĵ�С
	//����Ҫ�޸���һ��Ϊm_HashKey32% TableSize
	//��һ����������һ��Ҳһ��
	x=m_HashKey32 & 0xFFFFF;
	pht=&m_pTT[TableNo][x];//ȡ������ı���ָ��

	if(pht->depth>=depth && pht->checksum==m_HashKey64)
	{
		switch(pht->entry_type)//�ж���������
		{
		case exact://ȷ��ֵ
			return pht->eval;

		case lower_bound://�±߽�
			if(pht->eval>=beta)
				return pht->eval;
			else 
				break;

		case upper_bound://�ϱ߽�
			if (pht->eval<=alpha)
				return pht->eval;
			else 
				break;
		}
	}

	return 66666;
}

void EnterHashTable(ENTRY_TYPE entry_type, short eval, short depth, int TableNo)
{
	int x;
	HashItem* pht;

	x=m_HashKey32 & 0xFFFFF;//�����ʮλ��ϣ��ַ
	pht=&m_pTT[TableNo][x]; //ȡ������ı���ָ��

	//������д���ϣ��
	pht->checksum=m_HashKey64; //64λУ����
	pht->entry_type=entry_type;//��������
	pht->eval=eval;          //Ҫ�����ֵ
	pht->depth=depth;          //���
}

void InitializeHashKey()
{
	int i,j,k;
	srand((unsigned)time(NULL));
	//����������
	for(i=0;i<15;i++)
		for(j=0;j<10;j++)
			for(k=0;k<9;k++)
			{
				m_nHashKey32[i][j][k]=rand32();
				m_ulHashKey64[i][j][k]=rand64();
			}

			//�����û������ÿռ䡣1M "2 ����Ŀ������Ҳ��ָ��������С
			m_pTT[0]=new HashItem[1024*1024];//���ڴ��ȡ����ֵ�Ľڵ�����
			m_pTT[1]=new HashItem[1024*1024];//���ڴ��ȡ��Сֵ�Ľڵ�����
}


//using namespace std;

int main(int argc, char *argv[])
{
	SL=0; 
	int AI_color;
	char command[10]; //���ڱ���������ַ��� 
	for (int i = 0; i < GRID_NUM; i++) 
		for (int j = 0; j < GRID_NUM; j++) 
			m_RenjuBoard[i][j] = NOSTONE; //���̳�ʼ�� 

	int XS=0;
	//printf("��ѡ���������֣�1��ʾAI���� 0��ʾ��������\n");
	//cin >> XS; //�Ƿ�������� 
	AI_color=BLACK;
	m_nUserStoneColor=WHITE;
	while (!SL) 
	{ 
		printf("\n�������������꣺\n");
		int rival_x, rival_y; //���ڱ��������һ�����ӵ� 
		cin >> rival_x >> rival_y; //���������һ�����ӵ�
		cout << "  0 1 2 3 4 5 6 7 8 9 A B C D E" << endl;
		if(XS == 1 && rival_x == -1 && rival_y == -1) //���AIִ�����ǵ�һ������ռ����������λ�� 
		{ 
			m_RenjuBoard[GRID_NUM / 2][GRID_NUM / 2] = AI_color; //����������Ϣ
			//cout << GRID_NUM / 2 << ' ' << GRID_NUM / 2 << endl; //���
			//cout << flush; //ˢ�»�����  
		}
		else 
		{
			m_RenjuBoard[rival_x][rival_y] =m_nUserStoneColor; 
			//����������Ϣ 
			for (int i = 0; i < GRID_NUM; i++) 
			{
				cout<<mark[i]<<" ";
				for (int j = 0; j < GRID_NUM; j++) 
				{
					if(m_RenjuBoard[i][j]==WHITE)
						printf("O ");
					else if(m_RenjuBoard[i][j]==BLACK)
						printf("X ");
					else
						printf("+ ");
				}
				printf("\n");
			}
			m_nSearchDepth=4;//AI�Ѷȵȼ����� 
			//����������        
			do
			{
				CNegaScout_TT_HH();         //����NegaScout_TT_HH��������
				CTranspositionTable();
				SearchAGoodMove(m_RenjuBoard,AI_color);
				m_RenjuBoard[X][Y]=AI_color;
				printf("\nAI��������Ϊ��");
				cout << X << ' ' << Y << endl; //���
				cout << flush; //ˢ�»�����
				_CTranspositionTable();
				break; //����ѭ�� 
			}
			while (!SL); 
			//ѭ��ֱ������õ�һ����λ��
			cout << "  0 1 2 3 4 5 6 7 8 9 A B C D E" << endl;
			for (int i = 0; i < GRID_NUM; i++) 
			{
				cout<<mark[i]<<" ";
				for (int j = 0; j < GRID_NUM; j++) 
				{
					if(m_RenjuBoard[i][j]==WHITE)
						printf("O ");
					else if(m_RenjuBoard[i][j]==BLACK)
						printf("X ");
					else
						printf("+ ");
				}
				printf("\n");
			}
			if(SL==1)printf("AIʤ��\n");
			else if(SL==2)printf("���ʤ��\n");
		}
	}
	system("PAUSE");
	return EXIT_SUCCESS;
}
