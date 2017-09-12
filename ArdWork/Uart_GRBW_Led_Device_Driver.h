// Uart_GRBW_Led_Device_Driver.h

#ifndef _Uart_GRBW_Led_Device_Driver_h
#define _Uart_GRBW_Led_Device_Driver_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "m_Vector.h"

#include "Device_Driver.h"
#include "Uart_GRBW_Led_Device_Driver_Consts.h"
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

struct MyAnimationState
{
	RgbwColor StartingColor;
	RgbwColor EndingColor;
};

class Uart_GRBW_Led_Device_Driver : public Device_Driver
{
private:
	static int pixelCount;
	uint8_t actAnimation;
	static RgbColor mainColor;

	static NeoGamma<NeoGammaTableMethod>* colorGamma; // for any fade animations, best to correct gamma
	static NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>* strip;
	static NeoPixelAnimator* animations; // NeoPixel animation management object

	static void CylonAnimUpdate(const AnimationParam& param);
	static void RandomAnimUpdate(const AnimationParam& param);
	static void BlendAnimUpdate(const AnimationParam& param);
	static void FireAnimUpdate(const AnimationParam& param);
	static void ShineAnimUpdate(const AnimationParam& param);

	void SetRandomSeed();
	static uint16_t lastPixel; // track the eye position
	static int8_t moveDir; // track the direction of movement
	
	static AnimEaseFunction moveEase;
	static Vector <MyAnimationState*> animationState_list;
public:
	Uart_GRBW_Led_Device_Driver(Module_Driver* module, int _pixelcount, uint8_t priority = THREAD_PRIORITY_NORMAL);
private:
	void DoAfterInit();
	void DoBeforeShutdown();
	void DoBeforeSuspend();
	void DoDeviceMessage(Int_Thread_Msg message);
	void DoUpdate(uint32_t deltaTime);
	void Build_Descriptor();

protected:
	void Animation_Off();
	void Animation_Shine();
	void Animation_Random();
	void Animation_Cyclon();
	void Animation_Fire();
	void Animation_Next();
	void Animation_Prev();
	void Animation_Color(int R, int G, int B);

public:
	void Exec_Animation_Off();
	void Exec_Animation_Shine();
	void Exec_Animation_Random();
	void Exec_Animation_Cyclon();
	void Exec_Animation_Fire();
	void Exec_Animation_Next();
	void Exec_Animation_Prev();
	void Exec_Animation_Color(int R, int G, int B);
};

#endif

