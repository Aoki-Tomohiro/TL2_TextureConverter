#include "TextureConverter.h"
#include <Windows.h>
#include <filesystem>

void TextureConverter::OutputUsage()
{
	printf("画像ファイルをWIC形式からDDS形式に変換します。\n");
	printf("\n");
	printf("TextureConverter[ドライブ:][パス][ファイル名] [ドライブ:][パス] [-ml level]\n");
	printf("\n");
	printf("[ドライブ:][パス][ファイル名]: 変換したいWIC形式の画像ファイルを指定します。\n");
	printf("\n");
	printf("[ドライブ:][パス]: 出力先のディレクトリを指定します。\n");
	printf("\n");
	printf("[-ml level]: ミップレベルを指定します。０を使用すると１x１までのフルミップマップチェーンを生成します。\n");
	printf("\n");
}

void TextureConverter::ConvertTextureWICToDDS(const std::string& filePath, const std::string& outputDir, int numOptions, char* options[])
{
	//テクスチャファイルを読み込む
	LoadWICTextureFromFile(filePath);

	//テクスチャ変換
	SaveDDSTextureToFile(outputDir, numOptions, options);
}

void TextureConverter::LoadWICTextureFromFile(const std::string& filePath)
{
	//ファイルパスをワイド文字列に変換する
	std::wstring wfilePath = ConvertMultiByteStringToWideString(filePath);

	//WICテクスチャのロード
	HRESULT result = DirectX::LoadFromWICFile(wfilePath.c_str(), DirectX::WIC_FLAGS_NONE, &metaData_, scratchImage_);
	assert(SUCCEEDED(result));

	//フォルダパスとファイル名を分離する
	SeparateFilePath(wfilePath);
}

void TextureConverter::SeparateFilePath(const std::wstring& filePath)
{
	size_t pos1;
	std::wstring exceptExt;

	//区切り文字'.'が出てくる一番最後の部分を検索
	pos1 = filePath.rfind('.');
	//検索がヒットしたら
	if (pos1 != std::wstring::npos)
	{
		//区切り文字の後ろをファイル拡張子として保存
		fileExt_ = filePath.substr(pos1 + 1, filePath.size() - pos1 - 1);
		//区切り文字の前までを抜き出す
		exceptExt = filePath.substr(0, pos1);
	}
	else
	{
		fileExt_ = L"";
		exceptExt = filePath;
	}

	//区切り文字'\\'が出てくる一番最後の部分を検索
	pos1 = exceptExt.rfind('\\');
	if (pos1 != std::wstring::npos)
	{
		//区切り文字の前までをディレクトリパスとして保存
		directoryPath_ = exceptExt.substr(0, pos1 + 1);
		//区切り文字の後ろをファイル名として保存
		fileName_ = exceptExt.substr(pos1 + 1, exceptExt.size() - pos1 - 1);
		return;
	}

	//区切り文字'/'が出てくる一番最後の部分を検索
	pos1 = exceptExt.rfind('/');
	if (pos1 != std::wstring::npos)
	{
		//区切り文字の前までをディレクトリパスとして保存
		directoryPath_ = exceptExt.substr(0, pos1 + 1);
		//区切り文字の後ろをファイル名として保存
		fileName_ = exceptExt.substr(pos1 + 1, exceptExt.size() - pos1 - 1);
		return;
	}

	//区切り文字がないのでファイル名のみとして扱う
	directoryPath_ = L"";
	fileName_ = exceptExt;
}

void TextureConverter::SaveDDSTextureToFile(const std::string& outputDir, int numOptions, char* options[])
{
	//ミップレベルの初期値を設定
	size_t mipLevel = 0;

	//ミップレベル指定オプションを検索
	for (int i = 0; i < numOptions; i++)
	{
		//-mlオプションが見つからなければ次のオプションへ
		if (std::string(options[i]) != "-ml") continue;

		//-mlが見つかった場合は次の引数をミップレベルとして設定
		mipLevel = std::stoi(options[i + 1]);

		//ミップレベルが設定されたのでこれ以上検索しない
		break;
	}

	//ミップマップ生成
	DirectX::ScratchImage mipChain;
	HRESULT result = DirectX::GenerateMipMaps(scratchImage_.GetImages(), scratchImage_.GetImageCount(), scratchImage_.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, mipLevel, mipChain);
	if (SUCCEEDED(result))
	{
		//イメージとメタデータをミップマップ版で置き換える
		scratchImage_ = std::move(mipChain);
		metaData_ = scratchImage_.GetMetadata();
	}

	//圧縮形式に変換
	DirectX::ScratchImage converted;
	result = DirectX::Compress(scratchImage_.GetImages(), scratchImage_.GetImageCount(), metaData_, DXGI_FORMAT_BC7_UNORM_SRGB, DirectX::TEX_COMPRESS_BC7_QUICK, 1.0f, converted);
	if (SUCCEEDED(result))
	{
		scratchImage_ = std::move(converted);
		metaData_ = scratchImage_.GetMetadata();
	}

	//読み込んだテクスチャをSRGBとして扱う
	metaData_.format = DirectX::MakeSRGB(metaData_.format);

	//出力パスを構築
	std::filesystem::path outputPath = std::filesystem::path(outputDir) / (fileName_ + L".dds");

	//ディレクトリが存在しない場合は作成
	if (!std::filesystem::exists(outputPath.parent_path()))
	{
		std::filesystem::create_directories(outputPath.parent_path());
	}

	//DDSファイル書き出し
	result = DirectX::SaveToDDSFile(scratchImage_.GetImages(), scratchImage_.GetImageCount(), metaData_, DirectX::DDS_FLAGS_NONE, outputPath.wstring().c_str());
	assert(SUCCEEDED(result));
}

std::wstring TextureConverter::ConvertMultiByteStringToWideString(const std::string& mString)
{
	//ワイド文字列に変換した際の文字数を計算
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, nullptr, 0);

	//ワイド文字列
	std::wstring wString;
	wString.resize(filePathBufferSize);

	//ワイド文字列に変換
	MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, &wString[0], filePathBufferSize);
	return wString;
}