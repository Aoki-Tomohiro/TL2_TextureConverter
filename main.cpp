#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "TextureConverter.h"

//コマンドライン引数
enum Argument
{
	kApplicationPath, //アプリケーションのパス
	kFilePath,        //渡されたファイルのパス
	NumArgument,
};

int main(int argc, char* argv[])
{
	//COMライブラリの初期化
	assert(argc >= NumArgument);
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//テクスチャコンバーター
	TextureConverter converter;

	//テクスチャ変換
	converter.ConvertTextureWICToDDS(argv[kFilePath]);

	//COMライブラリの終了
	CoUninitialize();
	system("pause");
	return 0;
}