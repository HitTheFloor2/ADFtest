#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

const double edge = 0.5;		//��Χ�еı߽磬��-0.9~0.9
const double STEP = 0.01;	//�����������С��࣬STEP��edge������
const double min_double_equal_bias = 0.0001;
const double max_dis = 0.01;	//��С�ڴ�ֵ������Ϊ����������
const double Radius = 0.4f;		//��뾶
const string vdata_path = "vdata.txt";
static int line_num = (int)(2 * edge / STEP + 1); //��Χ�б߳���line_num����
int max_deep = 7;	//ADF���������

GLfloat dx = 2, dy =2, dz = 2;
GLfloat speed = 0.05;


class Point {
public:
	double x;
	double y;
	double z;
	double surface_dis;		//����surface_dis����Ϊ�ռ��ϵĵ㵽�����ľ���ֵ����
	double value;
	int out;				//��out=1�����������⣻out=-1,��������
	Point(double x, double y, double z);
	void _glVertex3f();
	void show();
	bool isEdge();

};
Point::Point(double x1, double y1, double z1) {
	x = x1;
	y = y1;
	z = z1;
	value = 0.0;
	out = -1;
}
void Point::show() {
	cout << x << " " << y << " " << z << " " << value << " " << out << endl;
}
void Point::_glVertex3f() {
	glVertex3d(x, y, z);
}
bool Point::isEdge() {
	if (fabs(x) < edge - STEP && fabs(y) < edge - STEP && fabs(z) < edge - STEP) return false;
	return false;
}
double getDistance(Point p1, Point p2) {
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y) +
		(p1.z - p2.z) * (p1.z - p2.z));
}

Point getMiddlePoint(Point p1, Point p2) {
	return Point((p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2);
}

class TreeNode {
public:
	vector<int> no;
	int deep;
	bool need;
	vector<Point> vertexs;
	vector<TreeNode> childNodes;
	//TreeNode�Ĺ��췽���в�����childNodes�ľ�������
	TreeNode(int no1,int deep1, TreeNode parentNode);
	TreeNode();
	void show();
	void showNeed();
	bool isInside(Point p);
	void _drawVoxel();
	
	
};
TreeNode::TreeNode() {
	//����root�ڵ�
	no.push_back(0);
	deep = 1;
	vertexs.push_back(Point(-edge, -edge, -edge));
	vertexs.push_back(Point(-edge, -edge, edge));
	vertexs.push_back(Point(-edge, edge, -edge));
	vertexs.push_back(Point(-edge, edge, edge));
	vertexs.push_back(Point(edge, -edge, -edge));
	vertexs.push_back(Point(edge, -edge, edge));
	vertexs.push_back(Point(edge, edge, -edge));
	vertexs.push_back(Point(edge, edge, edge));
}
TreeNode::TreeNode(int no1,int deep1,TreeNode parentNode) {
	no = parentNode.no;
	no.push_back(no1);
	deep = deep1;
	/*
	000 001 010 011
	100 101 110 111
	*/
	//no��deep��ʶ����Ԫ��λ�ã�no�����ҵ����ڵ���Ԫ�ĵ㣬ͨ���˵�͸��ڵ�����Ķ������е���Եó��ӽڵ�����Ķ���
	for (int i = 0; i < 8; i++) {
		vertexs.push_back(getMiddlePoint(parentNode.vertexs[i],parentNode.vertexs[no.back()]));
	}
}
void TreeNode::show() {
	cout << "------------------" << endl;
	for (int i = 0; i < no.size(); i++) {
		cout << no[i] << ",";
	}
	cout << "  " << endl;
	cout <<	" deep = " << deep <<" need = " << need << endl;

	for (int i = 0; i < 8; i++) {
		cout << "vertex " << i << " : " << ends;
		vertexs[i].show();
	}


}
void TreeNode::showNeed() {
	if (need) {
		show();
	}

}
bool TreeNode::isInside(Point p) {
	//�ж�ĳһ�����Ƿ�����Ԫ�ڲ�
	Point max = vertexs[7];
	Point min = vertexs[0];
	if (p.x > min.x&& p.x < max.x && p.y > min.y&& p.y < max.y && p.z > min.z&& p.z < max.z) {
		return true;
	}
	return false;
	
}
void TreeNode::_drawVoxel() {
	glBegin(GL_LINES);
	vertexs[0]._glVertex3f();
	vertexs[1]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[0]._glVertex3f();
	vertexs[2]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[1]._glVertex3f();
	vertexs[3]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[2]._glVertex3f();
    vertexs[3]._glVertex3f();
	glEnd();
	// 01 02 13 23 15 37 04 26 45 67 57 46
	glBegin(GL_LINES);
	vertexs[1]._glVertex3f();
	vertexs[5]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[3]._glVertex3f();
	vertexs[7]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[0]._glVertex3f();
	vertexs[4]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[2]._glVertex3f();
	vertexs[6]._glVertex3f();
	glEnd();
	// 01 02 13 23 15 37 04 26 45 67 57 46
	glBegin(GL_LINES);
	vertexs[4]._glVertex3f();
	vertexs[5]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[6]._glVertex3f();
	vertexs[7]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[5]._glVertex3f();
	vertexs[7]._glVertex3f();
	glEnd();
	glBegin(GL_LINES);
	vertexs[4]._glVertex3f();
	vertexs[6]._glVertex3f();
	glEnd();


}

void ADF(TreeNode root) {
	

	if (root.deep > max_deep) {
		//cout << "MAX DEEP! ADF STOPPED!" << endl;
		return;
	}
	Point Center = Point(0.0,0.0,0.0);
	//�鿴��ǰTreeNode��voxel���Ƿ��������ϵĵ㣬���У�������һ���˲���
	//voxel�������������棬����������������ཻ��������������Է�Ϊ��������Ͳ���������
	//���������ཻ�Ͱ�������ʱ����Ҫϸ��
	bool need = false;
	bool start = (getDistance(root.vertexs[0], Center) - Radius < 0.0);
	for (int i = 1; i < 8; i++) {
		double d = getDistance(root.vertexs[i], Center);
		if ((d - Radius < 0.0) !=
			start) {
			need = true;
			break;
		}
	}
	if (start == false && need == false) {
		//voxel���������voxel��������
		if (getDistance(getMiddlePoint(root.vertexs[0], root.vertexs[7]), Center) < Radius) {
			//voxel�����������ڲ�����voxel��������
			need = true;
		}
	}

	
	if (need) {
		//��ǰvoxel�������ཻ
		//�ֲ�,����8���ӽڵ�
		//ʵ�������ҵ�ǰ�������ڶ�����е�
		root.need = true;
		root._drawVoxel();
		
		
		for (int i = 0; i < 8; i++) {
			TreeNode tn = TreeNode(i,root.deep+1,root);
			root.childNodes.push_back(tn);

		}
	}

	//root.show();
	//root.showNeed();


	for (int i = 0; i < root.childNodes.size(); i++) {
		ADF(root.childNodes[i]);
	}
	
}


void draw_line_test(void) {
	//��ʼ��
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//ѡһ�����ʵĽǶ�
	gluLookAt(0.1, 0.2, -0.1, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-edge, -edge, -edge);
	glVertex3f(edge, edge, edge);
	glEnd();
	glFlush();

}
void draw_teapot()//����ͼ��
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60.0, (GLfloat)www / (GLfloat)hhh, 1.0, 20.0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	gluLookAt(dx, dy, dz, 0, 0, 0, -1, -1, -1);
	//��ʼ���ӵ���(3,0,0),Ŀ��(0,0,0),�Ϸ���Ϊy��������

	glColor3f(0.0, 1.0, 0.4);
	glutWireTeapot(1);//���Ʋ����ԭ��

	glutSwapBuffers();
}


