#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
#pragma once
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#pragma pack(push,1)
struct BITMAPFILEHEADER
{
	unsigned short  Type;
	unsigned long   Size;
	unsigned short  Reserved;
	unsigned short  Reserved2;
	unsigned long   OffsetBits;
};
#pragma pack(pop)
struct RGBTRIPLE
{
	BYTE Blue;
	BYTE Green;
	BYTE Red;
};
struct RGBQUAD
{
	BYTE    Blue;
	BYTE    Green;
	BYTE    Red;
	BYTE    Reserved;
};
struct BITMAPINFOHEADER
{
	DWORD   Size;
	DWORD   Width;
	DWORD   Height;
	WORD    Planes;
	WORD    BitCount;
	DWORD   Compression;
	DWORD   SizeImage;
	DWORD   XPelsPerMeter;
	DWORD   YPelsPerMeter;
	DWORD   ColorUsed;
	DWORD   ColorImportant;
};

class cImage
{
private:
	RGBQUAD * Palette;
	RGBQUAD *Rgbquad;
	RGBTRIPLE *RgbTriple;
	BITMAPFILEHEADER BMFileHeader;
	BITMAPINFOHEADER BMInfoHeader;
	BYTE *indexes;
	int numbOfIndexes;
	BYTE pack(RGBTRIPLE*pixels, int mn, int allPixelsCount);
	BYTE get8BitsPixel(RGBTRIPLE r);
public:

	cImage();
	~cImage();
	cImage(char Mode, unsigned short BCount, int Width, int Height);
	cImage(char *fileName);
	cImage operator = (cImage InpImage);
	void cImageSave(char *fileName);
	RGBTRIPLE getPixel(int x, int y);
	void setPixel(int x, int y, RGBTRIPLE h);
	cImage(const cImage &i);
	cImage operator /= (cImage InpImage);
	cImage operator / (short Depth);
	void loadImage(char *fileName);
};



cImage::cImage()
{
	BMFileHeader.Type = 0x4D42;
	BMFileHeader.OffsetBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	BMFileHeader.Size = BMFileHeader.OffsetBits;
	BMFileHeader.Reserved = 0;
	BMFileHeader.Reserved2 = 0;
	BMInfoHeader.Size = 40;
	BMInfoHeader.Width = 0;
	BMInfoHeader.Height = 0;
	BMInfoHeader.Planes = 1;
	BMInfoHeader.BitCount = 24;
	BMInfoHeader.Compression = 0;
	BMInfoHeader.SizeImage = 0;
	BMInfoHeader.ColorUsed = 0;
	BMInfoHeader.ColorImportant = 0;
	BMInfoHeader.XPelsPerMeter = 0;
	BMInfoHeader.YPelsPerMeter = 0;
	RgbTriple = NULL; // new RGBTRIPLE[BMInfoHeader.Width* BMInfoHeader.Height];
	Palette = NULL;//new RGBTRIPLE[0];
	indexes = NULL;// new BYTE[BMInfoHeader.Width* BMInfoHeader.Height];
	numbOfIndexes = 0;

}

