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

struct GRBWAnimationState
{
	RgbwColor StartingColor;
	RgbwColor EndingColor;
};

class Uart_GRBW_Led_Device_Driver : public Device_Driver
{
private:
	static uint8_t pixelCount;
	uint8_t actAnimation;
	static HslColor mainColor;
	uint8_t __brightness;
	bool __auto_brightness;

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
	
	static Vector <GRBWAnimationState*> animationState_list;
public:
	Uart_GRBW_Led_Device_Driver(Module_Driver* module, uint8_t _pixelcount, uint8_t priority = THREAD_PRIORITY_NORMAL);
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
	void Animation_Number(uint8_t _animation_number);
	void Set_Animation();
	void Animation_Prev();
	void Animation_Color(uint8_t R, uint8_t G, uint8_t B);
	void SetBrightness(uint8_t _brightness);
public:
	void Exec_Animation_Off();
	void Exec_Animation_Shine();
	void Exec_Animation_Random();
	void Exec_Animation_Cyclon();
	void Exec_Animation_Fire();
	void Exec_Animation_Next();
	void Exec_Animation_Prev();
	void Exec_Animation_Color(uint8_t R, uint8_t G, uint8_t B);
	void Exec_Set_Brightness(uint8_t _brightness);
};

#endif

