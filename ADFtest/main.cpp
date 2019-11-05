#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

const double edge = 0.5;		//包围盒的边界，即-0.9~0.9
const double STEP = 0.01;	//立方网格的最小间距，STEP是edge的因子
const double min_double_equal_bias = 0.0001;
const double max_dis = 0.01;	//若小于此值，则认为点在球面上
const double Radius = 0.4f;		//球半径
const string vdata_path = "vdata.txt";
static int line_num = (int)(2 * edge / STEP + 1); //包围盒边长有line_num个点
int max_deep = 7;	//ADF最大迭代深度

GLfloat dx = 2, dy =2, dz = 2;
GLfloat speed = 0.05;


class Point {
public:
	double x;
	double y;
	double z;
	double surface_dis;		//这里surface_dis含义为空间上的点到球表面的绝对值距离
	double value;
	int out;				//当out=1，点在球面外；out=-1,点在里面
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
	//TreeNode的构造方法中不生成childNodes的具体内容
	TreeNode(int no1,int deep1, TreeNode parentNode);
	TreeNode();
	void show();
	void showNeed();
	bool isInside(Point p);
	void _drawVoxel();
	
	
};
TreeNode::TreeNode() {
	//生成root节点
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
	//no和deep标识了体元的位置，no可以找到父节点体元的点，通过此点和父节点其余的顶点求中点可以得出子节点其余的顶点
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
	//判断某一个点是否在体元内部
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
	//查看当前TreeNode的voxel中是否有球面上的点，若有，则建立下一级八叉树
	//voxel可以在球面里面，球面外面或与球面相交，在球面外面可以分为包含球面和不包含球面
	//当与球面相交和包括球面时，需要细分
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
		//voxel包裹了球或voxel在球外面
		if (getDistance(getMiddlePoint(root.vertexs[0], root.vertexs[7]), Center) < Radius) {
			//voxel体心在球面内部，是voxel包裹了球
			need = true;
		}
	}

	
	if (need) {
		//当前voxel和球面相交
		//分叉,生成8个子节点
		//实质上是找当前两个相邻顶点的中点
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
	//初始化
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//选一个合适的角度
	gluLookAt(0.1, 0.2, -0.1, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-edge, -edge, -edge);
	glVertex3f(edge, edge, edge);
	glEnd();
	glFlush();

}
void draw_teapot()//绘制图形
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60.0, (GLfloat)www / (GLfloat)hhh, 1.0, 20.0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	gluLookAt(dx, dy, dz, 0, 0, 0, -1, -1, -1);
	//初始化视点在(3,0,0),目标(0,0,0),上方向为y轴正方向

	glColor3f(0.0, 1.0, 0.4);
	glutWireTeapot(1);//绘制茶壶在原点

	glutSwapBuffers();
}


void draw(void) {
	//用球面上面的点画球

	//初始化
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//选一个合适的角度
	gluLookAt(dx, dy, dz,0,0,0, -1, -1, -1);
	

	TreeNode root = TreeNode();
	ADF(root);
	
	glutSwapBuffers();
	//glFlush();//刷新缓冲区

}
void keyboard(unsigned char key, int x, int y)//定义键盘操作
{
	switch (key)
	{
		//控制位移的键
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
	//初始化GLUT library
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
	//对窗口的大小进行初始化
	glutInitWindowSize(1000, 1000);
	// 设置窗口出现的位置
	//glutInitWindowPosition(int x, int y);
	//初始化程序展示模式
	glutInitDisplayMode(GLUT_RGBA);
	glutCreateWindow("ADFtest");
	//为当前窗口设置函数回调，用于画图
	glutDisplayFunc(draw_teapot);
	glutKeyboardFunc(keyboard);//键盘改变视点
	//窗口改变时的函数
	//glutReshapeFunc(threeD);
	glutMainLoop();



	return 0;
}