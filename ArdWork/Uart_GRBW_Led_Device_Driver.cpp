// 
// 
// 

#include "Uart_GRBW_Led_Device_Driver.h"
#include "math.h"

uint8_t Uart_GRBW_Led_Device_Driver::pixelCount;
uint16_t Uart_GRBW_Led_Device_Driver::lastPixel = 0;
int8_t Uart_GRBW_Led_Device_Driver::moveDir = 1;
Vector <GRBWAnimationState*> Uart_GRBW_Led_Device_Driver::animationState_list;
RgbColor Uart_GRBW_Led_Device_Driver::mainColor = RgbColor(150, 0, 0);

NeoGamma<NeoGammaTableMethod>* Uart_GRBW_Led_Device_Driver::colorGamma;
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>* Uart_GRBW_Led_Device_Driver::strip;
NeoPixelAnimator* Uart_GRBW_Led_Device_Driver::animations;

Uart_GRBW_Led_Device_Driver::Uart_GRBW_Led_Device_Driver(Module_Driver* module, uint8_t _pixelcount, uint8_t priority) :
	Device_Driver(module, priority)
{
	driver_name = "Uart_GRBW_Led_Device_Driver";

	pixelCount = _pixelcount;
	colorGamma = new NeoGamma<NeoGammaTableMethod>; // for any fade animations, best to correct gamma
	strip = new NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>(_pixelcount);
	animations = new NeoPixelAnimator(_pixelcount + GRBW_ANIMATION_COUNT); // NeoPixel animation management object

	for (uint8_t i = 0; i < _pixelcount; i++) {
		GRBWAnimationState* animationState = new GRBWAnimationState();
		animationState_list.PushBack(animationState);
	}
	strip->Begin();
};

void Uart_GRBW_Led_Device_Driver::Build_Descriptor() {
	__descriptor->name = "GRBW-Stripe";
	__descriptor->descr = "GRBW-Stripe stellt die Steuerung der GRBW-LEDs bereit es erlaubt die Kontrolle &uuml;ber die Muster und Farben";
	__descriptor->published = true;

	Ctrl_Elem *ctrl_elem_pattern = new Ctrl_Elem(UART_GRBW_LED_SET_PATTERN_EXTERN, "Pattern", select, "Choose a pattern for the ambient light");
	Atomic<String> *atomic_pattern_cyclon = new Atomic<String>(GRBW_ANIMATION_CYLON, "Cyclon");
	Atomic<String> *atomic_pattern_random = new Atomic<String>(GRBW_ANIMATION_RANDOM, "Random");
	Atomic<String> *atomic_pattern_fire = new Atomic<String>(GRBW_ANIMATION_FIRE, "Fire");
	Atomic<String> *atomic_pattern_shine = new Atomic<String>(GRBW_ANIMATION_SHINE, "Shine");
	Atomic<String> *atomic_pattern_off = new Atomic<String>(GRBW_ANIMATION_OFF, "Off");

	ctrl_elem_pattern->AddAtomic(atomic_pattern_cyclon);
	ctrl_elem_pattern->AddAtomic(atomic_pattern_random);
	ctrl_elem_pattern->AddAtomic(atomic_pattern_fire);
	ctrl_elem_pattern->AddAtomic(atomic_pattern_shine);
	ctrl_elem_pattern->AddAtomic(atomic_pattern_off);
	ctrl_elem_pattern->published = true;

	Ctrl_Elem *ctrl_elem_color = new Ctrl_Elem(UART_GRBW_LED_SET_COLOR_EXTERN, "Color", color, "The main color for the ambient light pattern");
	Atomic<uint8_t> *atomic_color_r = new Atomic<uint8_t>(0, 227, "Dec");
	Atomic<uint8_t> *atomic_color_g = new Atomic<uint8_t>(1, 227, "Dec");
	Atomic<uint8_t> *atomic_color_b = new Atomic<uint8_t>(2, 227, "Dec");

	ctrl_elem_color->AddAtomic(atomic_color_r);
	ctrl_elem_color->AddAtomic(atomic_color_g);
	ctrl_elem_color->AddAtomic(atomic_color_b);
	ctrl_elem_color->published = true;

	__descriptor->Add_Descriptor_Element(ctrl_elem_pattern);
	__descriptor->Add_Descriptor_Element(ctrl_elem_color);
}


void Uart_GRBW_Led_Device_Driver::DoAfterInit()
{
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
	case UART_GRBW_LED_DEVICE_OFF:
	{
		Animation_Off();
	}
	break;
	case UART_GRBW_LED_DEVICE_CYCLON:
	{
		Animation_Cyclon();
	}
	break;
	case UART_GRBW_LED_DEVICE_RANDOM:
	{
		Animation_Random();
	}
	break;
	case UART_GRBW_LED_DEVICE_FIRE:
	{
		Animation_Fire();
	}
	break;
	case UART_GRBW_LED_DEVICE_SHINE:
	{
		Animation_Shine();
	}
	break;
	case UART_GRBW_LED_DEVICE_NEXT:
	{
		Animation_Next();
	}
	break;
	case UART_GRBW_LED_DEVICE_PREV:
	{
		Animation_Prev();
	}
	break;
	case UART_GRBW_LED_DEVICE_COLOR:
	{
		uint8_t R = message.GetUint8ParamByIndex(1);
		uint8_t G = message.GetUint8ParamByIndex(2);
		uint8_t B = message.GetUint8ParamByIndex(3);
		Animation_Color(R, G, B);
	}
	break;
	case UART_GRBW_LED_SET_PATTERN_EXTERN:
	{
		uint8_t animation_number = message.GetUint8ParamByIndex(1);
		Animation_Number(animation_number);
	}
	break;
	case UART_GRBW_LED_SET_COLOR_EXTERN:
	{
		uint32_t rgb = (uint32_t)strtol(message.GetStringParamByIndex(1).c_str(), NULL, 16);
		Serial.print("R: ");
		Serial.print(((rgb >> 16) & 0xFF));
		Serial.print(", G: ");
		Serial.print(((rgb >> 8) & 0xFF));
		Serial.print(", B: ");
		Serial.println(((rgb >> 0) & 0xFF));
		Animation_Color((uint8_t)(rgb >> 16), (uint8_t)(rgb >> 8), (uint8_t)(rgb >> 0));
	}
	break;
	}
}

