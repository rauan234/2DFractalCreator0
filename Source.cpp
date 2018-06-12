#include <C:\\Users\\Rauan\\Documents\\Visual Studio 2017\\Libraries\\CImg-2.2.3\\CImg.h>
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <io.h>

using namespace cimg_library;
using namespace std;



#define XSize 1920
#define YSize 800
#define Show 0
#define TreesNumber 20
#define PictureDuration 1000
#define StartDelay 3000

#define MainLimbSourceWidth 30

const float WidthDelta[2] =                       { -0.15, 0.22 };  // borders of possible width change per layer
const float AngleDelta[2] =                       { -7, 7 };        // borders of possible angle change per layer
const float TopChasingCoefficient =               0.003;            // coefficient that shows how much will tree grow vertically
const float BottomChasingCoefficient =            0.004;            // coefficient that shows how much will tree grow horisontally
const float SourceAngleChasingCoefficient =       0.005;            // coefficient that shows how straight tree will be
const float MultiplicationWidthDeltaCoefficient = 0.1;              // coefficient that shows how much will source limb width change after multiplication
const float MultiplicationWidthCoefficient[2] =   { 0.1, 0.6 };     // borders of possible child limb width
const float MinimalMultiplicationWidth =          6;                // if limb is not wide enough, it can`t multicate
const float MultiplicationAngleDeltaCoefficient = 0.15;              // coefficient, that shows how much will source limb deviate after multiplication
const int MediumNodeRange =                       80;               // 1 / limbfrequency
const int NodeSize[2] =                           {1, 3};           // borders of possible child limbs number



float Random(float min, float max) {
	return (float(((rand() % int(max * 1000 - min * 1000 + 1)) + min * 1000)) / 1000);
}
bool Getbool(int rarity) {
	return (rand() % rarity == 0);
}

char* Str(int inp) {
	const short size = 3;
	const char Numbers[10] = { '0', '1', '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' };

	char out[size];

	for (short q = 0; q < size; q++) {
		out[size - 1 - q] = Numbers[(inp / int(pow(10, q))) % 10];
	}

	return out;
}



struct Layer {
	short x0, x1;
	short y;

	Layer* pointer;
};
class LayerChain {
private:
	Layer start;
public:
	bool empty = 1;
	short length = 0;

	LayerChain();
	void append(short x0, short x1, short y);
	short* pop();
};
LayerChain::LayerChain() {
	start.pointer = &start;
}
void LayerChain::append(short x0, short x1, short y) {
	length += 1;

	if (empty) {
		start.x0 = x0;
		start.x1 = x1;
		start.y = y;

		empty = 0;
	}
	else {
		Layer* appendix = new Layer;
		(*appendix).pointer = appendix;
		(*appendix).x0 = x0;
		(*appendix).x1 = x1;
		(*appendix).y = y;

		Layer* last = &start;
		while ((*last).pointer != last) {
			last = (*last).pointer;
		}
		(*last).pointer = appendix;
	}
}
short* LayerChain::pop() {
	short out[2];

	out[0] = start.x0;
	out[1] = start.x1;

	start = *(start.pointer);

	length -= 1;
	return out;
}

class Limb {
private:
	LayerChain layers;
public:
	short x0, y0;
	float width;
	short angle;
	short sourceangle;

