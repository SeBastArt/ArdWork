// 
// 
// 

#include "Uart_GRBW_Led_Device_Driver.h"
#include "math.h"

uint8_t Uart_GRBW_Led_Device_Driver::pixelCount;
uint16_t Uart_GRBW_Led_Device_Driver::lastPixel = 0;
int8_t Uart_GRBW_Led_Device_Driver::moveDir = 1;
Vector <GRBWAnimationState*> Uart_GRBW_Led_Device_Driver::animationState_list;
HslColor Uart_GRBW_Led_Device_Driver::mainColor = RgbColor(150, 0, 0);

NeoGamma<NeoGammaTableMethod>* Uart_GRBW_Led_Device_Driver::colorGamma;
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>* Uart_GRBW_Led_Device_Driver::strip;
NeoPixelAnimator* Uart_GRBW_Led_Device_Driver::animations;

Uart_GRBW_Led_Device_Driver::Uart_GRBW_Led_Device_Driver(Module_Driver* module, uint8_t _pixelcount, uint8_t priority) :
	Device_Driver(module, priority)
{
#ifdef  DEBUG
	Serial.print("Start Constructor Uart_GRBW_Led_Device_Driver with ID: ");
	Serial.println(this->DriverId);
#endif //  DEBUG
	__DriverType = UART_GRBW_LED_DEVICE_TYPE;
	pixelCount = _pixelcount;
	__brightness = 50;
	colorGamma = new NeoGamma<NeoGammaTableMethod>; // for any fade animations, best to correct gamma
	strip = new NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>(_pixelcount, 0);
	animations = new NeoPixelAnimator(_pixelcount + GRBW_ANIMATION_COUNT); // NeoPixel animation management object

	for (uint8_t i = 0; i < _pixelcount; i++) {
		GRBWAnimationState* animationState = new GRBWAnimationState();
		animationState_list.PushBack(animationState);
	}
	strip->Begin();
#ifdef  DEBUG
	Serial.print("Ende Constructor Uart_GRBW_Led_Device_Driver");
#endif //  DEBUG
};

void Uart_GRBW_Led_Device_Driver::OnBuild_Descriptor() {
#ifdef  DEBUG
	Serial.println("Start Uart_GRBW_Led_Device_Driver::Build_Descriptor");
#endif //  DEBUG
	__descriptor->name = F("GRBW-Stripe");
	__descriptor->descr = F("GRBW-Stripe stellt die Steuerung der GRBW-LEDs bereit es erlaubt die Kontrolle &uuml;ber die Muster und Farben");
	__descriptor->published = false;

	Select_CtrlElem *ctrlElem_pattern = new Select_CtrlElem(UART_GRBW_LED_DEVICE_PATTERN, &sv_pattern, F("Pattern"), F("Choose a pattern for the ambilight"));
	ctrlElem_pattern->AddMember("Cyclon");
	ctrlElem_pattern->AddMember("Random");
	ctrlElem_pattern->AddMember("Fire");
	ctrlElem_pattern->AddMember("Shine");
	ctrlElem_pattern->AddMember("Off");

	Color_CtrlElem *ctrlElem_color = new Color_CtrlElem(UART_GRBW_LED_DEVICE_COLOR_HEX, &sv_color, F("Color"), F("Set the main color of the pattern"));

	Value_CtrlElem *ctrlElem_brightess = new Value_CtrlElem(UART_GRBW_LED_DEVICE_BRIGHTNESS, &sv_relBrightness, true, F("brightness"), F("the brightness for the ambient light from 1% to 200%"));
	ctrlElem_brightess->unit = "%";

	__descriptor->Add_Descriptor_Element(ctrlElem_pattern);
	__descriptor->Add_Descriptor_Element(ctrlElem_color);
	__descriptor->Add_Descriptor_Element(ctrlElem_brightess);
#ifdef  DEBUG
	Serial.println("Ende Uart_GRBW_Led_Device_Driver::Build_Descriptor");
#endif //  DEBUG
}


void Uart_GRBW_Led_Device_Driver::OnInit()
{
	Serial.println("Hier");
	Device_Driver::OnInit();
	SetRandomSeed();
	sv_pattern = GRBW_ANIMATION_SHINE;
	Animation_Off();
	Set_Animation();
}

