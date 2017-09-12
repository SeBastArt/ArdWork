// 
// 
// 

#include "Uart_GRBW_Led_Device_Driver.h"
#include "math.h"

int Uart_GRBW_Led_Device_Driver::pixelCount;
uint16_t Uart_GRBW_Led_Device_Driver::lastPixel = 0;
int8_t Uart_GRBW_Led_Device_Driver::moveDir = 1;
Vector <MyAnimationState*> Uart_GRBW_Led_Device_Driver::animationState_list;
RgbColor Uart_GRBW_Led_Device_Driver::mainColor = RgbColor(64, 0, 64);

NeoGamma<NeoGammaTableMethod>* Uart_GRBW_Led_Device_Driver::colorGamma;
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>* Uart_GRBW_Led_Device_Driver::strip;
NeoPixelAnimator* Uart_GRBW_Led_Device_Driver::animations;

Uart_GRBW_Led_Device_Driver::Uart_GRBW_Led_Device_Driver(Module_Driver* module, int _pixelcount, uint8_t priority) :
	Device_Driver(module, priority)
{
	driver_name = "Uart_GRBW_Led_Device_Driver";

	pixelCount = _pixelcount;
	colorGamma = new NeoGamma<NeoGammaTableMethod>; // for any fade animations, best to correct gamma
	strip = new NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>(_pixelcount);
	animations = new NeoPixelAnimator(_pixelcount + ANIMATION_COUNT); // NeoPixel animation management object

	for (int i = 0; i < _pixelcount; i++) {
		MyAnimationState* animationState = new MyAnimationState();
		animationState_list.PushBack(animationState);
	}

	strip->Begin();
};

void Uart_GRBW_Led_Device_Driver::Build_Descriptor() {
	__descriptor->name = "RGB-LED";
	__descriptor->descr = "controls the RGB-LED-Stripe";
}


void Uart_GRBW_Led_Device_Driver::DoAfterInit()
{
	//moveEase = NeoEase::Linear;
	//      NeoEase::QuadraticInOut;
	//      NeoEase::CubicInOut;
	//      NeoEase::QuarticInOut;
	//      NeoEase::QuinticInOut;
	//      NeoEase::SinusoidalInOut;
	//      NeoEase::ExponentialInOut;
	//      NeoEase::CircularInOut;
	SetRandomSeed();
	actAnimation = 1;
	Animation_Off();
	Animation_Shine();
	Serial.println("Uart_Rgb_Led-Driver initialized!");
}

void Uart_GRBW_Led_Device_Driver::DoBeforeShutdown()
{
	//
}

void Uart_GRBW_Led_Device_Driver::DoBeforeSuspend()
{
	//
}

void Uart_GRBW_Led_Device_Driver::DoDeviceMessage(Int_Thread_Msg message)
{
	int messageID = message.GetID();
	switch (messageID)
	{
	case UART_RGB_LED_DEVICE_OFF:
	{
		Animation_Off();
	}
	break;
	case UART_RGB_LED_DEVICE_CYCLON:
	{
		Animation_Cyclon();
	}
	break;
	case UART_RGB_LED_DEVICE_RANDOM:
	{
		Animation_Random();
	}
	break;
	case UART_RGB_LED_DEVICE_FIRE:
	{
		Animation_Fire();
	}
	break;
	case UART_RGB_LED_DEVICE_SHINE:
	{
		Animation_Shine();
	}
	break;
	case UART_RGB_LED_DEVICE_NEXT:
	{
		Animation_Next();
	}
	break;
	case UART_RGB_LED_DEVICE_PREV:
	{
		Animation_Prev();
	}
	break;
	case UART_RGB_LED_DEVICE_COLOR:
	{
		int R = message.GetIntParamByIndex(1);
		int G = message.GetIntParamByIndex(2);
		int B = message.GetIntParamByIndex(3);
		Animation_Color(R, G, B);
	}
	break;
	}
}

void Uart_GRBW_Led_Device_Driver::DoUpdate(uint32_t deltaTime) {
	animations->UpdateAnimations();
	strip->Show();
}

void Uart_GRBW_Led_Device_Driver::DoExecuteCommand(String _command)
{
}


void Uart_GRBW_Led_Device_Driver::SetRandomSeed()
{
	uint32_t seed;

	// random works best with a seed that can use 31 bits
	// analogRead on a unconnected pin tends toward less than four bits
	seed = analogRead(0);
	delay(1);

	for (int shifts = 3; shifts < 31; shifts += 3)
	{
		seed ^= analogRead(0) << shifts;
		delay(1);
	}

	// Serial.println(seed);
	randomSeed(seed);
}