	Limb(short x0, short y0, short srcwidth, short srcangle);
	void build(void* raw);
	void draw(void* raw);
};
Limb::Limb(short xcor, short ycor, short srcwidth, short srcangle) {
	width = srcwidth;
	x0 = xcor;
	y0 = ycor;
	angle = srcangle;
	sourceangle = srcangle;
}
void Limb::build(void* raw) {
	float x = x0 - width / 2;
	float y = y0;

	float widthdelta;
	while (width > 0) {
		layers.append(short(x) - width / 2, short(x) + width / 2, short(y));

		width -= Random(WidthDelta[0], WidthDelta[1]);
		angle += Random(AngleDelta[0], AngleDelta[1]);
		angle += TopChasingCoefficient * -angle;
		angle += BottomChasingCoefficient * (90 - abs(angle)) * abs(angle) / angle;
		angle += SourceAngleChasingCoefficient * (sourceangle - angle);
		y += (cos(float(angle) * 3.1415 / 180));
		x += (sin(float(angle) * 3.1415 / 180));

		if (Getbool(MediumNodeRange)) {
			if (width >= MinimalMultiplicationWidth) {
				for (short q = 0; q < Random(NodeSize[0], NodeSize[1]); q++) {
					widthdelta = Random(MultiplicationWidthCoefficient[0], MultiplicationWidthCoefficient[1]);

					Limb newlimb(x, y, width * widthdelta, (90 - angle) * (float((rand() % 2) - 0.5) * 2));
					newlimb.build(raw);
					newlimb.draw(raw);

					width *= (1 - widthdelta * MultiplicationWidthDeltaCoefficient);
					angle += (90 - angle) * widthdelta * MultiplicationAngleDeltaCoefficient;
				}
			}
		}
	}
}
void Limb::draw(void* raw) {
	CImg<unsigned char>* img = (static_cast <CImg<unsigned char>*> (raw));

	int8_t color[3] = {255, 255, 255};
	short* data;
	short x0, x1;
	short y = y0;
	//(*img).draw_point(xcor, ycor, color);
	while (layers.length > 0) {
		data = layers.pop();

		x0 = data[0];
		x1 = data[1];

		for (short xcor = x0; xcor < x1; xcor++) {
			(*img).draw_point(XSize - 1 - xcor, YSize - 1 - y, color);
		}
		y += 1;
	}
}



/*int WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       cmdShow)
{
	CImg<unsigned char> img(XSize, YSize, 1, 3); // create image Size[0] x Size[1]
	img.fill(0); // fill with black

	Limb source(XSize / 2, 0, 100);
	source.build();
	source.draw(&img);

	CImgDisplay display(img, "Image");
	while (!display.is_closed())
	{
		display.wait();
		img.display(display);
	}

	return 0;
}*/
int main(){
	cout << "Press any key to start" << endl;
	cout << endl;
	_getch();

	CImg<unsigned char> img(XSize, YSize, 1, 3); // create image Size[0] x Size[1]
	img.fill(0); // fill with black
	CImgDisplay display;
	if(Show){
		img.display(display);
		Sleep(StartDelay);
	}

	bool first = 1;
	int rnum = time(0);
	for (short treenumber = 0; treenumber < TreesNumber; treenumber++) {
		cout << "Started creating tree " << treenumber << endl;

		img.fill(0); // fill with black

		srand(rnum);
		rnum += 1;

		Limb source(XSize / 2, 0, MainLimbSourceWidth, 0);
		source.build(&img);
		cout << "\tBuilt tree " << treenumber << endl;
		source.draw(&img);
		cout << "\tDrawn tree " << treenumber << endl;

		if (Show) {
			img.display(display);
			/*while (!display.is_closed())
			{
				display.wait();
				img.display(display);
			}*/
			Sleep(PictureDuration);

			cout << "Shown tree " << treenumber << endl;
		}
		else {
			char* number = Str(treenumber);
			char newname[12] = {
				'T',
				'r',
				'e',
				'e',
				number[0],
				number[1],
				number[2],
				'.',
				'b',
				'm',
				'p',
				'\0'
			};
			img.save("Tree.bmp");
			cout << "\tWritten tree " << treenumber << " to'" << newname << "' file" << endl;
			DeleteFile(newname);
			rename("Tree.bmp", newname);
		}

		cout << "Finished creating tree " << treenumber << endl;
		cout << endl;
	}

	_getch();
	return 0;
}



/*    Example:

ImgDisplay display(img, "Image");
while (!display.is_closed())
{
display.wait();
if (display.button() && display.mouse_y() >= 0 && display.mouse_x() >= 0)
{
const int y = display.mouse_y();
const int x = display.mouse_x();

unsigned char randomColor[3];
randomColor[0] = rand() % 256;
randomColor[1] = rand() % 256;
randomColor[2] = rand() % 256;

img.draw_point(x, y, randomColor); // img.draw_point(int xcor, int ycor, int[3] color = {red, green, blue});
}
img.display(display);
}*/