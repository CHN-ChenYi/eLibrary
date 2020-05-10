#pragma once

// ��͸���ȵ���ɫ����Ϊʮ������
// ��Χ��Ϊ��[0, 65536)
typedef struct Color {
  int R, G, B;
  int Alpha;
} Color;

typedef struct ColorPoint {
  int x, y;
  Color color;
} ColorPoint;


typedef struct Rect {
  int left, right, top, bottom;
} Rect;

typedef struct Button {
  Rect position;       // λ��
  char caption[10];    // ��Ļ
} Button;

typedef struct InputBox {
  Rect position;
  char context[500];
} InputBox;

/* Draw UI components */
void DrawButton(Button* button, int mouse_x, int highlight);
void DrawInputBox(InputBox* input_box, int highlight);

/* Others */
Color ColorConvert(char* color, double alpha);
