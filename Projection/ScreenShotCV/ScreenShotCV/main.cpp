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
void mouse(int button, int state, int x, int y);


//----- テクスチャID -----//
GLuint texture[1];


///-----ウィンドウ生成用------//
int WinID[2]; //ウィンドウID
int wwidth = 640;
int wheight = 480;


/* RGB用と反転用とリサイズ用のIplImageの作成 */
HWND desktop;
HBITMAP hBitmap;
IplImage *iplimage;
IplImage *flipimage;
IplImage *resizeimage;
int width;
int height;
HDC hDC,hMemDC;


//--------矩形選択用---------//
int downX=0,downY=0,upX=0,upY=0;
int mwidth=100, mheight=100;
double aspect;

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


//ウィンドウ１用コールバック
void GLUT_CALL_FUNC()
{
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutIdleFunc(idle);
		glutMouseFunc(mouse);

}





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

	aspect = (double)width / (double)height; //デスクトップのアスペクト比計算


	/*openGL 初期化*/
	glutInit(&argc,(char **)argv);

	//ウィンドウ１つ目
	glutInitDisplayMode(GLUT_RGBA| GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(wwidth,wheight);
	WinID[0] = glutCreateWindow("全体キャプチャ");
	//glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	GLUT_CALL_FUNC();


	
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
		//LoadGLTextures(flipimage);

		

		// IplImageをcv::Matに変換する
		cv::Mat src_img = cv::cvarrToMat(flipimage);


		// (x,y)=(200,200), (width,height)=(100,100)
		cv::Mat roi_img(src_img, cv::Rect(upX,upY, mwidth, mheight));
		

		//cv::MatからIplImageへ変換
		IplImage outimage = roi_img;

		
		cvShowImage("image",&outimage);

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
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	//glRotated((double)r, 0.0, 1.0, 0.0);
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
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

}


void timer(int value)
{
	renew();

	glutPostRedisplay();
	glutTimerFunc(value,timer,TIME_INTERVAL); //タイマー関数
}

//マウスが押された時の処理
void mouse(int button, int state, int x, int y)
{

  switch (button) {
  case GLUT_LEFT_BUTTON:
    printf("left");
    break;
  case GLUT_MIDDLE_BUTTON:
    printf("middle");
    break;
  case GLUT_RIGHT_BUTTON:
    printf("right");
    break;
  default:
    break;
  }

  printf(" button is ");

  switch (state) {
  case GLUT_UP:
    printf("up");
	upX = x;
	upY = y;
	mwidth = upX - downX;
	mheight = upY - downY;
    break;
  case GLUT_DOWN:
    printf("down");
	downX = x;
	downY = y;
    break;
  default:
    break;
  }

  printf(" at (%d, %d)\n", x, y);
}

/**********[ここから各種関数]***********************/

void DRAW_SQU()
{
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	


	

	//printf("%f\n", (double)width / (double)height);

	glTexCoord2d(0.0, 1.0);
	glVertex2d(-1*aspect,-1);//アスペクト比に合わせて伸縮

	glTexCoord2d(0.0, 0.0);
	glVertex2d(-1*aspect,1);

	glTexCoord2d(1.0, 0.0);
	glVertex2d(1*aspect,1);

	glTexCoord2d(1.0, 1.0);
	glVertex2d(1*aspect,-1);

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