// 
// 
// 

#include "Uart_RGB_Led_Device_Driver.h"
#include "math.h"

uint8_t Uart_RGB_Led_Device_Driver::__pixelCount;
uint16_t Uart_RGB_Led_Device_Driver::lastPixel = 0;
int8_t Uart_RGB_Led_Device_Driver::moveDir = 1;
Vector <RGBAnimationState*> Uart_RGB_Led_Device_Driver::animationState_list;
HslColor Uart_RGB_Led_Device_Driver::mainColor = RgbColor(150, 0, 0);

NeoGamma<NeoGammaTableMethod>* Uart_RGB_Led_Device_Driver::colorGamma;
NeoPixelBus<NeoRgbFeature, NeoEsp8266Dma800KbpsMethod>* Uart_RGB_Led_Device_Driver::strip;
NeoPixelAnimator* Uart_RGB_Led_Device_Driver::animations;

REGISTERIMPL(Uart_RGB_Led_Device_Driver);

Uart_RGB_Led_Device_Driver::Uart_RGB_Led_Device_Driver(Module_Driver* module, uint8_t priority) :
	Device_Driver(module, priority)
{
	__DriverType = UART_RGB_LED_DEVICE_TYPE;
	__pixelCount = 0;
	__brightness = 50;
	colorGamma = new NeoGamma<NeoGammaTableMethod>; // for any fade animations, best to correct gamma
	
};

void Uart_RGB_Led_Device_Driver::OnBuild_Descriptor() {
	__descriptor->name = F("RGB-Stripe");
	__descriptor->descr = F("RGB-Stripe");
	__descriptor->published = false;

	//Ctrl_Elem *ctrl_elem_pattern = new Ctrl_Elem(UART_RGB_LED_PATTERN, F("Pattern"), select, F("Choose a pattern for the ambient light"));
	//Atomic<String> *atomic_pattern_cyclon = new Atomic<String>(RGB_ANIMATION_CYLON, F("Cyclon"));
	//Atomic<String> *atomic_pattern_random = new Atomic<String>(RGB_ANIMATION_RANDOM, F("Random"));
	//Atomic<String> *atomic_pattern_fire = new Atomic<String>(RGB_ANIMATION_FIRE, F("Fire"));
	//Atomic<String> *atomic_pattern_shine = new Atomic<String>(RGB_ANIMATION_SHINE, F("Shine"));
	//Atomic<String> *atomic_pattern_off = new Atomic<String>(RGB_ANIMATION_OFF, F("Off"));

	//ctrl_elem_pattern->AddAtomic(atomic_pattern_cyclon);
	//ctrl_elem_pattern->AddAtomic(atomic_pattern_random);
	//ctrl_elem_pattern->AddAtomic(atomic_pattern_fire);
	//ctrl_elem_pattern->AddAtomic(atomic_pattern_shine);
	//ctrl_elem_pattern->AddAtomic(atomic_pattern_off);
	//ctrl_elem_pattern->published = true;

	//Ctrl_Elem *ctrl_elem_color = new Ctrl_Elem(UART_RGB_LED_COLOR_HEX, F("Color"), color, F("The main color for the ambient light pattern"));
	//Atomic<uint8_t> *atomic_color_r = new Atomic<uint8_t>(0, 220, F("Dec"));
	//Atomic<uint8_t> *atomic_color_g = new Atomic<uint8_t>(1, 123, F("Dec"));
	//Atomic<uint8_t> *atomic_color_b = new Atomic<uint8_t>(2, 234, F("Dec"));

	//ctrl_elem_color->AddAtomic(atomic_color_r);
	//ctrl_elem_color->AddAtomic(atomic_color_g);
	//ctrl_elem_color->AddAtomic(atomic_color_b);
	//ctrl_elem_color->published = true;

	//Ctrl_Elem *ctrl_elem_brightess = new Ctrl_Elem(UART_RGB_LED_DEVICE_BRIGHTNESS, F("brightness"), edtvalue, F("the brightness for the ambient light pattern"));
	//Atomic<uint8_t> *atomic_brightess = new Atomic<uint8_t>(0, &__brightness, F("%"));
	//ctrl_elem_brightess->AddAtomic(atomic_brightess);
	//ctrl_elem_brightess->published = true;

	//__descriptor->Add_Descriptor_Element(ctrl_elem_pattern);
	//__descriptor->Add_Descriptor_Element(ctrl_elem_color);
	//__descriptor->Add_Descriptor_Element(ctrl_elem_brightess);
}


