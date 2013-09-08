import java.awt.*;

// 取得する画面エリア 
Rectangle screenSize = new Rectangle(0, 0, 2560, 800);
Robot robot;

void setup(){

  size(640,480);
  
  try{
    robot = new Robot();
  }catch(AWTException e){
   e.printStackTrace(); 
  }
  
}

void draw(){
  PImage pimg = new PImage(robot.createScreenCapture(screenSize));
  //image関数がうまくメモリ管理出来てない模様
  //解決策は今のところ不明
  image(pimg, 0, 0);
}