void Uart_GRBW_Led_Device_Driver::DoDeviceMessage(Int_Task_Msg message)
{
	int messageID = message.id;
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
	case UART_GRBW_LED_DEVICE_COLOR_RGB:
	{
		int R = message.GetIntParamByIndex(0);
		int G = message.GetIntParamByIndex(1);
		int B = message.GetIntParamByIndex(2);
		Animation_Color(R, G, B);
	}
	break;
	case UART_GRBW_LED_DEVICE_BRIGHTNESS:
	{
		uint8_t brightness = message.GetIntParamByIndex(0);
		SetBrightness(brightness);
	}
	break;
	case UART_GRBW_LED_DEVICE_PATTERN:
	{
		unsigned int animation_number = message.GetIntParamByIndex(0);
		Animation_Number(animation_number);
	}
	break;
	case UART_GRBW_LED_DEVICE_COLOR_HEX:
	{
		uint32_t rgb = (uint32_t)strtol(message.GetStringParamByIndex(0).c_str(), NULL, 16);
		sv_color.R = (uint8_t)((rgb >> 16) & 0xFF);
		sv_color.G = (uint8_t)((rgb >> 8) & 0xFF);
		sv_color.B = (uint8_t)((rgb >> 0) & 0xFF);
		Animation_Color((uint8_t)((rgb >> 16) & 0xFF), (uint8_t)((rgb >> 8) & 0xFF), (uint8_t)((rgb >> 0) & 0xFF));
	}
	break;
	}
}

void Uart_GRBW_Led_Device_Driver::DoUpdate(uint32_t deltaTime) {
	animations->UpdateAnimations();
	strip->Show();
}


void Uart_GRBW_Led_Device_Driver::SetBrightness(uint8_t _brightness) {
	__brightness = MIN(_brightness, 100);
	__brightness = MAX(__brightness, 1);

	_brightness = MIN(_brightness, 99);
	_brightness = MAX(_brightness, 1);
	sv_relBrightness = _brightness;
	float light;
	light = _brightness / 100.0;
	mainColor.L = light;
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
	randomSeed(seed);
}


void Uart_GRBW_Led_Device_Driver::CylonAnimUpdate(const AnimationParam& param)
{
	uint16_t nextPixel;

	float progress = NeoEase::Linear(param.progress);
	nextPixel = round(progress * pixelCount) % pixelCount;
	if ((lastPixel != nextPixel)) {
		uint8_t count = 2;
		for (uint8_t j = 0; j < count; j++) {
			uint8_t actpixel = (nextPixel + (j * (pixelCount / count))) % pixelCount;

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
		animations->RestartAnimation(pixelCount + GRBW_ANIMATION_FIRE - GRBW_ANIMATION_FIRST);
	}
}

void Uart_GRBW_Led_Device_Driver::ShineAnimUpdate(const AnimationParam& param) {
	if (param.state != AnimationState_Completed)
	{
		for (int pixel = 0; pixel < pixelCount; pixel++) {
			strip->SetPixelColor(pixel, mainColor);
		}
	}
	else {
		animations->RestartAnimation(pixelCount + GRBW_ANIMATION_SHINE - GRBW_ANIMATION_FIRST);
	}
}

void Uart_GRBW_Led_Device_Driver::Animation_Off() {
	for (uint8_t i = 0; i < pixelCount; i++) {
		strip->SetPixelColor(i, RgbColor(0, 0, 0));
	}
	strip->Show();
}

void Uart_GRBW_Led_Device_Driver::Animation_Shine() {
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
	sv_pattern++;
	if (sv_pattern > GRBW_ANIMATION_LAST)
		sv_pattern = GRBW_ANIMATION_FIRST;
	Set_Animation();
}

void Uart_GRBW_Led_Device_Driver::Animation_Number(unsigned int _animation_number) {
	sv_pattern = _animation_number;
	Set_Animation();
}

void Uart_GRBW_Led_Device_Driver::Animation_Prev() {
	sv_pattern--;
	if (sv_pattern < GRBW_ANIMATION_FIRST)
		sv_pattern = GRBW_ANIMATION_LAST;
	Set_Animation();
}

void Uart_GRBW_Led_Device_Driver::Set_Animation() {
	switch (sv_pattern)
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
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_OFF);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Shine()
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_SHINE);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Random()
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_RANDOM);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Cyclon() {
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_CYCLON);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Fire() {
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_FIRE);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Next() {
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_NEXT);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Animation_Prev() {
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_PREV);
	PostMessage(&message);
}


void Uart_GRBW_Led_Device_Driver::Exec_Animation_Color(int R, int G, int B)
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_COLOR_RGB);
	message->AddParam(R);
	message->AddParam(G);
	message->AddParam(B);
	PostMessage(&message);
}

void Uart_GRBW_Led_Device_Driver::Exec_Set_Brightness(int _brightness)
{
	Int_Task_Msg *message = new Int_Task_Msg(UART_GRBW_LED_DEVICE_BRIGHTNESS);
	message->AddParam(_brightness);
	PostMessage(&message);
}