void Uart_RGB_Led_Device_Driver::SetPixelCount(int _pixelCount)
{
	__pixelCount = _pixelCount;
	InitStrip();
}

void Uart_RGB_Led_Device_Driver::InitStrip()
{
	Animation_Off();

	if (__pixelCount < 1)
		return;

	if (strip != nullptr) {
		delete strip;
		strip = nullptr;
	}

	if (animations != nullptr) {
		delete animations;
		animations = nullptr;
	}

	animationState_list.Clear();

	strip = new NeoPixelBus<NeoRgbFeature, NeoEsp8266Dma800KbpsMethod>(__pixelCount, 0);
	animations = new NeoPixelAnimator(__pixelCount + RGB_ANIMATION_COUNT); // NeoPixel animation management object

	for (uint8_t i = 0; i < __pixelCount; i++) {
		RGBAnimationState* animationState = new RGBAnimationState();
		animationState_list.PushBack(animationState);
	}
	strip->Begin();
	//sv_pattern = GRBW_ANIMATION_SHINE;
	Animation_Off();
	Set_Animation();
}


void Uart_RGB_Led_Device_Driver::OnInit()
{
	Device_Driver::OnInit();
	SetRandomSeed();
	InitStrip();
	Serial.println(F("Uart_Rgb_Led-Driver initialized!"));
}

void Uart_RGB_Led_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
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
		int R = message.GetIntParamByIndex(0);
		int G = message.GetIntParamByIndex(1);
		int B = message.GetIntParamByIndex(2);
		Animation_Color(R, G, B);
	}
	break;
	case UART_RGB_LED_DEVICE_BRIGHTNESS:
	{
		uint8_t brightness = message.GetIntParamByIndex(0);
		SetBrightness(brightness);
	}
	break;
	case UART_RGB_LED_PATTERN:
	{
		unsigned int animation_number = message.GetIntParamByIndex(0);
		Animation_Number(animation_number);
	}
	break;
	case UART_RGB_LED_COLOR_HEX:
	{
		uint32_t rgb = (uint32_t)strtol(message.GetStringParamByIndex(0).c_str(), NULL, 16);
		/*Serial.print("R: ");
		Serial.print(((rgb >> 16) & 0xFF));
		Serial.print(", G: ");
		Serial.print(((rgb >> 8) & 0xFF));
		Serial.print(", B: ");
		Serial.println(((rgb >> 0) & 0xFF));*/
		Animation_Color((uint8_t)(rgb >> 16), (uint8_t)(rgb >> 8), (uint8_t)(rgb >> 0));
	}
	break;
	}
}

void Uart_RGB_Led_Device_Driver::DoUpdate(uint32_t deltaTime) {
	animations->UpdateAnimations();
	strip->Show();
}


void Uart_RGB_Led_Device_Driver::SetBrightness(uint8_t _brightness) {
	__brightness = MIN(_brightness, 100);
	__brightness = MAX(__brightness, 1);

	_brightness = MIN(_brightness, 99);
	_brightness = MAX(_brightness, 1);
	float light;
	light = _brightness / 100.0;
	mainColor.L = light;
}

void Uart_RGB_Led_Device_Driver::SetRandomSeed()
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
	randomSeed(seed);
}


