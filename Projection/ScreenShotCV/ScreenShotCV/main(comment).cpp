//-----------------------------------------------------
// openCVでスクリーンショットをBitmapTextureに読み込み
// openGLで描写するプログラム
//
//　参考
//  スクリーンショットがらみ　http://class-exe.jpn.org/2011/01/opencv_screenshot_windows/
//　テクスチャ読み込みがらみ　http://wiki.livedoor.jp/mikk_ni3_92/d/OpenCV%A4%C8%A5%C6%A5%AF%A5%B9%A5%C1%A5%E3%BA%EE%C0%AE
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

#define TIME_INTERVAL 10　//最小更新周期

//------------- プロトタイプ宣言 --------------------//
void display();
void reshape(int w, int h);
void timer(int value);
void DRAW_SQU();
void idle(void);

//----- テクスチャID -----//
GLuint texture[1];

//----- ウィンドウハンドル、OpenCV用画像バッファの宣言 -----//
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

	cvCvtColor(InputImage, InputImage, CV_BGR2RGB);//WindowAPIから取得した画像データの色配列を変換※これをしないと赤と青が入れ替わる、WindowsAPIから読み込む時点で正しく設定できれば変換する必要がないのに。。。。
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, InputImage->width,InputImage->height, GL_RGB,GL_UNSIGNED_BYTE,InputImage->imageData);//画像データをあらかじめ作成したテクスチャバッファに流し込みます


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


	initTextures();//テクスチャ作ります


	/*openGL 初期化*/
	glutInit(&argc,(char **)argv);
	glutInitDisplayMode(GLUT_RGBA| GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500,500);//初期のウィンドウサイズ
	glutCreateWindow("window name");//ウィンドウの名前
	glutDisplayFunc(display);//glutPostRedisplay()を呼ばれたとき実行される関数を設定、この場合 void display(void)
	glutReshapeFunc(reshape);//ウィンドウサイズが変更された時に実行される関数を設定
	glutTimerFunc(0,timer,TIME_INTERVAL);//指定時間おきに実行される関数を設定
	//glutIdleFunc(idle);//アイドル時、暇なときにだだひたすら繰り返し実行される関数
	glutMainLoop();//loop開始命令※GLは、glutTimerFuncもしくは、glutIdleFuncで指定した関数がloopするようになっています。

	/*
	基本的にOpenGLのループから抜け出すには、exit(0)を使うしかありません。（もちろんきちんとした方法もあるのですが。。。）
	KeyboardFuncやMouseFuncでキーボード操作、マウス操作時に実行できる関数もありますので所定の入力がされたらexitする感じです。
	また、描写ウィンドウの[×]ボタンでも閉じれます。

		glutMainLoopよりあとはmain関数に帰ってきません。


	終了処理はSafetyCloseクラスのデストラクタにまとめてあるので、プログラムが終了したら自動的に実行されます。安心だね！

	*/
	
    return 0;
}


void renew(){

	/* デスクトップから取得 */
		BitBlt(hMemDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);

		/* 上下反転して，リサイズ */
		cvFlip(iplimage, flipimage);
		cvResize(flipimage, resizeimage);//ここでリサイズしています？※リサイズしないのであればいらないのから、下のLoadTexturesの引数にflipImageを与えるとどうなるだろうか・・・

		LoadGLTextures(resizeimage);

		/* ウィンドウへ表示 */
		//cv::imshow("Screenshot", (cv::_InputArray)resizeimage);
		//if(cv::waitKey(30) >= 0) break;

}

void idle(void){//暇な時くるくるまわる


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
	DRAW_SQU();//テクスチャをポリゴンに貼り付けているのはこの関数の中
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


void timer(int value)//一定時間おきに実行するloop便利だね！
{
	renew();

	glutPostRedisplay();//Displayを実行
	glutTimerFunc(value,timer,TIME_INTERVAL); //タイマー関数
}

/**********[ここから各種関数]***********************/

void DRAW_SQU()//テクスチャをポリゴンにはりつけている。
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