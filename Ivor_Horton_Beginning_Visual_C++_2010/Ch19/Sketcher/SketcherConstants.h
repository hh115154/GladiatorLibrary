#pragma once

// Program version number for use in serialization
const UINT VERSION_NUMBER = 1;

// Element type definitions
enum ElementType{LINE, RECTANGLE, CIRCLE, CURVE, TEXT};

// Color values for drawing
const COLORREF BLACK = RGB(0,0,0);
const COLORREF RED = RGB(255,0,0);
const COLORREF GREEN = RGB(0,255,0);
const COLORREF BLUE = RGB(0,0,255);
const COLORREF SELECT_COLOR = RGB(255,0,180);
///////////////////////////////////