void Uart_GRBW_Led_Device_Driver::CylonAnimUpdate(const AnimationParam& param)
{
	uint16_t nextPixel;

	float progress = NeoEase::Linear(param.progress);
	nextPixel = round(progress * pixelCount) % pixelCount;
	if ((lastPixel != nextPixel))
	{
		/*for (uint16_t i = lastPixel + 1; i != nextPixel; i += 1)
		{
			strip->SetPixelColor(i, mainColor);
			animationState_list[i]->StartingColor = mainColor;
			animationState_list[i]->EndingColor = RgbColor(0, 0, 0);
			if (animations->IsAnimationActive(i)) {
				animations->StopAnimation(i);
			}
			animations->StartAnimation(i, 500, BlendAnimUpdate);
		}*/

		/*strip->SetPixelColor(nextPixel, mainColor);
		animationState_list[nextPixel]->StartingColor = mainColor;
		animationState_list[nextPixel]->EndingColor = RgbColor(0, 0, 0);
		if (animations->IsAnimationActive(nextPixel)) {
			animations->StopAnimation(nextPixel);
		}
		animations->StartAnimation(nextPixel, 500, BlendAnimUpdate);*/

		/*for (uint16_t i = lastPixel + 1; i != nextPixel; i++)
		{
			for (int j = 0; j < 4; j++) {
				int actpixel = (i + (j * (pixelCount / 4))) % pixelCount;
				strip->SetPixelColor(actpixel, mainColor);
				animationState_list[actpixel]->StartingColor = colorGamma->Correct(mainColor);
				animationState_list[actpixel]->EndingColor = RgbColor(0, 0, 0);

				animations->StartAnimation(actpixel, 200, BlendAnimUpdate);
			}
		}*/
		uint8 count = 2;
		for (int j = 0; j < count; j++) {
			int actpixel = (nextPixel + (j * (pixelCount / count))) % pixelCount;

			RgbColor startingColor;
			startingColor.R = fmin(255, mainColor.R + 200);
			startingColor.G = fmin(255, mainColor.G + 200);
			startingColor.B = fmin(255, mainColor.B + 200);

			strip->SetPixelColor(actpixel, colorGamma->Correct(startingColor));
			animationState_list[actpixel]->StartingColor = colorGamma->Correct(startingColor);
			animationState_list[actpixel]->EndingColor = colorGamma->Correct(mainColor);
			if (animations->IsAnimationActive(actpixel)) {
				animations->StopAnimation(actpixel);
			}
			animations->StartAnimation(actpixel, 200, BlendAnimUpdate);
		}
	}
	lastPixel = nextPixel;

	if (param.state == AnimationState_Completed)
	{
		// done, time to restart this position tracking animation/timer
		animations->RestartAnimation(pixelCount + ANIMATION_CYLON);
	}
}

void Uart_GRBW_Led_Device_Driver::RandomAnimUpdate(const AnimationParam & param)
{
	if (param.state != AnimationState_Completed)
	{
		uint16_t count = random(pixelCount);

		while (count > 0)
		{
			// pick a random pixel
			uint16_t pixel = random(pixelCount);

			// pick random time and random color
			// we use HslColor object as it allows us to easily pick a color
			// with the same saturation and luminance 
			if (!(animations->IsAnimationActive(pixel))) {
				uint16_t time = random(100, 400);
				animationState_list[pixel]->StartingColor = strip->GetPixelColor(pixel);
				animationState_list[pixel]->EndingColor = HslColor(random(360) / 360.0f, 1.0f, 0.5f);
				animations->StartAnimation(pixel, time, BlendAnimUpdate);
			}
			count--;
		}
	}
	else {
		animations->RestartAnimation(pixelCount + ANIMATION_RANDOM);
	}
}


void Uart_GRBW_Led_Device_Driver::BlendAnimUpdate(const AnimationParam& param)
{
	// this gets called for each animation on every time step
	// progress will start at 0.0 and end at 1.0
	// we use the blend function on the RgbColor to mix
	// color based on the progress given to us in the animation
	RgbwColor updatedColor = RgbwColor::LinearBlend(
		animationState_list[param.index]->StartingColor,
		animationState_list[param.index]->EndingColor,
		param.progress);
	// apply the color to the strip
	strip->SetPixelColor(param.index, updatedColor);
}