cImage::cImage(char *fileName)
{

	FILE *f;
	f = fopen(fileName, "rb");
	std::string file = fileName;
	if (file.find(".bmp") == -1) throw 4;
	if (f == NULL) throw  5;

	fread(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
	fread(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
	if (BMFileHeader.Type != 0x4D42) throw 6;
	int mx = BMInfoHeader.Width;
	int my = BMInfoHeader.Height;
	RgbTriple = new RGBTRIPLE[mx*my];

	if (BMInfoHeader.BitCount == 24)
	{
		int nb = 0;
		if (mx % 4 != 0)
			nb = 4 - (BMInfoHeader.Width*BMInfoHeader.BitCount / 8) % 4;

		for (int i = mx * my - 1; i >= 0; i--) {
			fread(&RgbTriple[i], sizeof(RGBTRIPLE), 1, f);

			if (i%mx == 0)
				for (int j = 0; j < nb; j++)
				{
					BYTE trash;
					fread(&trash, sizeof(BYTE), 1, f);

				}
		}
		if (BMInfoHeader.BitCount == 24) {
			for (int i = mx * my - 1; i >= 0; i--) {
				fread(&RgbTriple[i], sizeof(RGBTRIPLE), 1, f);
				BYTE trash;
				fread(&trash, sizeof(BYTE), 1, f);
			}
		}
		BMInfoHeader.BitCount = 24;
		fclose(f);
	}
	Rgbquad = new RGBQUAD[mx*my];
	if (BMInfoHeader.BitCount == 32)
	{
		int nb = 0;
		if (mx % 4 != 0)
			nb = 4 - (BMInfoHeader.Width*BMInfoHeader.BitCount / 8) % 4;

		for (int i = mx * my - 1; i >= 0; i--) {
			fread(&Rgbquad[i], sizeof(Rgbquad[i]), 1, f);

			if (i%mx == 0)
				for (int j = 0; j < nb; j++)
				{
					BYTE trash;
					fread(&trash, sizeof(BYTE), 1, f);

				}
		}
		if (BMInfoHeader.BitCount == 32) {
			for (int i = mx * my - 1; i >= 0; i--) {
				fread(&Rgbquad[i], sizeof(RGBQUAD), 1, f);
				BYTE trash;
				fread(&trash, sizeof(BYTE), 1, f);
			}
		}
		BMInfoHeader.BitCount = 32;
		fclose(f);
	}
}


cImage::cImage(char Mode, unsigned short BCount, int Width, int Height)
{
	BMFileHeader.Type = 0x4D42;
	if (BCount == 8)
		BMFileHeader.OffsetBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBTRIPLE) * 256;
	if (BCount == 4)
		BMFileHeader.OffsetBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBTRIPLE) * 16;
	if (BCount == 1)
		BMFileHeader.OffsetBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBTRIPLE) * 2;
	if ((BCount == 24) || (BCount == 32))
		BMFileHeader.OffsetBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	BMFileHeader.Size = BCount * Height*Width + BMFileHeader.OffsetBits;
	BMFileHeader.Reserved = 0;
	BMFileHeader.Reserved2 = 0;
	BMInfoHeader.Size = 40;
	BMInfoHeader.Width = Width;
	BMInfoHeader.Height = Height;
	BMInfoHeader.Planes = 1;
	BMInfoHeader.BitCount = BCount;
	BMInfoHeader.Compression = 0;
	BMInfoHeader.SizeImage = BCount * Height*Width;
	if (BCount == 8)
		BMInfoHeader.ColorUsed = 256;
	if (BCount == 4)
		BMInfoHeader.ColorUsed = 16;
	if (BCount == 1)
		BMInfoHeader.ColorUsed = 2;
	if ((BCount == 24) || (BCount == 32))
		BMInfoHeader.ColorUsed = 0;
	BMInfoHeader.ColorImportant = 0;
	if (BCount == 24) {//if 24bits
		RgbTriple = new RGBTRIPLE[BMInfoHeader.Width* BMInfoHeader.Height];
		for (unsigned int i = 0; i < BMInfoHeader.Width* BMInfoHeader.Height; i++)
		{
			RgbTriple[i].Blue = Mode;
			RgbTriple[i].Green = Mode;
			RgbTriple[i].Red = Mode;

		}

	}
	if (BCount == 32) {//if 32bits
		Rgbquad = new RGBQUAD[BMInfoHeader.Width* BMInfoHeader.Height];
		for (unsigned int i = 0; i < BMInfoHeader.Width* BMInfoHeader.Height; i++)
		{
			Rgbquad[i].Blue = Mode;
			Rgbquad[i].Green = Mode;
			Rgbquad[i].Red = Mode;
			Rgbquad[i].Reserved = Mode;

		}

	}

	if (BCount == 8) { // RGBQUAD
		indexes = new BYTE[BMInfoHeader.Width* BMInfoHeader.Height];
		RGBTRIPLE bit;
		bit.Blue = Mode; bit.Green = Mode; bit.Red = Mode;
		for (int i = 0; i < BMInfoHeader.Width* BMInfoHeader.Height; i++)
			indexes[i] = get8BitsPixel(bit);
		Palette = new RGBQUAD[256];
		for (int i = 0; i < 256; i++)
		{
			Palette[i].Blue = i;
			Palette[i].Green = i;
			Palette[i].Red = i;
			Palette[i].Reserved = 0;
		}
	}



	if (BCount == 4) {
		Palette = new RGBQUAD[2];


		Palette[0].Blue = 0;
		Palette[0].Green = 0;
		Palette[0].Red = 0;
		Palette[0].Reserved = 0;

		Palette[1].Blue = 255;
		Palette[1].Green = 255;
		Palette[1].Red = 255;
		Palette[1].Reserved = 0;



		if (BMInfoHeader.Width % 4 == 0)
		{
			numbOfIndexes = (BMInfoHeader.Width* BMInfoHeader.Height) / 2;
			indexes = new BYTE[numbOfIndexes];
			for (int i = 0; i < numbOfIndexes; i++) {
				indexes[i] = 0;

			}

		}
	}



	if (BCount == 1) {
		Palette = new RGBQUAD[2];


		Palette[0].Blue = 0;
		Palette[0].Green = 0;
		Palette[0].Red = 0;
		Palette[0].Reserved = 0;

		Palette[1].Blue = 255;
		Palette[1].Green = 255;
		Palette[1].Red = 255;
		Palette[1].Reserved = 0;



		if (BMInfoHeader.Width % 8 == 0)
		{
			numbOfIndexes = (BMInfoHeader.Width* BMInfoHeader.Height) / 8;
			indexes = new BYTE[numbOfIndexes];
			for (int i = 0; i < numbOfIndexes; i++) {
				indexes[i] = 0;

			}

		}
		else
		{
			numbOfIndexes = (BMInfoHeader.Width / 8 + 1)*BMInfoHeader.Height;
			indexes = new BYTE[numbOfIndexes];
			for (int i = 0; i < numbOfIndexes; i++)
			{
				indexes[i] = 0;


			}
		}

	}

}

