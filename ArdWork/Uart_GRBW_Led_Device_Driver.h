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
	unsigned int sv_pattern = 0;
	Color sv_color = { 255, 255, 255 };
	float sv_relBrightness = 100;
	static uint8_t __pixelCount;
	unsigned int actAnimation;
	static HslColor mainColor;
	uint8_t __brightness;

	uint8_t GetLedCount() const { return __pixelCount; };

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
	Uart_GRBW_Led_Device_Driver(Module_Driver* module, uint8_t _pixelCount, uint8_t priority = TASK_PRIORITY_NORMAL);
private:
	void DoDeviceMessage(Int_Task_Msg message);

	void DoUpdate(uint32_t deltaTime);
	void OnBuild_Descriptor() override;
protected:
	void OnInit() override;
protected:
	void Animation_Off();
	void Animation_Shine();
	void Animation_Random();
	void Animation_Cyclon();
	void Animation_Fire();
	void Animation_Next();
	void Animation_Number(unsigned int _animation_number);
	void Set_Animation();
	void Animation_Prev();
	void Animation_Color(uint8_t R, uint8_t G, uint8_t B);
	void SetBrightness(uint8_t _brightness);
	void SetPixel(uint8_t _index, uint8_t R, uint8_t G, uint8_t B);
	void Set_Color_All(int R, int G, int B);
public:
	void Exec_Animation_Off();
	void Exec_Animation_Shine();
	void Exec_Animation_Random();
	void Exec_Animation_Cyclon();
	void Exec_Animation_Fire();
	void Exec_Animation_Next();
	void Exec_Animation_Prev();
	void Exec_Animation_Number(uint8_t _number);
	void Exec_Animation_Color(int R, int G, int B);
	void Exec_Set_Brightness(int _brightness);
	void Exec_Set_Pixel(int _index, int R, int G, int B);
	void Exec_Set_Color_All(int R, int G, int B);

	Property<uint8_t, Uart_GRBW_Led_Device_Driver> led_count{ this, nullptr, &Uart_GRBW_Led_Device_Driver::GetLedCount };

};

#endif


