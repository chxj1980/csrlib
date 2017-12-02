#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "csrorder.h"
#include "interview.h"
#include "csrlist.h"
#include "csrplot.h"
#include "csrlog.h"
//#include "plot/koolplot.h"


using namespace std;


int main(int argc, char** argv)
{
	cout << "begin" << endl;
	float arrF[] = { 1.0, 1.4, 1.7, 2.7, 6.4, 5.5 };
	SortBubble(arrF, sizeof(arrF) / sizeof(float), false);
	DisplayData(arrF, sizeof(arrF) / sizeof(float));

	cout << AddNum(3, 120) << endl;

	cout << MaxNumByte(3, 6) << endl;
	cout << MaxNumByte(9, 4) << endl;
	cout << MaxNumByte(-13, 15) << endl;
	(cout << MaxNumByte(-10, -189)) << endl;

	printf("hello world\n");

	float a = 113.3f;
	float b = 115.8f;
	swap(a, b);
	cout << a << "  " << b << endl;

	int i;
	int array[10] = {1,2,3,4,5,6,7,8,9,10};
	ACCESS_BUFORE(array[5], 4, 6);
	for (i = 0; i < 10; ++i)
	{
		printf("%d\n", array[i]);
	}

	addressBin();

	cout << thranslateScale("10", 10, 2).c_str() << endl;

	cout << thranslateScale("0000000011111010", 2, 10).c_str() << endl;

	//list���Դ���
	linuxlist tlist;
	listCreate(&tlist);

	for (int i = 0; i < 10; ++i)
		listHeadInsert(&tlist, i);

	for (int i = 35; i > 20; --i)
		listHeadInsert(&tlist, i);

	listDisplay(&tlist);

	listReverse(&tlist);

	listDisplay(&tlist);

	listBubbleSort(&tlist);

	listDisplay(&tlist);

	int arrNum[] = { 1, 4, 6, 8, 3, 100, 32, 45, 2, 7, 6, 2000, 5000, 30, 90, 80, 10, 20, 30, 40, 50, 60, 70, 80, 100, 200, 300, 400, 500, -5, -3};
	QuickSort(arrNum, sizeof(arrNum) / sizeof(int), 0, sizeof(arrNum) / sizeof(int), true);
	DisplayData(arrNum, sizeof(arrNum) / sizeof(int));

	//��������
	//srand((unsigned int)time(0));
	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);//ʹ��˫����ģʽ����Ȼ���
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("2D Bezier����");
	//initGLLib();
	glutDisplayFunc(drawLine);
	glutReshapeFunc(reshape);
	//glutKeyboardFunc(keyboard);
	//glutIdleFunc(drawLine);//���ÿ���ʱ���õĺ���

	InitLogSys();
	LogWrite("hellos");
	EndLogSys();

	glutMainLoop();

	//glutInit(&argc, argv);
	////��ʼ��GLUT��OpenGL���ڵ���ʾģʽ
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	//glutInitWindowSize(500, 500);
	//glutInitWindowPosition(100, 100);
	//glutCreateWindow(argv[0]);
	//glutDisplayFunc(drawLine);
	//glutReshapeFunc(ChangeSize);
	//SetupRC();
	//glutMainLoop();

	system("pause");
	return 0;
}