#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

#define FILE_NAME "beats.txt"

const int red = 2;
const int green = 3;
const int blue = 4;
const int SW = 5;
const int push = 6;
const int rhythm = 7;
const int CS = 10;

File file;

struct BeatData {
  float interval;
};

void setup() {
  Serial.begin(9600);

  Serial.print("\nSD 카드 초기화 중...");

  if (!SD.begin(CS)) {
    Serial.println("SD 카드 초기화 실패! 다시 한번 확인해주세요!");
    return;
  }
  Serial.println("\nSD 카드 초기화 성공!");

  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(rhythm, OUTPUT);
  pinMode(push, INPUT);
  pinMode(SW, INPUT_PULLUP);
}

void registerBeats() {
  Serial.println("\n등록 모드");

  digitalWrite(green, HIGH);
  digitalWrite(blue, HIGH);

  BeatData result[10];

  for (int i = 0; i < 10; i++) {
    float interval;
    int rV = digitalRead(push);

    float start_ = (millis() / 1000.0);
    
    while (rV == LOW) {
      digitalWrite(rhythm, LOW);
      rV = digitalRead(push);
    }
    digitalWrite(rhythm, HIGH);

    float end_ = (millis() / 1000.0);
    interval = i > 0 ? (end_ - start_) : 0;
    Serial.println(interval);
    result[i].interval = interval;
    delay(300);
  }

  Serial.println("데이터 파일 삭제 중...");
  SD.remove(FILE_NAME);
  if (SD.exists(FILE_NAME)) {
    Serial.println("파일 삭제에 실패했어요. 다시 확인해주세요.");
    return;
  } else {
    Serial.println("파일 삭제 성공!");
  }

  file = SD.open(FILE_NAME, FILE_WRITE);

  if (!file) {
    Serial.println("파일 열기 실패");
    return;
  }

  for (int i = 0; i < 10; i++) {
    file.println(result[i].interval);
  }

  file.close();
  Serial.println("저장완료");

  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(green, LOW);
  digitalWrite(rhythm, LOW);

  for (int i = 0; i < 5; i++) {
    digitalWrite(blue, HIGH);
    delay(500);
    digitalWrite(blue, LOW);
    delay(500);
  }

  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(green, LOW);
  digitalWrite(rhythm, LOW);
}

void verifyIdentity() {
  Serial.println("\n인증 모드");

  digitalWrite(blue, HIGH);

  BeatData result[10];

  for (int i = 0; i < 10; i++) {
    float interval;
    int rV = digitalRead(push);

    float start_ = (millis() / 1000.00);
    
    while (rV == LOW) {
      digitalWrite(rhythm, LOW);
      rV = digitalRead(push);
    }
    digitalWrite(rhythm, HIGH);

    float end_ = (millis() / 1000.00);
    interval = i > 0 ? (end_ - start_) : 0;
    Serial.println(interval);
    result[i].interval = interval;
    delay(300);
  }


  BeatData storedResult[10];
  int lineNumber = 0;

  file = SD.open(FILE_NAME);

  if (!file) {
    Serial.println("파일 열기 실패");
    return;
  }

  while (file.available()) {
    storedResult[lineNumber].interval = file.parseFloat();
    lineNumber++;
  }
  file.close();

  digitalWrite(blue, LOW);
  digitalWrite(green, LOW);
  digitalWrite(rhythm, LOW);

  bool isVerified = compareBeats(storedResult, result);

  if (isVerified) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(green, HIGH);
      delay(500);
      digitalWrite(green, LOW);
      delay(500);
    }
  } else {
    for (int i = 0; i < 5; i++) {
      digitalWrite(red, HIGH);
      delay(500);
      digitalWrite(red, LOW);
      delay(500);
    }
  }
  Serial.println(isVerified ? "True" : "False");

  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
  digitalWrite(green, LOW);
  digitalWrite(rhythm, LOW);
}

bool compareBeats(BeatData stored[], BeatData input[]) {
  int count = 0;
  for (int i = 0; i < 10; i += 1) {
    if (abs(stored[i].interval - input[i].interval) > 0.3) {
      count++;
    }  
  }
  if (count != 0) {
    return false;
  }
  return true;
}


void loop() {
  if (analogRead(A1) == 0) {
    registerBeats(); // 등록 모드 시작
  } else if (analogRead(A1) >= 1000) {
    verifyIdentity(); // 인증 모드 시작
  } else {
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(red, LOW);
    digitalWrite(rhythm, LOW);
  }
}