cImage::cImage(const cImage &in)
{

	BMFileHeader = in.BMFileHeader;
	BMInfoHeader = in.BMInfoHeader;
	if (BMInfoHeader.ColorUsed == 0)
	{
		RgbTriple = new RGBTRIPLE[BMInfoHeader.Width* BMInfoHeader.Height];
		for (int i = 0; i < BMInfoHeader.Width* BMInfoHeader.Height; i++)
			RgbTriple[i] = in.RgbTriple[i];
	}
	else RgbTriple = new RGBTRIPLE[0];

	Palette = new RGBQUAD[BMInfoHeader.ColorUsed];
	for (int i = 0; i < BMInfoHeader.ColorUsed; i++)
		Palette[i] = in.Palette[i];

	indexes = new BYTE[in.numbOfIndexes];
	numbOfIndexes = in.numbOfIndexes;
	for (int i = 0; i < numbOfIndexes; i++)
		indexes[i] = in.indexes[i];

	//    Rgbquad = new RGBQUAD[BMInfoHeader.ColorUsed];
	//    for (int i = 0; i < BMInfoHeader.ColorUsed; i++)
	//        Rgbquad[i] = in.Rgbquad[i];



}

cImage cImage::operator=(cImage in)
{
	cImage nImg;
	nImg.BMFileHeader = in.BMFileHeader;
	nImg.BMInfoHeader = in.BMInfoHeader;
	if (nImg.BMInfoHeader.ColorUsed == 0)
	{
		nImg.RgbTriple = new RGBTRIPLE[nImg.BMInfoHeader.Width* nImg.BMInfoHeader.Height];
		for (int i = 0; i < nImg.BMInfoHeader.Width* nImg.BMInfoHeader.Height; i++)
			nImg.RgbTriple[i] = in.RgbTriple[i];
	}
	else nImg.RgbTriple = new RGBTRIPLE[0];


	nImg.Palette = new RGBQUAD[nImg.BMInfoHeader.ColorUsed];
	for (int i = 0; i < nImg.BMInfoHeader.ColorUsed; i++)
		nImg.Palette[i] = in.Palette[i];

	nImg.indexes = new BYTE[in.numbOfIndexes];
	nImg.numbOfIndexes = in.numbOfIndexes;
	for (int i = 0; i < nImg.numbOfIndexes; i++)
		nImg.indexes[i] = in.indexes[i];

	return nImg;
}

