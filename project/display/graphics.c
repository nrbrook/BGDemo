/***********************************************************************************************//**
 * \file   graphics.c
 * \brief  Draws the graphics on the display
 ***************************************************************************************************
 * <b> (C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* standard headers */
#include <string.h>
#include <stdio.h>

#include "bg_types.h"
#include "em_types.h"
#include "glib.h"
#include "dmd.h"
#include "display.h"

#include <math.h>

/* Own header */
#include "graphics.h"

#define M_PI		3.14159265358979323846
typedef enum {
	GraphicsArrowDirectionN = 0,
	GraphicsArrowDirectionNE,
	GraphicsArrowDirectionE,
	GraphicsArrowDirectionSE,
	GraphicsArrowDirectionS,
	GraphicsArrowDirectionSW,
	GraphicsArrowDirectionW,
	GraphicsArrowDirectionNW
} GraphicsArrowDirection;


#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a < b ? b : a)
#define BETWEEN(a,min,max) MIN(max,MAX(min,a))
#define ABS(a) (a < 0 ? -a : a)

/***************************************************************************************************
 Local Variables
 **************************************************************************************************/

/* Global glib context */
static GLIB_Context_t glibContext;
static GLIB_Rectangle_t joystickBounds;
static int32_t joystickCenterX;
static int32_t joystickCenterY;
static GLIB_Rectangle_t buttonBounds[4];
static int32_t buttonCenterX[4];

static const uint32_t circleRadius = 10;

/***************************************************************************************************
 Static Function Declarations
 **************************************************************************************************/

/***************************************************************************************************
 Function Definitions
 **************************************************************************************************/
void graphInit(void) {
	EMSTATUS status;

	/* Initialize the display module. */
	status = DISPLAY_Init();
	if (DISPLAY_EMSTATUS_OK != status) {
		while (1)
			;
	}

	/* Initialize the DMD module for the DISPLAY device driver. */
	status = DMD_init(0);
	if (DMD_OK != status) {
		while (1)
			;
	}

	status = GLIB_contextInit(&glibContext);
	if (GLIB_OK != status) {
		while (1)
			;
	}

	glibContext.backgroundColor = White;
	glibContext.foregroundColor = Black;

	/* Use Narrow font */
	GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNarrow6x8);

	GLIB_clear(&glibContext);

	joystickBounds = glibContext.clippingRegion;
	joystickCenterX = (joystickBounds.xMax - joystickBounds.xMin)/2;
	joystickBounds.yMax = joystickBounds.yMax / 2;
	joystickCenterY = (joystickBounds.yMax - joystickBounds.yMin)/2 - 5;

	{
		const int32_t y = 90;
		const int32_t height = 2 * circleRadius + 5 + glibContext.font.fontHeight;
		const int32_t ymax = MIN(y + height - 1, glibContext.clippingRegion.yMax);
		const int32_t buttonWidth = glibContext.clippingRegion.yMax / 4;
		const int32_t halfButtonWidth = buttonWidth/2;
		const char labels[4][4] = { "PB0\0", "PB1\0", "BTN2", "BTN3" };
		const uint8_t labelLens[4] = { 3, 3, 4, 4 };

		for(int i = 0; i < 4; i++) {
			buttonBounds[i].xMin = i * buttonWidth;
			buttonBounds[i].xMax = buttonBounds[i].xMin + buttonWidth - 1;
			buttonBounds[i].yMin = y;
			buttonBounds[i].yMax = ymax;
			buttonCenterX[i] = buttonBounds[i].xMin + halfButtonWidth;

			uint8_t strWidth = labelLens[i] * glibContext.font.fontWidth;
			uint8_t posX = buttonCenterX[i] - (strWidth >> 1);
			uint8_t posY = y - glibContext.font.fontHeight - 5;
			GLIB_drawString(&glibContext, labels[i], labelLens[i], posX, posY, 0);
		}
	}

	DMD_updateDisplay();
}

/***************************************************************************************************
 Static Function Definitions
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Print the given string center aligned
 *  \note   The string may contain several lines separated by new line
 *          characters ('\n'). Each line will be printed center aligned.
 *  \param[in]  pContext  Context
 *  \param[in]  pString  String to be displayed
 **************************************************************************************************/
static void graphPrintCenter(GLIB_Context_t *pContext, int32_t y, char *pString) {
	char* nextToken;
	uint8_t len;

	/* Search for the next important token (new line or terminating NULL) */
	for (nextToken = pString; ((*nextToken != '\n') && (*nextToken != '\0')); nextToken++) {
		;
	}

	len = nextToken - pString;
	/* Print the line if it is not null length */
	if (len) {
		uint8_t strWidth = len * pContext->font.fontWidth;
		uint8_t posX = (pContext->pDisplayGeometry->xSize - strWidth) >> 1;
		uint8_t posY = y;
		GLIB_drawString(pContext, pString, len, posX, posY, 0);
	}
	pString = nextToken;
}

