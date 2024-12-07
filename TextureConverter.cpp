#include "TextureConverter.h"
#include <Windows.h>
#include <filesystem>

void TextureConverter::OutputUsage()
{
	printf("�摜�t�@�C����WIC�`������DDS�`���ɕϊ����܂��B\n");
	printf("\n");
	printf("TextureConverter[�h���C�u:][�p�X][�t�@�C����] [�h���C�u:][�p�X] [-ml level]\n");
	printf("\n");
	printf("[�h���C�u:][�p�X][�t�@�C����]: �ϊ�������WIC�`���̉摜�t�@�C�����w�肵�܂��B\n");
	printf("\n");
	printf("[�h���C�u:][�p�X]: �o�͐�̃f�B���N�g�����w�肵�܂��B\n");
	printf("\n");
	printf("[-ml level]: �~�b�v���x�����w�肵�܂��B�O���g�p����ƂPx�P�܂ł̃t���~�b�v�}�b�v�`�F�[���𐶐����܂��B\n");
	printf("\n");
}

void TextureConverter::ConvertTextureWICToDDS(const std::string& filePath, const std::string& outputDir, int numOptions, char* options[])
{
	//�e�N�X�`���t�@�C����ǂݍ���
	LoadWICTextureFromFile(filePath);

	//�e�N�X�`���ϊ�
	SaveDDSTextureToFile(outputDir, numOptions, options);
}

void TextureConverter::LoadWICTextureFromFile(const std::string& filePath)
{
	//�t�@�C���p�X�����C�h������ɕϊ�����
	std::wstring wfilePath = ConvertMultiByteStringToWideString(filePath);

	//WIC�e�N�X�`���̃��[�h
	HRESULT result = DirectX::LoadFromWICFile(wfilePath.c_str(), DirectX::WIC_FLAGS_NONE, &metaData_, scratchImage_);
	assert(SUCCEEDED(result));

	//�t�H���_�p�X�ƃt�@�C�����𕪗�����
	SeparateFilePath(wfilePath);
}

void TextureConverter::SeparateFilePath(const std::wstring& filePath)
{
	size_t pos1;
	std::wstring exceptExt;

	//��؂蕶��'.'���o�Ă����ԍŌ�̕���������
	pos1 = filePath.rfind('.');
	//�������q�b�g������
	if (pos1 != std::wstring::npos)
	{
		//��؂蕶���̌����t�@�C���g���q�Ƃ��ĕۑ�
		fileExt_ = filePath.substr(pos1 + 1, filePath.size() - pos1 - 1);
		//��؂蕶���̑O�܂ł𔲂��o��
		exceptExt = filePath.substr(0, pos1);
	}
	else
	{
		fileExt_ = L"";
		exceptExt = filePath;
	}

	//��؂蕶��'\\'���o�Ă����ԍŌ�̕���������
	pos1 = exceptExt.rfind('\\');
	if (pos1 != std::wstring::npos)
	{
		//��؂蕶���̑O�܂ł��f�B���N�g���p�X�Ƃ��ĕۑ�
		directoryPath_ = exceptExt.substr(0, pos1 + 1);
		//��؂蕶���̌����t�@�C�����Ƃ��ĕۑ�
		fileName_ = exceptExt.substr(pos1 + 1, exceptExt.size() - pos1 - 1);
		return;
	}

	//��؂蕶��'/'���o�Ă����ԍŌ�̕���������
	pos1 = exceptExt.rfind('/');
	if (pos1 != std::wstring::npos)
	{
		//��؂蕶���̑O�܂ł��f�B���N�g���p�X�Ƃ��ĕۑ�
		directoryPath_ = exceptExt.substr(0, pos1 + 1);
		//��؂蕶���̌����t�@�C�����Ƃ��ĕۑ�
		fileName_ = exceptExt.substr(pos1 + 1, exceptExt.size() - pos1 - 1);
		return;
	}

	//��؂蕶�����Ȃ��̂Ńt�@�C�����݂̂Ƃ��Ĉ���
	directoryPath_ = L"";
	fileName_ = exceptExt;
}

void TextureConverter::SaveDDSTextureToFile(const std::string& outputDir, int numOptions, char* options[])
{
	//�~�b�v���x���̏����l��ݒ�
	size_t mipLevel = 0;

	//�~�b�v���x���w��I�v�V����������
	for (int i = 0; i < numOptions; i++)
	{
		//-ml�I�v�V������������Ȃ���Ύ��̃I�v�V������
		if (std::string(options[i]) != "-ml") continue;

		//-ml�����������ꍇ�͎��̈������~�b�v���x���Ƃ��Đݒ�
		mipLevel = std::stoi(options[i + 1]);

		//�~�b�v���x�����ݒ肳�ꂽ�̂ł���ȏ㌟�����Ȃ�
		break;
	}

	//�~�b�v�}�b�v����
	DirectX::ScratchImage mipChain;
	HRESULT result = DirectX::GenerateMipMaps(scratchImage_.GetImages(), scratchImage_.GetImageCount(), scratchImage_.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, mipLevel, mipChain);
	if (SUCCEEDED(result))
	{
		//�C���[�W�ƃ��^�f�[�^���~�b�v�}�b�v�łŒu��������
		scratchImage_ = std::move(mipChain);
		metaData_ = scratchImage_.GetMetadata();
	}

	//���k�`���ɕϊ�
	DirectX::ScratchImage converted;
	result = DirectX::Compress(scratchImage_.GetImages(), scratchImage_.GetImageCount(), metaData_, DXGI_FORMAT_BC7_UNORM_SRGB, DirectX::TEX_COMPRESS_BC7_QUICK, 1.0f, converted);
	if (SUCCEEDED(result))
	{
		scratchImage_ = std::move(converted);
		metaData_ = scratchImage_.GetMetadata();
	}

	//�ǂݍ��񂾃e�N�X�`����SRGB�Ƃ��Ĉ���
	metaData_.format = DirectX::MakeSRGB(metaData_.format);

	//�o�̓p�X���\�z
	std::filesystem::path outputPath = std::filesystem::path(outputDir) / (fileName_ + L".dds");

	//�f�B���N�g�������݂��Ȃ��ꍇ�͍쐬
	if (!std::filesystem::exists(outputPath.parent_path()))
	{
		std::filesystem::create_directories(outputPath.parent_path());
	}

	//DDS�t�@�C�������o��
	result = DirectX::SaveToDDSFile(scratchImage_.GetImages(), scratchImage_.GetImageCount(), metaData_, DirectX::DDS_FLAGS_NONE, outputPath.wstring().c_str());
	assert(SUCCEEDED(result));
}

std::wstring TextureConverter::ConvertMultiByteStringToWideString(const std::string& mString)
{
	//���C�h������ɕϊ������ۂ̕��������v�Z
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, nullptr, 0);

	//���C�h������
	std::wstring wString;
	wString.resize(filePathBufferSize);

	//���C�h������ɕϊ�
	MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, &wString[0], filePathBufferSize);
	return wString;
}