void cImage::cImageSave(char *fileName)
{

	int mx = BMInfoHeader.Width;
	int my = BMInfoHeader.Height;
	BYTE z = 0;
	FILE *f;
	f = fopen(fileName, "wb");
	fwrite(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
	fwrite(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);


	if (this->BMInfoHeader.BitCount == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			fwrite(&Palette[i], sizeof(RGBQUAD), 1, f);
		}

		unsigned int nb = 0x0;
		if (BMInfoHeader.Width % 4)
			nb = 4 - BMInfoHeader.Width % 4;

		for (int i = mx * my - 1; i >= 0; i--)
		{
			fwrite(&indexes[i], sizeof(BYTE), 1, f);
			if (i%mx == 0)
				for (int i = 0; i < nb; i++) {
					fwrite(&z, sizeof(BYTE), 1, f);
				}
		}

	}


	if (this->BMInfoHeader.BitCount == 4)
	{
		for (int i = 0; i < 16; i++)
		{
			fwrite(&Palette[i], sizeof(RGBQUAD), 1, f);
		}
		unsigned int nb = 0x0;
		if (mx % 2)
			nb = 3 - (mx / 8) % 2;
		int stroka;
		if (mx % 2 != 0) stroka = mx / 2 + 1; else stroka = mx / 2;



		for (int i = numbOfIndexes - 1; i >= 0; i--)
		{

			fwrite(&indexes[i], sizeof(BYTE), 1, f);

			if (i % stroka == 0)
				for (int i = 0; i < nb; i++) {
					fwrite(&z, sizeof(BYTE), 1, f);
				}
		}
	}

	if (this->BMInfoHeader.BitCount == 1)
	{
		for (int i = 0; i < 2; i++)
		{
			fwrite(&Palette[i], sizeof(RGBQUAD), 1, f);
		}
		unsigned int nb = 0x0;
		if (mx % 8)
			nb = 3 - (mx / 8) % 4;
		int stroka;
		if (mx % 8 != 0) stroka = mx / 8 + 1; else stroka = mx / 8;



		for (int i = numbOfIndexes - 1; i >= 0; i--)
		{

			fwrite(&indexes[i], sizeof(BYTE), 1, f);

			if (i % stroka == 0)
				for (int i = 0; i < nb; i++) {
					fwrite(&z, sizeof(BYTE), 1, f);
				}
		}
	}
	if (this->BMInfoHeader.BitCount == 24) {
		int nb = 0;
		if (mx % 4 != 0)
			nb = 4 - (BMInfoHeader.Width*BMInfoHeader.BitCount / 8) % 4;
		for (int i = mx * my - 1; i >= 0; i--) {
			fwrite(&RgbTriple[i], sizeof(RGBTRIPLE), 1, f);
			if (i%mx == 0)
				for (int j = 0; j < nb; j++) {
					BYTE trash = 0;
					fwrite(&trash, sizeof(BYTE), 1, f);

				}
		}
	}
	if (this->BMInfoHeader.BitCount == 32) {
		int nb = 0;
		if (mx % 4 != 0)
			nb = 4 - (BMInfoHeader.Width*BMInfoHeader.BitCount / 8) % 4;
		for (int i = mx * my - 1; i >= 0; i--) {
			fwrite(&Rgbquad[i], sizeof(RGBQUAD), 1, f);
			if (i%mx == 0)
				for (int j = 0; j < nb; j++) {
					BYTE trash = 0;
					fwrite(&trash, sizeof(BYTE), 1, f);

				}
		}
	}

	fclose(f);

}

RGBTRIPLE cImage::getPixel(int x, int y)
{

	int mx = BMInfoHeader.Width;
	int my = BMInfoHeader.Height;
	if (x > mx || y > my || x < 0 || y < 0)
	{
		printf("Out of bounds");
		return RgbTriple[0];
	}
	x = mx - x - 1;

	RGBTRIPLE pixel = RgbTriple[x + y * mx];
	return pixel;
}

void cImage::setPixel(int x, int y, RGBTRIPLE pix)
{
	int mx = BMInfoHeader.Width;
	int my = BMInfoHeader.Height;
	if (x > mx || y > my || x < 0 || y < 0)
	{
		printf("Out of bounds");
		return;
	}
	x = mx - x - 1;
	RgbTriple[x + y * mx].Blue = pix.Blue;
	RgbTriple[x + y * mx].Green = pix.Green;
	RgbTriple[x + y * mx].Red = pix.Red;

}

