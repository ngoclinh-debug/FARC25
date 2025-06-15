#include <Wire.h>

#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#include <PS2X_lib.h>
#define PS2_DAT 39 // MISO
#define PS2_CMD 36 // MOSI
#define PS2_SEL 32// SS
#define PS2_CLK 25 // SLK
#define pressures false
#define rumble false
PS2X ps2x;
int error;

//Khai báo chân của động cơ
//Dong co ben trai
#define chan1_DC1 8
#define chan2_DC1 9
//Dong co ben phai
#define chan1_DC2 10
#define chan2_DC2 11

//Khai báo các biến được dùng
int LeftJ, RightJ;
#define Y_JOY_CALIB 127
#define saiso 5
int MinSpeed = 1000;
int MaxSpeed = 3500;
int Tocdo = 0;

void setup() {
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50); //50Hz
  Serial.begin(115200);
  Serial.print("Ket noi voi tay cam PS2:");
  for (int i = 0; i < 10; i++) // thử kết nối với tay cầm ps2 trong 10 lần
  {
    delay(1000); // đợi 1 giây
    // cài đặt chân và các chế độ: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    Serial.print(".");
    }

  switch (error) // kiểm tra lỗi nếu sau 10 lần không kết nối được
  {
  case 0:
    Serial.println(" Ket noi tay cam PS2 thanh cong");
    break;
  case 1:
    Serial.println(" LOI: Khong tim thay tay cam, hay kiem tra day ket noi vơi tay cam ");
    break;
  case 2:
    Serial.println(" LOI: khong gui duoc lenh");
    break;
  case 3:
    Serial.println(" LOI: Khong vao duoc Pressures mode ");
    break;

}
  //tắt cả 2 động cơ khi mới khởi động
  pwm.setPWM(chan1_DC1, 0, 0);
  pwm.setPWM(chan2_DC1, 0, 0);
  pwm.setPWM(chan1_DC2, 0, 0);
  pwm.setPWM(chan2_DC2, 0, 0);

}

//Hàm điều khiển động cơ dựa trên 2 chân của động cơ và giá trị của joystick (input)
void Tien(int chan1, int chan2, int input){
  //Tính tốc độ quay của động cơ
  //nếu tiến, input [122,0], xung băm [MinSpeed, Maxspeed]
  Tocdo = map(input, 122, 0, MinSpeed, MaxSpeed);
  pwm.setPWM(chan1, 0, Tocdo);
  pwm.setPWM(chan2, 0, 0);
}
void Lui(int chan1, int chan2, int input){
  //Tính tốc độ quay của động cơ
  //nếu lùi, input [132,255], xung băm [MinSpeed, Maxspeed]
  Tocdo = map(input, 132, 255, MinSpeed, MaxSpeed);
  pwm.setPWM(chan1, 0, 0);
  pwm.setPWM(chan2, 0, Tocdo);
}
void Dung(int chan1, int chan2){
  //Giá trị 2222 là 1 số đẹp, thích set số nà khác cũng được
  pwm.setPWM(chan1, 0, 2222);
  pwm.setPWM(chan2, 0, 2222);
}

void loop() {
  //Đọc lại tất cả giá trị từ tay cầm
  ps2x.read_gamepad(false, false);

  //Gán giá trị 2 bên joystick cho 2 biến
  LeftJ = ps2x.Analog(PSS_LY);
  RightJ = ps2x.Analog(PSS_RY);

  //Tình huống muốn bánh xe tiến
  if(LeftJ < (Y_JOY_CALIB - saiso)){
    //Bánh bên trái tiến lên
    Tien(chan1_DC1, chan2_DC1, LeftJ);
  }else if(LeftJ > (Y_JOY_CALIB + saiso)){
    //Bánh bên trái lui
    Lui(chan1_DC1, chan2_DC1, LeftJ);
  }
  else {
    //Bánh bên trái dừng lại;
    Dung(chan1_DC1, chan2_DC1);
  }

  if(RightJ < (Y_JOY_CALIB - saiso)){
    //Bánh bên phải tiến lên
    Tien(chan1_DC2, chan2_DC2, RightJ);
  }else if(RightJ > (Y_JOY_CALIB + saiso)){
    //Bánh bên phải lui
    Lui(chan1_DC2, chan2_DC2, RightJ);
  }
  else {
    //Bánh bên phải dừng lại;
    Dung(chan1_DC2, chan2_DC2);
  }
  int LeftSpeed = LeftJ>Y_JOY_CALIB ? map(LeftJ, 132, 255, MinSpeed, MaxSpeed) : map(LeftJ, 122, 0, MinSpeed, MaxSpeed);
  int RightSpeed = RightJ>Y_JOY_CALIB ? map(RightJ, 132, 255, MinSpeed, MaxSpeed) : map(RightJ, 122, 0, MinSpeed, MaxSpeed);

  //Khảo sát thay đổi trong tốc độ khi dùng Joystick
  //Mục đích: xem code có lỗi không, các giá trị hằng (sai số, calib) đã phù hợp chưa
  //Serial.println("Van toc trai: %d, Van toc phai: %d", LeftSpeed, RightSpeed);
}