void Uart_GRBW_Led_Device_Driver::DoUpdate(uint32_t deltaTime) {
	animations->UpdateAnimations();
	strip->Show();
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
		uint8_t count = 2;
		for (uint8_t j = 0; j < count; j++) {
			uint8_t actpixel = (nextPixel + (j * (pixelCount / count))) % pixelCount;

			RgbwColor startingColor;
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
		animations->RestartAnimation(pixelCount + GRBW_ANIMATION_CYLON - GRBW_ANIMATION_FIRST);
	}
}

void Uart_GRBW_Led_Device_Driver::RandomAnimUpdate(const AnimationParam & param)
{
	if (param.state != AnimationState_Completed)
	{
		uint8_t count = random(pixelCount);

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
		animations->RestartAnimation(pixelCount + GRBW_ANIMATION_RANDOM - GRBW_ANIMATION_FIRST);
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
		uint8_t count = random(pixelCount);

		while (count > 0)
		{
			// pick a random pixel
			uint8_t pixel = random(pixelCount);

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
		animations->RestartAnimation(pixelCount + GRBW_ANIMATION_FIRE - GRBW_ANIMATION_FIRST);
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
		animations->RestartAnimation(pixelCount + GRBW_ANIMATION_SHINE - GRBW_ANIMATION_FIRST);
	}
}

void Uart_GRBW_Led_Device_Driver::Animation_Off() {
	animations->StopAll();
	for (uint8_t i = 0; i < pixelCount; i++) {
		strip->SetPixelColor(i, RgbColor(0, 0, 0));
	}
	strip->Show();
}

void Uart_GRBW_Led_Device_Driver::Animation_Shine() {
	animations->StopAll();
	animations->StartAnimation(pixelCount + GRBW_ANIMATION_SHINE - GRBW_ANIMATION_FIRST, 200, ShineAnimUpdate);
}

void Uart_GRBW_Led_Device_Driver::Animation_Random() {
	Animation_Off();
	animations->StartAnimation(pixelCount + GRBW_ANIMATION_RANDOM - GRBW_ANIMATION_FIRST, 200, RandomAnimUpdate);
}

void Uart_GRBW_Led_Device_Driver::Animation_Cyclon() {
	Animation_Off();
	lastPixel = 0;
	moveDir = 1;
	animations->StartAnimation(pixelCount + GRBW_ANIMATION_CYLON - GRBW_ANIMATION_FIRST, 2000, CylonAnimUpdate);
}

void Uart_GRBW_Led_Device_Driver::Animation_Fire() {
	Animation_Off();
	animations->StartAnimation(pixelCount + GRBW_ANIMATION_FIRE - GRBW_ANIMATION_FIRST, 200, FireAnimUpdate);
}

void Uart_GRBW_Led_Device_Driver::Animation_Next() {
	actAnimation++;
	if (actAnimation > GRBW_ANIMATION_LAST)
		actAnimation = GRBW_ANIMATION_FIRST;
	Set_Animation();
}

void Uart_GRBW_Led_Device_Driver::Animation_Number(uint8_t _animation_number) {
	actAnimation = _animation_number;
	Set_Animation();
}

void Uart_GRBW_Led_Device_Driver::Animation_Prev() {
	actAnimation--;
	if (actAnimation < GRBW_ANIMATION_FIRST)
		actAnimation = GRBW_ANIMATION_LAST;
	Set_Animation();
}

void Uart_GRBW_Led_Device_Driver::Set_Animation() {
	switch (actAnimation)
	{
	case GRBW_ANIMATION_CYLON:
		Animation_Cyclon();
		break;
	case GRBW_ANIMATION_RANDOM:
		Animation_Random();
		break;
	case GRBW_ANIMATION_FIRE:
		Animation_Fire();
		break;
	case GRBW_ANIMATION_SHINE:
		Animation_Shine();
		break;
	default:
		Animation_Off();
	}
}

void Uart_GRBW_Led_Device_Driver::Animation_Color(uint8_t R, uint8_t G, uint8_t B)
{
	mainColor = RgbColor(R, G, B);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Off()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_GRBW_LED_DEVICE_OFF);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Shine()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_GRBW_LED_DEVICE_SHINE);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Random()
{
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_GRBW_LED_DEVICE_RANDOM);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Cyclon() {
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_GRBW_LED_DEVICE_CYCLON);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Fire() {
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_GRBW_LED_DEVICE_FIRE);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Next() {
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_GRBW_LED_DEVICE_NEXT);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Prev() {
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_GRBW_LED_DEVICE_PREV);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Color(uint8_t R, uint8_t G, uint8_t B)
{
	Int_Thread_Msg *message = new Int_Thread_Msg(UART_GRBW_LED_DEVICE_COLOR);
	message->AddParam(R);
	message->AddParam(G);
	message->AddParam(B);
	PostMessage(&message);
}
