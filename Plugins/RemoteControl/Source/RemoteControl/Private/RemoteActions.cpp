#include "RemoteControlPrivatePCH.h"
#include "RemoteControl.h"
#include "RemoteActions.h"


FRemoteActionImageCompress::FRemoteActionImageCompress(class FRemoteControl* InContext, int32 InControllerId, uint8* InData, int32 InDataSize, int32 InWidth, int32 InHeight, int32 InQuality)
	: Context(InContext)
{
	ControllerId = InControllerId;
	Data = InData;
	DataSize = InDataSize;
	Width = InWidth;
	Height = InHeight;
	Quality = InQuality;
}

FRemoteActionImageCompress::~FRemoteActionImageCompress()
{
	delete[] Data;
}

void FRemoteActionImageCompress::DoWork()
{
	static const FName ImageWrapperModuleName("ImageWrapper");
	IImageWrapperModule& ImageWrapperModule = FModuleManager::GetModuleChecked<IImageWrapperModule>(ImageWrapperModuleName);
	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	//const float StartTime = GetTickCount();

	const bool bResult = ImageWrapper->SetRaw(Data, DataSize, Width, Height, ERGBFormat::RGBA, 8);
	if (bResult)
	{
		TArray<uint8> CompressedData = ImageWrapper->GetCompressed(Quality);

		if (CompressedData.Num() > 0)
		{
			uint32 DatLen = CompressedData.Num();

			uint8 Orientation = Context->GetOrientation(ControllerId);
			FBufferArchive Buffer;
			Buffer << Orientation;
			Buffer.Append(CompressedData.GetData(), CompressedData.Num());

			if (Context)
			{
				Context->SendAction(ControllerId, REMOTE_ACTION_COMPRESSED_IMAGE, Buffer);
			}
		}
	}
}
