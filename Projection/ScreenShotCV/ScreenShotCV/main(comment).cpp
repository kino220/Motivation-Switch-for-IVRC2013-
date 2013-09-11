//-----------------------------------------------------
// openCV�ŃX�N���[���V���b�g��BitmapTexture�ɓǂݍ���
// openGL�ŕ`�ʂ���v���O����
//
//�@�Q�l
//  �X�N���[���V���b�g����݁@http://class-exe.jpn.org/2011/01/opencv_screenshot_windows/
//�@�e�N�X�`���ǂݍ��݂���݁@http://wiki.livedoor.jp/mikk_ni3_92/d/OpenCV%A4%C8%A5%C6%A5%AF%A5%B9%A5%C1%A5%E3%BA%EE%C0%AE
//-------------------------------------------


#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "GL/freeglut.h"

#ifdef _DEBUG
//Debug���[�h�̏ꍇ
#pragma comment(lib,"opencv_core244d.lib")
#pragma comment(lib,"opencv_imgproc244d.lib")
#pragma comment(lib,"opencv_highgui244d.lib")
#pragma comment(lib,"opencv_video244d.lib")
#pragma comment(lib,"opencv_legacy244d.lib")
#else
//Release���[�h�̏ꍇ
#pragma comment(lib,"opencv_core244 1.lib")
#pragma comment(lib,"opencv_imgproc244 1.lib")
#pragma comment(lib,"opencv_highgui244 .lib")
#pragma comment(lib,"opencv_video244 .lib")
#endif

#define TIME_INTERVAL 10�@//�ŏ��X�V����

//------------- �v���g�^�C�v�錾 --------------------//
void display();
void reshape(int w, int h);
void timer(int value);
void DRAW_SQU();
void idle(void);

//----- �e�N�X�`��ID -----//
GLuint texture[1];

//----- �E�B���h�E�n���h���AOpenCV�p�摜�o�b�t�@�̐錾 -----//
HWND desktop;
HBITMAP hBitmap;
IplImage *iplimage;
IplImage *flipimage;
IplImage *resizeimage;
int width;
int height;
HDC hDC,hMemDC;

//---------- �摜��ǂݍ���ŃR���o�[�g ----------------//

void initTextures()
{

	/******* �e�N�X�`���쐬 ****************/
	glGenTextures(1, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

};

void LoadGLTextures(IplImage *InputImage)
{

	cvCvtColor(InputImage, InputImage, CV_BGR2RGB);//WindowAPI����擾�����摜�f�[�^�̐F�z���ϊ�����������Ȃ��ƐԂƐ�����ւ��AWindowsAPI����ǂݍ��ގ��_�Ő������ݒ�ł���Εϊ�����K�v���Ȃ��̂ɁB�B�B�B
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, InputImage->width,InputImage->height, GL_RGB,GL_UNSIGNED_BYTE,InputImage->imageData);//�摜�f�[�^�����炩���ߍ쐬�����e�N�X�`���o�b�t�@�ɗ������݂܂�


};

//-------------OpenGL�̏����ݒ�------------------//



int main (int argc, char * const argv[])
{


	/* �f�X�N�g�b�v�̃T�C�Y */
	desktop = GetDesktopWindow();
	RECT rect;
	GetWindowRect(desktop, &rect);
	width = rect.right;
	height = rect.bottom;
	
	/* RGB�p�Ɣ��]�p�ƃ��T�C�Y�p��IplImage�̍쐬 */
	iplimage = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
	flipimage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	resizeimage = cvCreateImage(cvSize(width/2, height/2), IPL_DEPTH_8U, 3);
	
	/* DIB�̏���ݒ肷�� */
	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	
	/* DIBSection�쐬 */
	LPDWORD lpPixel;
	hDC = GetDC(desktop);
	hBitmap = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&lpPixel, NULL, 0);
	hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBitmap);

	/* IplImage�w�b�_�Ƀf�[�^���Z�b�g */
	iplimage->imageData = (char *)lpPixel;


	initTextures();//�e�N�X�`�����܂�


	/*openGL ������*/
	glutInit(&argc,(char **)argv);
	glutInitDisplayMode(GLUT_RGBA| GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500,500);//�����̃E�B���h�E�T�C�Y
	glutCreateWindow("window name");//�E�B���h�E�̖��O
	glutDisplayFunc(display);//glutPostRedisplay()���Ă΂ꂽ�Ƃ����s�����֐���ݒ�A���̏ꍇ void display(void)
	glutReshapeFunc(reshape);//�E�B���h�E�T�C�Y���ύX���ꂽ���Ɏ��s�����֐���ݒ�
	glutTimerFunc(0,timer,TIME_INTERVAL);//�w�莞�Ԃ����Ɏ��s�����֐���ݒ�
	//glutIdleFunc(idle);//�A�C�h�����A�ɂȂƂ��ɂ����Ђ�����J��Ԃ����s�����֐�
	glutMainLoop();//loop�J�n���߁�GL�́AglutTimerFunc�������́AglutIdleFunc�Ŏw�肵���֐���loop����悤�ɂȂ��Ă��܂��B

	/*
	��{�I��OpenGL�̃��[�v���甲���o���ɂ́Aexit(0)���g����������܂���B�i������񂫂���Ƃ������@������̂ł����B�B�B�j
	KeyboardFunc��MouseFunc�ŃL�[�{�[�h����A�}�E�X���쎞�Ɏ��s�ł���֐�������܂��̂ŏ���̓��͂����ꂽ��exit���銴���ł��B
	�܂��A�`�ʃE�B���h�E��[�~]�{�^���ł�����܂��B

		glutMainLoop��肠�Ƃ�main�֐��ɋA���Ă��܂���B


	�I��������SafetyClose�N���X�̃f�X�g���N�^�ɂ܂Ƃ߂Ă���̂ŁA�v���O�������I�������玩���I�Ɏ��s����܂��B���S���ˁI

	*/
	
    return 0;
}


void renew(){

	/* �f�X�N�g�b�v����擾 */
		BitBlt(hMemDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);

		/* �㉺���]���āC���T�C�Y */
		cvFlip(iplimage, flipimage);
		cvResize(flipimage, resizeimage);//�����Ń��T�C�Y���Ă��܂��H�����T�C�Y���Ȃ��̂ł���΂���Ȃ��̂���A����LoadTextures�̈�����flipImage��^����Ƃǂ��Ȃ邾�낤���E�E�E

		LoadGLTextures(resizeimage);

		/* �E�B���h�E�֕\�� */
		//cv::imshow("Screenshot", (cv::_InputArray)resizeimage);
		//if(cv::waitKey(30) >= 0) break;

}

void idle(void){//�ɂȎ����邭��܂��


	renew();

	glutPostRedisplay();

	Sleep(10);
};



/********[��������R�[���o�b�N]****************************************/
void display()
{
	

	static int r = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	glRotated((double)r, 0.0, 1.0, 0.0);
	DRAW_SQU();//�e�N�X�`�����|���S���ɓ\��t���Ă���̂͂��̊֐��̒�
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


void timer(int value)//��莞�Ԃ����Ɏ��s����loop�֗����ˁI
{
	renew();

	glutPostRedisplay();//Display�����s
	glutTimerFunc(value,timer,TIME_INTERVAL); //�^�C�}�[�֐�
}

/**********[��������e��֐�]***********************/

void DRAW_SQU()//�e�N�X�`�����|���S���ɂ͂���Ă���B
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


class SafetyClose{//�s���I���h�~
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