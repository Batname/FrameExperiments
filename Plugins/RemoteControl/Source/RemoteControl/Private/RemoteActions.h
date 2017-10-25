#pragma once

/** Action - Image Compress */
class FRemoteActionImageCompress : public FNonAbandonableTask
{
public:
	FRemoteActionImageCompress(class FRemoteControl* InContext, int32 InControllerId, uint8* InData, int32 InDataSize, int32 InWidth, int32 InHeight, int32 InQuality);
	~FRemoteActionImageCompress();

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FActionImageCompress, STATGROUP_ThreadPoolAsyncTasks);
	}
private:
	int32 ControllerId;
	uint8* Data;
	int32 DataSize;
	int32 Quality;
	int32 Width;
	int32 Height;
	class FRemoteControl* Context;
};
