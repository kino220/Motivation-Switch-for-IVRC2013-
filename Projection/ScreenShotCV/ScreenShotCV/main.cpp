//-----------------------------------------------------
// openCVでスクリーンショットをBitmapTextureに読み込み
// openGLで描写するプログラム
//
//　参考　http://class-exe.jpn.org/2011/01/opencv_screenshot_windows/
//-------------------------------------------


#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "GL/freeglut.h"

#ifdef _DEBUG
//Debugモードの場合
#pragma comment(lib,"opencv_core244d.lib")
#pragma comment(lib,"opencv_imgproc244d.lib")
#pragma comment(lib,"opencv_highgui244d.lib")
#pragma comment(lib,"opencv_video244d.lib")
#pragma comment(lib,"opencv_legacy244d.lib")
#else
//Releaseモードの場合
#pragma comment(lib,"opencv_core244 1.lib")
#pragma comment(lib,"opencv_imgproc244 1.lib")
#pragma comment(lib,"opencv_highgui244 .lib")
#pragma comment(lib,"opencv_video244 .lib")
#endif

#define TIME_INTERVAL 10

//------------- プロトタイプ宣言 --------------------//
void display();
void reshape(int w, int h);
void timer(int value);
void DRAW_SQU();
void idle(void);

//----- テクスチャID -----//
GLuint texture[1];


/* RGB用と反転用とリサイズ用のIplImageの作成 */
HWND desktop;
HBITMAP hBitmap;
IplImage *iplimage;
IplImage *flipimage;
IplImage *resizeimage;
int width;
int height;
HDC hDC,hMemDC;

//---------- 画像を読み込んでコンバート ----------------//

void initTextures()
{

	
	/******* テクスチャ作成 ****************/
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

};

void LoadGLTextures(IplImage *InputImage)
{

	cvCvtColor(InputImage, InputImage, CV_BGR2RGB);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, InputImage->width,InputImage->height, GL_RGB,GL_UNSIGNED_BYTE,InputImage->imageData);


};

//-------------OpenGLの初期設定------------------//



int main (int argc, char * const argv[])
{


	/* デスクトップのサイズ */
	desktop = GetDesktopWindow();
	RECT rect;
	GetWindowRect(desktop, &rect);
	width = rect.right;
	height = rect.bottom;
	
	/* RGB用と反転用とリサイズ用のIplImageの作成 */
	iplimage = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
	flipimage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	resizeimage = cvCreateImage(cvSize(width/2, height/2), IPL_DEPTH_8U, 3);
	
	/* DIBの情報を設定する */
	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	
	/* DIBSection作成 */
	LPDWORD lpPixel;
	hDC = GetDC(desktop);
	hBitmap = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&lpPixel, NULL, 0);
	hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBitmap);

	/* IplImageヘッダにデータをセット */
	iplimage->imageData = (char *)lpPixel;


	initTextures();


	/*openGL 初期化*/
	glutInit(&argc,(char **)argv);
	glutInitDisplayMode(GLUT_RGBA| GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("window name");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0,timer,TIME_INTERVAL);
	//glutIdleFunc(idle);
	glutMainLoop();

	
    return 0;
}


void renew(){

	/* デスクトップから取得 */
		BitBlt(hMemDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);

		/* 上下反転して，リサイズ */
		cvFlip(iplimage, flipimage);
		cvResize(flipimage, resizeimage);

		LoadGLTextures(resizeimage);

		/* ウィンドウへ表示 */
		//cv::imshow("Screenshot", (cv::_InputArray)resizeimage);
		//if(cv::waitKey(30) >= 0) break;

}

void idle(void){


	renew();

	glutPostRedisplay();

	Sleep(10);
};



/********[ここからコールバック]****************************************/
void display()
{
	

	static int r = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	glRotated((double)r, 0.0, 1.0, 0.0);
	DRAW_SQU();
	glPopMatrix();


	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();

	if(++r > 360){
		r= 0;
	}

}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

}


void timer(int value)
{
	renew();

	glutPostRedisplay();
	glutTimerFunc(value,timer,TIME_INTERVAL); //タイマー関数
}

/**********[ここから各種関数]***********************/

void DRAW_SQU()
{
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 1.0);
	glVertex2d(-1,-1);

	glTexCoord2d(0.0, 0.0);
	glVertex2d(-1,1);

	glTexCoord2d(1.0, 0.0);
	glVertex2d(1,1);

	glTexCoord2d(1.0, 1.0);
	glVertex2d(1,-1);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}


class SafetyClose{//不正終了防止
public:
	~SafetyClose(){

		cvReleaseImageHeader(&iplimage);
		cvReleaseImage(&flipimage);
		cvReleaseImage(&resizeimage);
		ReleaseDC(desktop, hDC);
		DeleteDC(hMemDC);
		DeleteObject(hBitmap);
		printf("CLOSE\n");
	}
};
SafetyClose SAFETY_CLOSE;