cImage cImage::operator /= (cImage InpImage) {

	int oldX = InpImage.BMInfoHeader.Width, oldY = InpImage.BMInfoHeader.Height;
	int newX = this->BMInfoHeader.Width, newY = this->BMInfoHeader.Height;
	cImage sizeble;
	sizeble.BMFileHeader = this->BMFileHeader;
	sizeble.BMInfoHeader = this->BMInfoHeader;
	sizeble.RgbTriple = new RGBTRIPLE[newX*newY];


	for (int y = 0; y < newY; y++)
		for (int x = 0; x < newX; x++)
			sizeble.setPixel(x, y, InpImage.getPixel(x*oldX / newX, y*oldY / newY));

	return sizeble;
}

cImage cImage::operator / (short Depth)
{
	if ((Depth == 24) || (Depth == 32)) return *this;

	int mx = this->BMInfoHeader.Width;
	int my = this->BMInfoHeader.Height;
	cImage gray(0, Depth, mx, my);



	if (Depth == 8)
	{
		{
			gray.Palette = new RGBQUAD[256];
			for (int i = 0; i < 256; i++)
			{
				gray.Palette[i].Blue = i;
				gray.Palette[i].Green = i;
				gray.Palette[i].Red = i;
				gray.Palette[i].Reserved = 0;
			}
		}

		gray.indexes = new BYTE[mx*my];
		gray.numbOfIndexes = mx * my;
		for (int i = 0; i < mx*my; i++)
			gray.indexes[i] = get8BitsPixel(this->RgbTriple[i]);
	}


	if (Depth == 4)
	{
		gray.Palette = new RGBQUAD[2];


		gray.Palette[0].Blue = 0;
		gray.Palette[0].Green = 0;
		gray.Palette[0].Red = 0;
		gray.Palette[0].Reserved = 0;

		gray.Palette[1].Blue = 255;
		gray.Palette[1].Green = 255;
		gray.Palette[1].Red = 255;
		gray.Palette[1].Reserved = 0;
		int mn = 0;
		if (mx % 4 == 0)
		{
			gray.numbOfIndexes = (mx*my) / 2;
			gray.indexes = new BYTE[gray.numbOfIndexes];
			for (int i = 0; i < gray.numbOfIndexes; i++) {
				gray.indexes[i] = pack(this->RgbTriple, mn, 4);
				mn += 2;
			}

		}
	}


	if (Depth == 1)
	{
		gray.Palette = new RGBQUAD[2];


		gray.Palette[0].Blue = 0;
		gray.Palette[0].Green = 0;
		gray.Palette[0].Red = 0;
		gray.Palette[0].Reserved = 0;

		gray.Palette[1].Blue = 255;
		gray.Palette[1].Green = 255;
		gray.Palette[1].Red = 255;
		gray.Palette[1].Reserved = 0;
		int mn = 0;
		if (mx % 8 == 0)
		{
			gray.numbOfIndexes = (mx*my) / 8;
			gray.indexes = new BYTE[gray.numbOfIndexes];
			for (int i = 0; i < gray.numbOfIndexes; i++) {
				gray.indexes[i] = pack(this->RgbTriple, mn, 8);
				mn += 8;
			}

		}
		else
		{
			gray.numbOfIndexes = (mx / 8 + 1)*my;
			gray.indexes = new BYTE[gray.numbOfIndexes];
			int j = 1;
			for (int i = 0; i < gray.numbOfIndexes; i++)
			{
				if (mx*j >= mn + 8) {
					gray.indexes[i] = pack(this->RgbTriple, mn, 8);
					mn += 8; continue;
				}

				gray.indexes[i] = pack(this->RgbTriple, mn, 8);
				mn = mx * j;
				j++;

			}
		}
	}
	return gray;
}

BYTE cImage::pack(RGBTRIPLE*pixels, int mn, int writePixels) // mn - смещение
{

	BYTE r = 0;
	for (int i = 0; i < 8; i++)
	{
		if (writePixels > i) // выход за предел строки
		{
			if (pixels[i + mn].Red*0.299 + pixels[i + mn].Green*0.587 + pixels[i + mn].Blue*0.114 > 128)
			{
				r += 128;
			}
			if (i != (writePixels - 1)) r = r >> 1;
		}
	}
	return r;
}

