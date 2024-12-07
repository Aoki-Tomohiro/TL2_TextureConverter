#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "TextureConverter.h"

//�R�}���h���C������
enum Argument
{
	kApplicationPath, //�A�v���P�[�V�����̃p�X
	kFilePath,        //�n���ꂽ�t�@�C���̃p�X
	kOutputDir,       //�o�͐�̃f�B���N�g��
	NumArgument,
};

int main(int argc, char* argv[])
{
	//�R�}���h���C�������w��Ȃ�
	if (argc < NumArgument)
	{
		//�g������\������
		TextureConverter::OutputUsage();
		return 0;
	}

	//COM���C�u�����̏�����
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	//�e�N�X�`���R���o�[�^�[
	TextureConverter converter;

	//�I�v�V�����̐�
	int numOptions = argc - NumArgument;
	//�I�v�V�����z��i�_�u���|�C���^�j
	char** options = argv + NumArgument;
	//�e�N�X�`���ϊ�
	converter.ConvertTextureWICToDDS(argv[kFilePath], argv[kOutputDir], numOptions, options);

	//COM���C�u�����̏I��
	CoUninitialize();
	return 0;
}