void Uart_RGB_Led_Device_Driver::CylonAnimUpdate(const AnimationParam& param)
{
	uint16_t nextPixel;

	float progress = NeoEase::Linear(param.progress);
	nextPixel = round(progress * __pixelCount) % __pixelCount;
	if ((lastPixel != nextPixel)) {
		uint8_t count = 2;
		for (uint8_t j = 0; j < count; j++) {
			uint8_t actpixel = (nextPixel + (j * (__pixelCount / count))) % __pixelCount;

			RgbColor startingColor;
			RgbColor tempRGBColor = mainColor;
			startingColor.R = fmin(255, tempRGBColor.R + 200);
			startingColor.G = fmin(255, tempRGBColor.G + 200);
			startingColor.B = fmin(255, tempRGBColor.B + 200);

			strip->SetPixelColor(actpixel, colorGamma->Correct(startingColor));
			animationState_list[actpixel]->StartingColor = colorGamma->Correct(startingColor);
			animationState_list[actpixel]->EndingColor = mainColor;
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
		animations->RestartAnimation(__pixelCount + RGB_ANIMATION_CYLON - RGB_ANIMATION_FIRST);
	}
}

void Uart_RGB_Led_Device_Driver::RandomAnimUpdate(const AnimationParam & param)
{
	if (param.state != AnimationState_Completed)
	{
		uint8_t count = random(__pixelCount);

		while (count > 0)
		{
			// pick a random pixel
			uint16_t pixel = random(__pixelCount);

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
		animations->RestartAnimation(__pixelCount + RGB_ANIMATION_RANDOM - RGB_ANIMATION_FIRST);
	}
}


void Uart_RGB_Led_Device_Driver::BlendAnimUpdate(const AnimationParam& param)
{
	// this gets called for each animation on every time step
	// progress will start at 0.0 and end at 1.0
	// we use the blend function on the RgbColor to mix
	// color based on the progress given to us in the animation
	RgbColor updatedColor = RgbColor::LinearBlend(
		animationState_list[param.index]->StartingColor,
		animationState_list[param.index]->EndingColor,
		param.progress);
	// apply the color to the strip
	strip->SetPixelColor(param.index, updatedColor);
}


void Uart_RGB_Led_Device_Driver::FireAnimUpdate(const AnimationParam & param)
{
	if (param.state != AnimationState_Completed)
	{
		uint8_t count = random(__pixelCount);

		while (count > 0)
		{
			// pick a random pixel
			uint8_t pixel = random(__pixelCount);

			// pick random time and random color
			// we use HslColor object as it allows us to easily pick a color
			// with the same saturation and luminance 
			if (!(animations->IsAnimationActive(pixel))) {
				uint16_t time = random(40, 300);
				uint8 region = 255;
				RgbColor tempRGBColor = mainColor;
				uint8 regionR = region * tempRGBColor.R * 1 / 255;
				uint8 regionG = region * tempRGBColor.G * 1 / 255;
				uint8 regionB = region * tempRGBColor.B * 1 / 255;

				uint8 r = random(fmax(0, tempRGBColor.R - (regionR / 2)), fmin(255, tempRGBColor.R + (regionR / 2)));
				uint8 g = random(fmax(0, tempRGBColor.G - (regionG / 2)), fmin(255, tempRGBColor.G + (regionG / 2)));
				uint8 b = random(fmax(0, tempRGBColor.B - (regionB / 2)), fmin(255, tempRGBColor.B + (regionB / 2)));

				animationState_list[pixel]->StartingColor = strip->GetPixelColor(pixel);
				animationState_list[pixel]->EndingColor = colorGamma->Correct(RgbColor(r, g, b));
				animations->StartAnimation(pixel, time, BlendAnimUpdate);
			}
			count--;
		}
	}
	else {
		animations->RestartAnimation(__pixelCount + RGB_ANIMATION_FIRE - RGB_ANIMATION_FIRST);
	}
}

void Uart_RGB_Led_Device_Driver::ShineAnimUpdate(const AnimationParam& param) {
	if (param.state != AnimationState_Completed)
	{
		for (int pixel = 0; pixel < __pixelCount; pixel++) {
			strip->SetPixelColor(pixel, mainColor);
		}
	}
	else {
		animations->RestartAnimation(__pixelCount + RGB_ANIMATION_SHINE - RGB_ANIMATION_FIRST);
	}
}

void Uart_RGB_Led_Device_Driver::Animation_Off() {
	animations->StopAll();
	for (uint8_t i = 0; i < __pixelCount; i++) {
		strip->SetPixelColor(i, RgbColor(0, 0, 0));
	}
	strip->Show();
}

void Uart_RGB_Led_Device_Driver::Animation_Shine() {
	animations->StartAnimation(__pixelCount + RGB_ANIMATION_SHINE - RGB_ANIMATION_FIRST, 200, ShineAnimUpdate);
}

void Uart_RGB_Led_Device_Driver::Animation_Random() {
	Animation_Off();
	animations->StartAnimation(__pixelCount + RGB_ANIMATION_RANDOM - RGB_ANIMATION_FIRST, 200, RandomAnimUpdate);
}

void Uart_RGB_Led_Device_Driver::Animation_Cyclon() {
	Animation_Off();
	lastPixel = 0;
	moveDir = 1;
	animations->StartAnimation(__pixelCount + RGB_ANIMATION_CYLON - RGB_ANIMATION_FIRST, 2000, CylonAnimUpdate);
}

void Uart_RGB_Led_Device_Driver::Animation_Fire() {
	Animation_Off();
	animations->StartAnimation(__pixelCount + RGB_ANIMATION_FIRE - RGB_ANIMATION_FIRST, 200, FireAnimUpdate);
}

void Uart_RGB_Led_Device_Driver::Animation_Next() {
	actAnimation++;
	if (actAnimation > RGB_ANIMATION_LAST)
		actAnimation = RGB_ANIMATION_FIRST;
	Set_Animation();
}

void Uart_RGB_Led_Device_Driver::Animation_Number(unsigned int _animation_number) {
	actAnimation = _animation_number;
	Set_Animation();
}

void Uart_RGB_Led_Device_Driver::Animation_Prev() {
	actAnimation--;
	if (actAnimation < RGB_ANIMATION_FIRST)
		actAnimation = RGB_ANIMATION_LAST;
	Set_Animation();
}

void Uart_RGB_Led_Device_Driver::Set_Animation() {
	switch (actAnimation)
	{
	case RGB_ANIMATION_CYLON:
	{
		Animation_Cyclon();
	}
	break;
	case RGB_ANIMATION_RANDOM:
	{
		Animation_Random();
	}
	break;
	case RGB_ANIMATION_FIRE:
	{
		Animation_Fire();
	}
	break;
	case RGB_ANIMATION_SHINE:
	{
		Animation_Shine();
	}
	break;
	default:
	{
		Animation_Off();
	}
	}
}

void Uart_RGB_Led_Device_Driver::Animation_Color(uint8_t R, uint8_t G, uint8_t B)
{
	mainColor = RgbColor(G, R, B);
}

void Uart_RGB_Led_Device_Driver::Exec_Animation_Off()
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_OFF);
	PostMessage(&message);
}

void Uart_RGB_Led_Device_Driver::Exec_Animation_Shine()
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_SHINE);
	PostMessage(&message);
}

void Uart_RGB_Led_Device_Driver::Exec_Animation_Random()
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_RANDOM);
	PostMessage(&message);
}

void Uart_RGB_Led_Device_Driver::Exec_Animation_Cyclon() {
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_CYCLON);
	PostMessage(&message);
}

void Uart_RGB_Led_Device_Driver::Exec_Animation_Fire() {
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_FIRE);
	PostMessage(&message);
}

void Uart_RGB_Led_Device_Driver::Exec_Animation_Next() {
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_NEXT);
	PostMessage(&message);
}

void Uart_RGB_Led_Device_Driver::Exec_Animation_Prev() {
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_PREV);
	PostMessage(&message);
}

void Uart_RGB_Led_Device_Driver::Exec_Animation_Color(int R, int G, int B)
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_COLOR);
	message->AddParam(R);
	message->AddParam(G);
	message->AddParam(B);
	PostMessage(&message);
}

void Uart_RGB_Led_Device_Driver::Exec_Set_Brightness(int _brightness)
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_RGB_LED_DEVICE_BRIGHTNESS);
	message->AddParam(_brightness);
	PostMessage(&message);
}