static void drawArrow(GLIB_Context_t *pContext, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t width, uint32_t lineWidth, float arrowProportion) {
	arrowProportion = BETWEEN(arrowProportion, 0, 1);
	float gradient = (float) (y2 - y1) / (float) (x2 - x1);
	float tangentGradient = -1 / gradient;
	float tangentCosine;
	float tangentSine;
	if (ABS(tangentGradient) == INFINITY) {
		tangentCosine = 0;
		tangentSine = 1;
	} else {
		float hypotenuse = sqrt(1 + tangentGradient * tangentGradient);
		tangentCosine = 1 / hypotenuse;
		tangentSine = tangentGradient / hypotenuse;
	}
	float halfWidth = (float) width / 2;
	float halfLineWidth = (float) lineWidth / 2;

	int32_t points[16];
	points[0] = x1;
	points[1] = y1;
	// left bottom corner of arrow line
	points[2] = x1 + roundf(-halfLineWidth * tangentCosine);
	points[3] = y1 + roundf(-halfLineWidth * tangentSine);
	// left top corner of arrow line
	points[4] = points[2] + roundf(((x2 - x1) * (1 - arrowProportion)));
	points[5] = points[3] + roundf(((y2 - y1) * (1 - arrowProportion)));
	// left corner of arrow point
	points[6] = points[4] + roundf((halfLineWidth - halfWidth) * tangentCosine);
	points[7] = points[5] + roundf((halfLineWidth - halfWidth) * tangentSine);

	// top point
	points[8] = x2;
	points[9] = y2;

	// mirror arrow bottom points
	int32_t arrowBottomCenterX = x2 - roundf((x2 - x1) * arrowProportion);
	int32_t arrowBottomCenterY = y2 - roundf((y2 - y1) * arrowProportion);
	// right corner of arrow point
	points[10] = arrowBottomCenterX + (arrowBottomCenterX - points[6]);
	points[11] = arrowBottomCenterY + (arrowBottomCenterY - points[7]);
	// right top corner of arrow line
	points[12] = arrowBottomCenterX + (arrowBottomCenterX - points[4]);
	points[13] = arrowBottomCenterY + (arrowBottomCenterY - points[5]);
	// right bottom corner of arrow line
	points[14] = x1 + (x1 - points[2]);
	points[15] = y1 + (y1 - points[3]);

	GLIB_drawPolygon(pContext, 8, points);
}


static void graphDrawArrowDirection(GLIB_Context_t *pContext, GraphicsArrowDirection direction, int32_t centerX, int32_t centerY, uint32_t width, uint32_t lineWidth, uint32_t length,
		float arrowProportion) {
	// calculate coordinates
	float angle = direction * M_PI / 4;

	// y coordinates are inverted
	int32_t y2 = centerY - round(length * cos(angle) / 2);
	int32_t x2 = centerX + round(length * sin(angle) / 2);
	int32_t y1 = centerY + (centerY - y2);
	int32_t x1 = centerX - (x2 - centerX);
	drawArrow(pContext, x1, y1, x2, y2, width, lineWidth, arrowProportion);
}

void clearRect(GLIB_Rectangle_t rect) {
	glibContext.clippingRegion = rect;
	GLIB_applyClippingRegion(&glibContext);
	GLIB_clearRegion(&glibContext);
	GLIB_resetClippingRegion(&glibContext);
	GLIB_applyClippingRegion(&glibContext);
}

void graphSetJoystickDirection(JoystickDirection direction) {
	clearRect(joystickBounds);
	static const int32_t textOffset = 20;

	switch(direction) {
	case JoystickDirectionNone:
		GLIB_drawCircle(&glibContext, joystickCenterX, joystickCenterY, circleRadius);
		graphPrintCenter(&glibContext, joystickCenterY + textOffset, "Joystick off\0");
		break;
	case JoystickDirectionCenter:
		GLIB_drawCircleFilled(&glibContext, joystickCenterX, joystickCenterY, circleRadius);
		graphPrintCenter(&glibContext, joystickCenterY + textOffset, "Center\0");
		break;
	default:
		graphDrawArrowDirection(&glibContext, direction - 2, joystickCenterX, joystickCenterY, 30, 12, 30, 0.5);
		switch(direction) {
		case JoystickDirectionN:
			graphPrintCenter(&glibContext, joystickCenterY + textOffset, "Up\0");
			break;
		case JoystickDirectionNE:
			graphPrintCenter(&glibContext, joystickCenterY + textOffset, "NE\0");
			break;
		case JoystickDirectionE:
			graphPrintCenter(&glibContext, joystickCenterY + textOffset, "Right\0");
			break;
		case JoystickDirectionSE:
			graphPrintCenter(&glibContext, joystickCenterY + textOffset, "SE\0");
			break;
		case JoystickDirectionS:
			graphPrintCenter(&glibContext, joystickCenterY + textOffset, "Down\0");
			break;
		case JoystickDirectionSW:
			graphPrintCenter(&glibContext, joystickCenterY + textOffset, "SW\0");
			break;
		case JoystickDirectionW:
			graphPrintCenter(&glibContext, joystickCenterY + textOffset, "Left\0");
			break;
		case JoystickDirectionNW:
			graphPrintCenter(&glibContext, joystickCenterY + textOffset, "NWp\0");
			break;
		default:break;
		}
	}

	DMD_updateDisplay();
}

void graphSetButtonState(uint8_t buttonNum, bool on) {
	if(buttonNum > 3) { return; }
	clearRect(buttonBounds[buttonNum]);

	if(on) {
		GLIB_drawCircleFilled(&glibContext, buttonCenterX[buttonNum], buttonBounds[buttonNum].yMin + circleRadius, circleRadius);
	} else {
		GLIB_drawCircle(&glibContext, buttonCenterX[buttonNum], buttonBounds[buttonNum].yMin + circleRadius, circleRadius);
	}

	char onText[] = "OUT";
	char offText[] = "IN";
	uint8_t len = on ? 3 : 2;

	uint8_t strWidth = len * glibContext.font.fontWidth;
	uint8_t posX = buttonCenterX[buttonNum] - (strWidth >> 1);
	uint8_t posY = buttonBounds[buttonNum].yMax - glibContext.font.fontHeight;
	GLIB_drawString(&glibContext, on ? onText : offText, len, posX, posY, 0);

	DMD_updateDisplay();
}