void cImage::loadImage(char *fileName)
{
	FILE *f;
	f = fopen(fileName, "rb");
	std::string file = fileName;
	if (file.find(".bmp") == -1) throw 4;
	if (f == NULL) throw  5;

	fread(&BMFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
	fread(&BMInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
	if (BMFileHeader.Type != 0x4D42) throw 6;
	int mx = BMInfoHeader.Width;
	int my = BMInfoHeader.Height;
	RgbTriple = new RGBTRIPLE[mx*my];

	if (BMInfoHeader.BitCount == 24) {
		int nb = 0;
		if (mx % 4 != 0)
			nb = 4 - (BMInfoHeader.Width*BMInfoHeader.BitCount / 8) % 4;

		for (int i = mx * my - 1; i >= 0; i--) {
			fread(&RgbTriple[i], sizeof(RGBTRIPLE), 1, f);

			if (i%mx == 0)
				for (int j = 0; j < nb; j++) {
					BYTE trash;
					fread(&trash, sizeof(BYTE), 1, f);

				}
		}
		if (BMInfoHeader.BitCount == 24) {
			for (int i = mx * my - 1; i >= 0; i--) {
				fread(&RgbTriple[i], sizeof(RGBTRIPLE), 1, f);
				BYTE trash;
				fread(&trash, sizeof(BYTE), 1, f);
			}
		}
		BMInfoHeader.BitCount = 24;
		fclose(f);
	}
	Rgbquad = new RGBQUAD[mx*my];
	if (BMInfoHeader.BitCount == 32) {
		int nb = 0;
		if (mx % 4 != 0)
			nb = 4 - (BMInfoHeader.Width*BMInfoHeader.BitCount / 8) % 4;

		for (int i = mx * my - 1; i >= 0; i--) {
			fread(&Rgbquad[i], sizeof(RGBQUAD), 1, f);

			if (i%mx == 0)
				for (int j = 0; j < nb; j++) {
					BYTE trash;
					fread(&trash, sizeof(BYTE), 1, f);

				}
		}
		if (BMInfoHeader.BitCount == 32) {
			for (int i = mx * my - 1; i >= 0; i--) {
				fread(&Rgbquad[i], sizeof(RGBQUAD), 1, f);
				BYTE trash;
				fread(&trash, sizeof(BYTE), 1, f);
			}
		}
		BMInfoHeader.BitCount = 32;
		fclose(f);
	}

}

BYTE cImage::get8BitsPixel(RGBTRIPLE pixel)
{
	return  pixel.Red*0.299 + pixel.Green*0.587 + pixel.Blue*0.114;
}
cImage::~cImage()
{

	delete this->Palette;
	delete this->indexes;

}

int main()
{
	setlocale(LC_ALL, "rus");
	cImage sImage;
	cImage nImage;
	char str[150];
	char str2[150];
	int x = 0, y = 0, h = 0, n;

	cout << "Введите название .bmp" << endl;
	cin >> str;
	try {

		sImage.loadImage(str);
		//sImage.cImageSave("image.bmp");
		nImage.loadImage(str);
		// nImage.cImageSave("image1.bmp");
	}
	catch (int o)
	{
		if (o == 4) cout << "Введен неверный файл - " << str << endl;
		if (o == 5)	cout << "Не удалось найти - " << str << endl;
		if (o == 6)	cout << "Файл поврежден - " << str << endl;
		cout << "Программа будет завершена" << endl;

		return 0;
	}
	cout << "Выберите режим сохранения:" << endl;
	cout << "32 - без масштабирования" << endl;
	cout << "24 - c масштабированием" << endl;

	cin >> n;

	if (n == 24) {


		cout << "Введите ширину в пикселях" << endl;
		while (1) {
			cin >> x;
			if (x < 1 || x>10000)
				cout << "ОШИБКА! допустимые значения 2..10000" << endl;
			else break;
		}
		cout << "Введите высоту в пикселях" << endl;
		while (1) {
			cin >> y;
			if (y < 1 || y>10000)
				cout << "ОШИБКА! допустимые значения 2..10000" << endl;
			else break;
		}
		cout << "Введите палитровую глубину (1,4,8. 24 - для простого масштабирования)" << endl;
		while (1) {
			cin >> h;
			if (h != 1 && h != 8 && h != 4 && h != 24)
				cout << "ОШИБКА! допустимые значения 1,8,4,24" << endl;
			else break;
		}

		cImage nnImage(0, 24, x, y);
		cImage mnImage = (nnImage /= nImage) / h;
		cout << "Сохранить новое изображение как... " << endl;
		cin >> str2;


		mnImage.cImageSave(str2);
	}

	else {


		cout << "Сохранить новое изображение как... " << endl;
		cin >> str2;
		sImage.cImageSave(str2);