void draw(void) {
	//����������ĵ㻭��

	//��ʼ��
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//ѡһ�����ʵĽǶ�
	gluLookAt(dx, dy, dz,0,0,0, -1, -1, -1);
	

	TreeNode root = TreeNode();
	ADF(root);
	
	glutSwapBuffers();
	//glFlush();//ˢ�»�����

}
void keyboard(unsigned char key, int x, int y)//������̲���
{
	switch (key)
	{
		//����λ�Ƶļ�
	case 'u': case 'U':
		dx += speed;
		break;
	case 'j': case 'J':
		dx -= speed;
		break;
	case 'i': case 'I':
		dy -= speed;
		break;
	case 'k': case 'K':
		dy += speed;
		break;
	case 'o': case 'O':
		dz += speed;
		break;
	case 'l': case 'L':
		dz -= speed;
		break;

	}
	glutPostRedisplay();
}


int main(int argc, char* argv[]) {
	
	cout << "Please input the Deep:" << endl;
	cin >> max_deep;
	//��ʼ��GLUT library
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
	//�Դ��ڵĴ�С���г�ʼ��
	glutInitWindowSize(1000, 1000);
	// ���ô��ڳ��ֵ�λ��
	//glutInitWindowPosition(int x, int y);
	//��ʼ������չʾģʽ
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow("ADFtest");
	//Ϊ��ǰ�������ú����ص������ڻ�ͼ
	glutDisplayFunc(draw_teapot);
	glutKeyboardFunc(keyboard);//���̸ı��ӵ�
	//���ڸı�ʱ�ĺ���
	//glutReshapeFunc(threeD);
	glutMainLoop();



	return 0;
}