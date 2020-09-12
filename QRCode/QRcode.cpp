#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <windows.h>
#include <fstream>
#include <io.h>
#include <conio.h>
#include <time.h>
#include <string>
#include <stdlib.h>
using namespace std;
class qrcode
{
private:
	char word[5000];
public:
	void input();//�����ַ���
	char* G_to_U();//תutf8
};
typedef BOOL(WINAPI *PROCSETCONSOLEFONT)(HANDLE, DWORD);
/***************************************************************************
�������ƣ�
��    �ܣ��ı�������ڵ�����
���������
�� �� ֵ��
˵    ����
***************************************************************************/
void setconsolefont(const HANDLE hout, const int font_no)
{
	HMODULE hKernel32 = GetModuleHandleA("kernel32");
	PROCSETCONSOLEFONT setConsoleFont = (PROCSETCONSOLEFONT)GetProcAddress(hKernel32, "SetConsoleFont");
	/* font_no width high
	0       3     5
	1       4     6
	2       5     8
	3       6     8
	4       8     8
	5       16    8
	6       5     12
	7       6     12
	8       7     12
	9       8     12
	10      16    12
	11      8     16
	12      12    16
	13      8     18
	14      10    18
	15      10    20 */
	setConsoleFont(hout, font_no);
	return;
}
void setcolor(HANDLE hout, const int bg_color, const int fg_color)
{
	SetConsoleTextAttribute(hout, bg_color * 16 + fg_color);
}
char* qrcode::G_to_U()//תutf8
{
	int len = MultiByteToWideChar(CP_ACP, 0, word, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, word, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr)
		delete[] wstr;
	return str;
}
void qrcode::input()
{
	gets_s(word);
}
bool whetheris_legal(char p[])//�ж��ַ����Ƿ�Ϸ�
{
	if (strlen(p) <= 100 && strlen(p) > 0)
		return 1;
	else
		return 0;
}
void mode_and_wordnum(char *p, char codenum[])//ģʽָʾ��+�ַ�������
{
	int t;//��¼�ַ���
	int n;//��¼������λ��
	int i = 11;
	strcpy(codenum, "0100");
	for (t = 1; *p; p++, t++);
	t--;
	for (n = 0; t; t = t / 2)
	{
		if (t % 2 == 0)
			codenum[i] = '0';
		else
			codenum[i] = '1';
		i--;
		n++;
	}
	for (i = 4; i < 12 - n; i++)
		codenum[i] = '0';
	codenum[12] = 0;
}
void change_to_binary(char *p, char codenum[])//ģʽָʾ��+�ַ�������+�����ԭ��
{
	int s, i;
	int t;//��¼�ַ���
	int n;//��¼������λ��
	for (t = 1; *p; p++, t++)
	{
		i = 11 + 8 * t;
		if ((int)(*p) > 0)
		{
			for (s = int(*p), n = 0; s; s = s / 2)
			{
				if (s % 2 == 0)
					codenum[i] = '0';
				else
					codenum[i] = '1';
				i--;
				n++;
			}
			for (i = 4 + 8 * t; i < 12 + 8 * t - n; i++)
				codenum[i] = '0';
		}
		else
		{
			for (s = int(*p) + 256; s; s = s / 2)
			{
				if (s % 2 == 0)
					codenum[i] = '0';
				else
					codenum[i] = '1';
				i--;
			}
		}
	}
	t--;
	codenum[12 + 8 * t] = 0;
}
int choose_version(char *p, int version[])//���ݲ�ͬ���ַ���ѡ���Ӧ�汾
{
	int t;//��¼�ֽ���
	for (t = 0; *p; p++, t++);
	if (t * 8 + 16 <= (version[0] - 2) * 8)
		return 1;
	else if (t * 8 + 16 > (version[0] - 2) * 8 && t * 8 + 16 <= (version[1] - 2) * 8)
		return 2;
	else if (t * 8 + 16 > (version[1] - 2) * 8 && t * 8 + 16 <= (version[2] - 2) * 8)
		return 3;
	else if (t * 8 + 16 > (version[2] - 2) * 8 && t * 8 + 16 <= (version[3] - 2) * 8)
		return 4;
	else
		return 5;
}
void supply_bit(char codenum[], int version[], int *ver)//ģʽָʾ��+�ַ�������+�����ԭ��+��λ
{
	char *q;
	int t;//��¼�����Ƶ�λ��
	strcat(codenum, "0000");
	for (q = codenum, t = 0; *q; q++, t++);
	while (t != version[*ver - 1] * 8)
	{
		strcat(codenum, "11101100");
		for (q = codenum, t = 0; *q; q++, t++);
		if (t == version[*ver - 1] * 8)
			break;
		else
			strcat(codenum, "00010001");
		for (q = codenum, t = 0; *q; q++, t++);
	}
}
void group_and_dec(char codenum[], int DEC[])//�ָת��ʮ����
{
	char *q;
	int i;
	int m;//���ʮ����
	for (q = codenum, i = 8, m = 0; *q; q++, i--)
	{
		if (i == 0)
		{
			m++;
			i = 8;
		}
		if (*q == '1')
			DEC[m] = int(DEC[m] + pow(2, i - 1));
	}
}
void add_errnum(char codenum[], int DEC[], int a_n[], int err_a[][100], int *ver)//�����루���������DEC��codenum�У�
{
	int i, j, k;
	int n_;//�������ʽM��x����x����ߴ���
	int n_1 = -1;//���ɶ���ʽG��x����x����ߴ���
	int err_[100] = { 0 };//���ɶ���ʽG��x����ϵ����a��n�η��Ĵη�������x�����ɸߵ��ͣ��ɸ�
	int err_n[100] = { 0 };//���ɶ���ʽG��x����ϵ����a��n�η��Ĵη�������x�����ɸߵ��ͣ�
	int DEC_[1000] = { 0 };//�������ʽM��x����ϵ����x�����ɸߵ��ͣ�
	for (i = 0;; i++)
	{
		if (DEC[i] == 0)
			break;
		DEC_[i] = DEC[i];//�������ʽM��x����ϵ����x�����ɸߵ��ͣ�
	}
	for (i = 99; i >= 0; i--)
	{
		if (err_a[*ver - 1][i] == -1)
			break;
	}
	i--;
	for (j = 0; i >= 0; j++, i--)
	{
		err_n[j] = err_a[*ver - 1][i];//���ɶ���ʽG��x����ϵ����a��n�η��Ĵη�������x�����ɸߵ��ͣ�
		err_[j] = err_n[j];//���ɶ���ʽG��x����ϵ����a��n�η��Ĵη�������x�����ɸߵ��ͣ��ɸ�
		n_1++;
	}
	n_ = n_1 - 1;
	for (i = 0;; i++)
	{
		if (DEC_[i] == 0)
			break;
		n_++;
	}
	/*������*/
	while (n_ >= n_1)
	{
		for (k = 0; k <= 255; k++)
		{
			if (a_n[k] == DEC_[0])
				break;
		}
		if (k <= 255)
		{
			for (j = 0; j <= n_1; j++)
			{
				err_[j] = err_[j] + k;
				if (err_[j] >= 255)
					err_[j] = err_[j] % 255;
			}
			for (j = 0; j <= n_; j++)
			{
				if (j <= n_1)
					DEC_[j] = a_n[err_[j]] ^ DEC_[j];
				else
					DEC_[j] = 0 ^ DEC_[j];
			}
			for (j = 1; j < 1000; j++)
				DEC_[j - 1] = DEC_[j];
			n_--;
		}
		else
		{
			for (j = 1; j < 1000; j++)
				DEC_[j - 1] = DEC_[j];
			n_--;
		}
		for (j = 0; j < 100; j++)
			err_[j] = err_n[j];
	}
	/*���������DEC*/
	for (j = 0;; j++)
	{
		if (DEC[j] == 0)
			break;
	}
	for (i = 0; i < n_1; i++, j++)
	{
		DEC[j] = DEC_[i];
	}
	/*���������codenum*/
	char *q;
	int n = 0;//��¼����������λ��
	int t;
	for (q = codenum; *q; q++, n++);
	n--;
	for (i = 0; i < n_1; i++)
	{
		j = n + 8 * (i + 1);
		for (t = 0; DEC_[i]; DEC_[i] = DEC_[i] / 2)
		{
			if (DEC_[i] % 2 == 0)
				codenum[j] = '0';
			else
				codenum[j] = '1';
			j--;
			t++;
		}
		for (j = n + 1 + 8 * i; j < n + 1 + 8 * (i + 1) - t; j++)
			codenum[j] = '0';
	}
	codenum[n + 1 + 8 * n_1] = 0;
}
void draw_1(int picture[][40], int *ver)//���λ��̽��ͼ�Σ�У��ͼ���Լ�������Ϣ
{
	int i, j;//�У���
			 /*λ��̽��ͼ��*/
	for (i = 1, j = 1; i <= 7, j <= 7; i++, j++)
	{
		picture[1][j] = 1;
		picture[i][1] = 1;
		picture[7][j] = 1;
		picture[i][7] = 1;
	}
	for (i = 3; i <= 5; i++)
	{
		for (j = 3; j <= 5; j++)
			picture[i][j] = 1;
	}//����
	for (i = 1, j = 4 * (*ver) + 11; i <= 7, j <= 4 * (*ver) + 17; i++, j++)
	{
		picture[1][j] = 1;
		picture[i][4 * (*ver) + 11] = 1;
		picture[7][j] = 1;
		picture[i][4 * (*ver) + 17] = 1;
	}
	for (i = 3; i <= 5; i++)
	{
		for (j = j = 4 * (*ver) + 13; j <= 4 * (*ver) + 15; j++)
			picture[i][j] = 1;
	}//����
	for (i = 4 * (*ver) + 11, j = 1; i <= 4 * (*ver) + 17, j <= 7; i++, j++)
	{
		picture[4 * (*ver) + 11][j] = 1;
		picture[i][1] = 1;
		picture[4 * (*ver) + 17][j] = 1;
		picture[i][7] = 1;
	}
	for (i = 4 * (*ver) + 13; i <= 4 * (*ver) + 15; i++)
	{
		for (j = 3; j <= 5; j++)
			picture[i][j] = 1;
	}//����
	 /*У��ͼ�Σ��汾2-5��*/
	if (*ver >= 2 && *ver <= 5)
	{
		for (j = 4 * (*ver) + 17; j > 4 * (*ver) + 11; j--);
		for (i = 1; i < 4 * (*ver) + 11; i++);
		picture[i][j] = 1;
		i = i - 2;
		j = j - 2;
		int m = i + 4;
		int n = j + 4;
		for (; i <= m, j <= n; i++, j++)
		{
			picture[4 * (*ver) + 9][j] = 1;
			picture[i][4 * (*ver) + 9] = 1;
			picture[m][j] = 1;
			picture[i][n] = 1;
		}
	}
	/*��λͼ��*/
	for (i = 7, j = 7; i <= 4 * (*ver) + 11, j <= 4 * (*ver) + 11; i = i + 2, j = j + 2)
	{
		picture[i][7] = 1;
		picture[7][j] = 1;
	}
	picture[4 * (*ver) + 10][9] = 1;
	/*��ʽ��Ϣ��L������ĤģʽΪ0*/
	picture[9][4 * (*ver) + 17] = 0;
	picture[9][4 * (*ver) + 16] = 0;
	picture[9][4 * (*ver) + 15] = 1;
	picture[9][4 * (*ver) + 14] = 0;
	picture[9][4 * (*ver) + 13] = 0;
	picture[9][4 * (*ver) + 12] = 0;
	picture[9][4 * (*ver) + 11] = 1;
	picture[9][4 * (*ver) + 10] = 1;

	picture[4 * (*ver) + 11][9] = 1;
	picture[4 * (*ver) + 12][9] = 1;
	picture[4 * (*ver) + 13][9] = 1;
	picture[4 * (*ver) + 14][9] = 0;
	picture[4 * (*ver) + 15][9] = 1;
	picture[4 * (*ver) + 16][9] = 1;
	picture[4 * (*ver) + 17][9] = 1;

	picture[1][9] = 0;
	picture[2][9] = 0;
	picture[3][9] = 1;
	picture[4][9] = 0;
	picture[5][9] = 0;
	picture[6][9] = 0;
	picture[8][9] = 1;
	picture[9][9] = 1;

	picture[9][8] = 1;
	picture[9][6] = 1;
	picture[9][5] = 1;
	picture[9][4] = 0;
	picture[9][3] = 1;
	picture[9][2] = 1;
	picture[9][1] = 1;
}
void mark(int *ver, int picture_1[][40])//��¼�Ѿ�ռ�õ�ͼ��λ��
{
	int i, j;
	for (i = 1; i <= 8; i++)
	{
		for (j = 1; j <= 8; j++)
			picture_1[i][j] = 2;
	}//����
	for (i = 1; i <= 8; i++)
	{
		for (j = 4 * (*ver) + 10; j <= 4 * (*ver) + 17; j++)
			picture_1[i][j] = 2;
	}//����
	for (i = 4 * (*ver) + 10; i <= 4 * (*ver) + 17; i++)
	{
		for (j = 1; j <= 8; j++)
			picture_1[i][j] = 2;
	}//����
	 /*У��ͼ�Σ��汾2-5��*/
	if (*ver >= 2 && *ver <= 5)
	{
		for (j = 4 * (*ver) + 17; j > 4 * (*ver) + 11; j--);
		for (i = 1; i < 4 * (*ver) + 11; i++);
		picture_1[i][j] = 2;
		i = i - 2;
		j = j - 2;
		int m = i + 4;
		int n = j + 4;
		for (i = m - 4; i <= m; i++)
		{
			for (j = n - 4; j <= n; j++)
				picture_1[i][j] = 2;
		}
	}
	/*��λͼ��*/
	for (i = 7, j = 7; i <= 4 * (*ver) + 11, j <= 4 * (*ver) + 11; i = i++, j = j++)
	{
		picture_1[i][7] = 2;
		picture_1[7][j] = 2;
	}
	picture_1[4 * (*ver) + 10][9] = 2;
	/*��ʽ��Ϣ��L������ĤģʽΪ1*/
	for (j = 4 * (*ver) + 17; j >= 4 * (*ver) + 10; j--)
		picture_1[9][j] = 2;
	for (i = 4 * (*ver) + 11; i <= 4 * (*ver) + 17; i++)
		picture_1[i][9] = 2;
	picture_1[1][9] = 2;
	picture_1[2][9] = 2;
	picture_1[3][9] = 2;
	picture_1[4][9] = 2;
	picture_1[5][9] = 2;
	picture_1[6][9] = 2;
	picture_1[8][9] = 2;
	picture_1[9][9] = 2;

	picture_1[9][8] = 2;
	picture_1[9][6] = 2;
	picture_1[9][5] = 2;
	picture_1[9][4] = 2;
	picture_1[9][3] = 2;
	picture_1[9][2] = 2;
	picture_1[9][1] = 2;
}
void draw_2(int picture[][40], int *ver, char codenum[], int picture_1[][40])//���
{
	int i, j = 4 * (*ver) + 17;
	int t;
	int n;//һ�ֵĴ���
	int m = 0;
	for (n = 0;; n++)
	{
		if (4 * (*ver) + 17 - 2 * n == 7)
			break;
		if (codenum[m] == 0)
			break;
		if (n % 2 == 0)
		{
			for (i = 4 * (*ver) + 17, j = 4 * (*ver) + 17 - 2 * n, t = 0; i >= 1;)
			{
				if (picture_1[i][j] == 0)
				{
					if (t % 2 == 0)
					{
						if (codenum[m] == '1')
							picture[i][j] = 1;
						else
							picture[i][j] = 0;
						m++;
						t++;
						j--;
					}
					else
					{
						if (codenum[m] == '1')
							picture[i][j] = 1;
						else
							picture[i][j] = 0;
						m++;
						t++;
						j++;
						i--;
					}
				}
				else
				{
					if (t % 2 == 0)
					{
						t++;
						j--;
					}
					else
					{
						t++;
						j++;
						i--;
					}
				}
			}
		}
		else
		{
			for (i = 1, j = 4 * (*ver) + 17 - 2 * n, t = 0; i <= 4 * (*ver) + 17;)
			{
				if (picture_1[i][j] == 0)
				{
					if (t % 2 == 0)
					{
						if (codenum[m] == '1')
							picture[i][j] = 1;
						else
							picture[i][j] = 0;
						m++;
						t++;
						j--;
					}
					else
					{
						if (codenum[m] == '1')
							picture[i][j] = 1;
						else
							picture[i][j] = 0;
						m++;
						t++;
						j++;
						i++;
					}
				}
				else
				{
					if (t % 2 == 0)
					{
						t++;
						j--;
					}
					else
					{
						t++;
						j++;
						i++;
					}
				}
			}
		}
	}
	for (n = 1; n <= 3; n++)
	{
		if (codenum[m] == 0)
			break;
		if (n % 2 == 0)
		{
			for (i = 4 * (*ver) + 17, j = 8 - 2 * n, t = 0; i >= 1;)
			{
				if (codenum[m] == 0)
					break;
				if (picture_1[i][j] == 0)
				{
					if (t % 2 == 0)
					{
						if (codenum[m] == '1')
							picture[i][j] = 1;
						else
							picture[i][j] = 0;
						m++;
						t++;
						j--;
					}
					else
					{
						if (codenum[m] == '1')
							picture[i][j] = 1;
						else
							picture[i][j] = 0;
						m++;
						t++;
						j++;
						i--;
					}
				}
				else
				{
					if (t % 2 == 0)
					{
						t++;
						j--;
					}
					else
					{
						t++;
						j++;
						i--;
					}
				}
			}
		}
		else
		{
			for (i = 1, j = 8 - 2 * n, t = 0; i <= 4 * (*ver) + 17;)
			{
				if (codenum[m] == 0)
					break;
				if (picture_1[i][j] == 0)
				{
					if (t % 2 == 0)
					{
						if (codenum[m] == '1')
							picture[i][j] = 1;
						else
							picture[i][j] = 0;
						m++;
						t++;
						j--;
					}
					else
					{
						if (codenum[m] == '1')
							picture[i][j] = 1;
						else
							picture[i][j] = 0;
						m++;
						t++;
						j++;
						i++;
					}
				}
				else
				{
					if (t % 2 == 0)
					{
						t++;
						j--;
					}
					else
					{
						t++;
						j++;
						i++;
					}
				}
			}
		}
	}
}
void draw_3(int picture[][40], int *ver, int picture_1[][40])//��Ĥ
{
	int i, j;
	for (i = 1; i <= 4 * (*ver) + 17; i++)
	{
		for (j = 1; j <= 4 * (*ver) + 17; j++)
		{
			if ((i + j - 2) % 2 == 0 && picture_1[i][j] == 0)
				picture[i][j] = 1 ^ picture[i][j];
		}
	}
}
void draw_final(int picture[][40], int picture_1[][40], int *ver)//��ͼ
{
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	int i, j;
	for (i = 0; i < (4 * (*ver) + 17) + 2; i++)
	{
		for (j = 0; j < (4 * (*ver) + 17) + 2; j++)
		{
			if (picture[i][j] == 1 && (j >= i - 7 || j >= i - (2 * (4 * (*ver) + 17) - 7)))
			{
				setcolor(hout, 0, 7);
				cout << "  ";
			}
			else if (picture[i][j] == 1 && !(j >= i - 7 || j >= i - (2 * (4 * (*ver) + 17) - 7)))
			{
				setcolor(hout, 0, 13);
				cout << "  ";
			}
			else
			{
				setcolor(hout, 15, 0);
				cout << "  ";
			}
		}
		cout << endl;
	}
	setcolor(hout, 0, 7);
}
int main()
{
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	qrcode q;
	char *p, codenum[10000];//תutf8,�洢�����Ʊ���
	int ver;//�汾(1-5)
	int version[5] = { 19,34,55,80,108 };//����ȼ�ΪL
	int DEC[1000] = { 0 };//���תʮ���Ƶ���
	int a_n[256] = { 1,2,4,8,16,32,64,128,29,58,116,232,205,135,19,38,
		76,152,45,90,180,117,234,201,143,3,6,12,24,48,96,192,
		157,39,78,156,37,74,148,53,106,212,181,119,238,193,159,35,
		70,140,5,10,20,40,80,160,93,186,105,210,185,111,222,161,
		95,190,97,194,153,47,94,188,101,202,137,15,30,60,120,240,
		253,231,211,187,107,214,177,127,254,225,223,163,91,182,113,226,
		217,175,67,134,17,34,68,136,13,26,52,104,208,189,103,206,
		129,31,62,124,248,237,199,147,59,118,236,197,151,51,102,204,
		133,23,46,92,184,109,218,169,79,158,33,66,132,21,42,84,
		168,77,154,41,82,164,85,170,73,146,57,114,228,213,183,115,
		230,209,191,99,198,145,63,126,252,229,215,179,123,246,241,255,
		227,219,171,75,150,49,98,196,149,55,110,220,165,87,174,65,
		130,25,50,100,200,141,7,14,28,56,112,224,221,167,83,166,
		81,162,89,178,121,242,249,239,195,155,43,86,172,69,138,9,
		18,36,72,144,61,122,244,245,247,243,251,235,203,139,11,22,
		44,88,176,125,250,233,207,131,27,54,108,216,173,71,142,1 };//a��n�η�ϵ��
	int err_a[5][100] = { { 21,102,238,149,146,229,87,0,-1 },//1-L
	{ 45,32,94,64,70,118,61,46,67,251,0,-1 },//2-L
	{ 105,99,5,124,140,237,58,58,51,37,202,91,61,183,8,0,-1 },//3-L
	{ 190,188,212,212,164,156,239,83,225,221,180,202,187,26,163,61,50,79,60,17,0,-1 },//4-L
	{ 70,218,145,153,227,48,102,13,142,245,21,161,53,165,28,111,201,145,17,118,182,103,2,158,125,173,0,-1 }//5-L
	};//���ɶ���ʽ��a�Ĵ���(x�����ɵ͵������У�
	int picture[40][40] = { 0 };//��ͼ
	int picture_1[40][40] = { 0 };//��¼λ��
	while (1)
	{
		cout << "����Ҫ���ɵ��ַ�������ַ��������100�ֽڣ�����ÿ������Ϊ3���ֽڣ���" << endl;
		q.input();
		if (whetheris_legal(q.G_to_U()) == 1)//�ж��ַ����Ƿ�Ϸ�
		{
			p = q.G_to_U();//תutf8
			mode_and_wordnum(p, codenum);//ģʽָʾ��+�ַ�������
			p = q.G_to_U();//תutf8
			change_to_binary(p, codenum);//ģʽָʾ��+�ַ�������+�����ԭ��
			p = q.G_to_U();//תutf8
			ver = choose_version(p, version);//���ݲ�ͬ���ַ���ѡ���Ӧ�汾��1-5��
			cout << "��Ӧ��ά��汾Ϊ" << (4 * ver + 17) << "*" << (4 * ver + 17) << "������ȼ�ΪL������2�����Ժ����ɡ���" << endl;
			supply_bit(codenum, version, &ver);//ģʽָʾ��+�ַ�������+�����ԭ��+��λ
			group_and_dec(codenum, DEC);//�ָת��ʮ����
			add_errnum(codenum, DEC, a_n, err_a, &ver);//�����루���������DEC��codenum�У�
			draw_1(picture, &ver);//���λ��̽��ͼ�Σ�У��ͼ���Լ�������Ϣ
			mark(&ver, picture_1);//��¼�Ѿ�ռ�õ�ͼ��λ��
			draw_2(picture, &ver, codenum, picture_1);//���
			draw_3(picture, &ver, picture_1);//��Ĥ
			Sleep(2000);
			system("cls");
			setconsolefont(hout, 7);
			system("mode con cols=200 lines=80");
			draw_final(picture, picture_1, &ver);//��ͼ
			break;
		}
		else
			cout << "�����ַ������Ϸ������������룡" << endl;
	}
	system("pause");
	return 0;
}