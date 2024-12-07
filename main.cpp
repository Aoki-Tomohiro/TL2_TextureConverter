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
	kOutputDir,       //出力先のディレクトリ
	NumArgument,
};

int main(int argc, char* argv[])
{
	//コマンドライン引数指定なし
	if (argc < NumArgument)
	{
		//使い方を表示する
		TextureConverter::OutputUsage();
		return 0;
	}

	//COMライブラリの初期化
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	//テクスチャコンバーター
	TextureConverter converter;

	//オプションの数
	int numOptions = argc - NumArgument;
	//オプション配列（ダブルポインタ）
	char** options = argv + NumArgument;
	//テクスチャ変換
	converter.ConvertTextureWICToDDS(argv[kFilePath], argv[kOutputDir], numOptions, options);

	//COMライブラリの終了
	CoUninitialize();
	return 0;
}