void Uart_GRBW_Led_Device_Driver::FireAnimUpdate(const AnimationParam & param)
{
	if (param.state != AnimationState_Completed)
	{
		uint16_t count = random(pixelCount);

		while (count > 0)
		{
			// pick a random pixel
			uint16_t pixel = random(pixelCount);

			// pick random time and random color
			// we use HslColor object as it allows us to easily pick a color
			// with the same saturation and luminance 
			if (!(animations->IsAnimationActive(pixel))) {
				uint16_t time = random(40, 300);
				uint8 region = 255;
				uint8 regionR = region * mainColor.R * 1 / 255;
				uint8 regionG = region * mainColor.G * 1 / 255;
				uint8 regionB = region * mainColor.B * 1 / 255;

				uint8 r = random(fmax(0, mainColor.R - (regionR / 2)), fmin(255, mainColor.R + (regionR / 2)));
				uint8 g = random(fmax(0, mainColor.G - (regionG / 2)), fmin(255, mainColor.G + (regionG / 2)));
				uint8 b = random(fmax(0, mainColor.B - (regionB / 2)), fmin(255, mainColor.B + (regionB / 2)));

				animationState_list[pixel]->StartingColor = strip->GetPixelColor(pixel);
				animationState_list[pixel]->EndingColor = colorGamma->Correct(RgbColor(r, g, b));
				animations->StartAnimation(pixel, time, BlendAnimUpdate);
			}
			count--;
		}
	}
	else {
		animations->RestartAnimation(pixelCount + ANIMATION_FIRE);
	}
}

void Uart_GRBW_Led_Device_Driver::ShineAnimUpdate(const AnimationParam& param) {
	if (param.state != AnimationState_Completed)
	{
		for (int pixel = 0; pixel < pixelCount; pixel++) {
			strip->SetPixelColor(pixel, colorGamma->Correct(mainColor));
		}
	}
	else {
		animations->RestartAnimation(pixelCount + ANIMATION_SHINE);
	}
}

void Uart_GRBW_Led_Device_Driver::Animation_Off() {
	animations->StopAll();
	for (int i = 0; i < pixelCount; i++) {
		strip->SetPixelColor(i, RgbColor(0, 0, 0));
	}
	strip->Show();
}

void Uart_GRBW_Led_Device_Driver::Animation_Shine() {
	animations->StopAll();
	animations->StartAnimation(pixelCount + ANIMATION_SHINE, 200, ShineAnimUpdate);
}

void Uart_GRBW_Led_Device_Driver::Animation_Random() {
	Animation_Off();
	animations->StartAnimation(pixelCount + ANIMATION_RANDOM, 200, RandomAnimUpdate);
}

void Uart_GRBW_Led_Device_Driver::Animation_Cyclon() {
	Animation_Off();
	lastPixel = 0;
	moveDir = 1;
	animations->StartAnimation(pixelCount + ANIMATION_CYLON, 2000, CylonAnimUpdate);
}

void Uart_GRBW_Led_Device_Driver::Animation_Fire() {
	Animation_Off();
	animations->StartAnimation(pixelCount + ANIMATION_FIRE, 200, FireAnimUpdate);
}

void Uart_GRBW_Led_Device_Driver::Animation_Next() {
	actAnimation = (actAnimation + 1) % (ANIMATION_COUNT);
	switch (actAnimation)
	{
	case 0:
		Animation_Cyclon();
		break;
	case 1:
		Animation_Random();
		break;
	case 2:
		Animation_Fire();
		break;
	case 3:
		Animation_Shine();
		break;
	default:
		Animation_Off();
	}
}

void Uart_GRBW_Led_Device_Driver::Animation_Prev() {
	actAnimation = (actAnimation - 1);
	//Buffer overflow
	if (actAnimation >= ANIMATION_COUNT) {
		actAnimation = ANIMATION_LAST;
	}
	switch (actAnimation)
	{
	case 0:
		Animation_Cyclon();
		break;
	case 1:
		Animation_Random();
		break;
	case 2:
		Animation_Fire();
		break;
	case 3:
		Animation_Shine();
		break;
	default:
		Animation_Off();
	}
}

void Uart_GRBW_Led_Device_Driver::Animation_Color(int R, int G, int B)
{
	mainColor = RgbColor(R, G, B);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Off()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_RGB_LED_DEVICE_OFF);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Shine()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_RGB_LED_DEVICE_SHINE);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Random()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_RGB_LED_DEVICE_RANDOM);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Cyclon() {
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_RGB_LED_DEVICE_CYCLON);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Fire() {
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_RGB_LED_DEVICE_FIRE);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Next() {
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_RGB_LED_DEVICE_NEXT);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Prev() {
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_RGB_LED_DEVICE_PREV);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Color(int R, int G, int B)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_RGB_LED_DEVICE_COLOR);
	message->AddParam(R);
	message->AddParam(G);
	message->AddParam(B);
	PostMessage(&message);
}
