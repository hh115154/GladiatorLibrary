#ifndef SI_H
#define SI_H

static const int si_indicator[200][7] = {

		{   1,   26},
		{  10,    1,   26,   16,    0,    0,    0}, //M
		{   7,    1,   26,   19,    0,    0,    0}, //L
		{  17,    1,   26,    9,    0,    0,    0}, //H
		{  13,    1,   26,   13,    0,    0,    0}, //Q

		{   2,   44},
		{  16,    1,   44,   28,    0,    0,    0},
		{  10,    1,   44,   34,    0,    0,    0},
		{  28,    1,   44,   16,    0,    0,    0},
		{  22,    1,   44,   22,    0,    0,    0},

		{   3,   70},
		{  26,    1,   70,   44,    0,    0,    0},
		{  15,    1,   70,   55,    0,    0,    0},
		{  44,    2,   35,   13,    0,    0,    0},
		{  36,    2,   35,   17,    0,    0,    0},

		{   4,  100},
		{  36,    2,   50,   32,    0,    0,    0},
		{  20,    1,  100,   80,    0,    0,    0},
		{  64,    4,   25,    9,    0,    0,    0},
		{  52,    2,   50,   24,    0,    0,    0},

		{   5,  134},
		{  48,    2,   67,   43,    0,    0,    0},
		{  26,    1,  134,  108,    0,    0,    0},
		{  88,    2,   33,   11,    2,   34,   12},
		{  72,    2,   33,   15,    2,   34,   16},

		{   6,  172},
		{  64,    4,   43,   27,    0,    0,    0},
		{  36,    2,   86,   68,    0,    0,    0},
		{ 112,    4,   43,   15,    0,    0,    0},
		{  96,    4,   43,   19,    0,    0,    0},

		{   7,  196},
		{  72,    4,   49,   31,    0,    0,    0},
		{  40,    2,   98,   78,    0,    0,    0},
		{ 130,    4,   39,   13,    1,   40,   14},
		{ 108,    2,   32,   14,    4,   33,   15},

		{   8,  242},
		{  88,    2,   60,   38,    2,   61,   39},
		{  48,    2,  121,   97,    0,    0,    0},
		{ 156,    4,   40,   14,    2,   41,   15},
		{ 132,    4,   40,   18,    2,   41,   19},

		{   9,  292},
		{ 110,    3,   58,   36,    2,   59,   37},
		{  60,    2,  146,  116,    0,    0,    0},
		{ 192,    4,   36,   12,    4,   37,   13},
		{ 160,    4,   36,   16,    4,   37,   17},

		{  10,  346},
		{ 130,    4,   69,   43,    1,   70,   44},
		{  72,    2,   86,   68,    2,   87,   69},
		{ 224,    6,   43,   15,    2,   44,   16},
		{ 192,    6,   43,   19,    2,   44,   20},

		{  11,  404},
		{ 150,    1,   80,   50,    4,   81,   51},
		{  80,    4,  101,   81,    0,    0,    0},
		{ 264,    3,   36,   12,    8,   37,   13},
		{ 224,    4,   50,   22,    4,   51,   23},

		{  12,  466},
		{ 176,    6,   58,   36,    2,   59,   37},
		{  96,    2,  116,   92,    2,  117,   93},
		{ 308,    7,   42,   14,    4,   43,   15},
		{ 260,    4,   46,   20,    6,   47,   21},

		{  13,  532},
		{ 198,    8,   59,   37,    1,   60,   38},
		{ 104,    4,  133,  107,    0,    0,    0},
		{ 352,   12,   33,   11,    4,   34,   12},
		{ 288,    8,   44,   20,    4,   45,   21},

		{  14,  581},
		{ 216,    4,   64,   40,    5,   65,   41},
		{ 120,    3,  145,  115,    1,  146,  116},
		{ 384,   11,   36,   12,    5,   37,   13},
		{ 320,   11,   36,   16,    5,   37,   17},

		{  15,  655},
		{ 240,    5,   65,   41,    5,   66,   42},
		{ 132,    5,  109,   87,    1,  110,   88},
		{ 432,   11,   36,   12,    7,   37,   13},
		{ 360,    5,   54,   24,    7,   55,   25},

		{  16,  733},
		{ 280,    7,   73,   45,    3,   74,   46},
		{ 144,    5,  122,   98,    1,  123,   99},
		{ 480,    3,   45,   15,   13,   46,   16},
		{ 408,   15,   43,   19,    2,   44,   20},

		{  17,  815},
		{ 308,   10,   74,   46,    1,   75,   47},
		{ 168,    1,  135,  107,    5,  136,  108},
		{ 532,    2,   42,   14,   17,   43,   15},
		{ 448,    1,   50,   22,   15,   51,   23},

		{  18,  901},
		{ 338,    9,   69,   43,    4,   70,   44},
		{ 180,    5,  150,  120,    1,  151,  121},
		{ 588,    2,   42,   14,   19,   43,   15},
		{ 504,   17,   50,   22,    1,   51,   23},

		{  19,  991},
		{ 364,    3,   70,   44,   11,   71,   45},
		{ 196,    3,  141,  113,    4,  142,  114},
		{ 650,    9,   39,   13,   16,   40,   14},
		{ 546,   17,   47,   21,    4,   48,   22},

		{  20, 1085},
		{ 416,    3,   67,   41,   13,   68,   42},
		{ 224,    3,  135,  107,    5,  136,  108},
		{ 700,   15,   43,   15,   10,   44,   16},
		{ 600,   15,   54,   24,    5,   55,   25},

		{  21, 1156},
		{ 442,   17,   68,   42,    0,    0,    0},
		{ 224,    4,  144,  116,    4,  145,  117},
		{ 750,   19,   46,   16,    6,   47,   17},
		{ 644,   17,   50,   22,    6,   51,   23},

		{  22, 1258},
		{ 476,   17,   74,   46,    0,    0,    0},
		{ 252,    2,  139,  111,    7,  140,  112},
		{ 816,   34,   37,   13,    0,    0,    0},
		{ 690,    7,   54,   24,   16,   55,   25},

		{  23, 1364},
		{ 504,    4,   75,   47,   14,   76,   48},
		{ 270,    4,  151,  121,    5,  152,  122},
		{ 900,   16,   45,   15,   14,   46,   16},
		{ 750,   11,   54,   24,   14,   55,   25},

		{  24, 1474},
		{ 560,    6,   73,   45,   14,   74,   46},
		{ 300,    6,  147,  117,    4,  148,  118},
		{ 960,   30,   46,   16,    2,   47,   17},
		{ 810,   11,   54,   24,   16,   55,   25},

		{  25, 1588},
		{ 588,    8,   75,   47,   13,   76,   48},
		{ 312,    8,  132,  106,    4,  133,  107},
		{1050,   22,   45,   15,   13,   46,   16},
		{ 870,    7,   54,   24,   22,   55,   25},

		{  26, 1706},
		{ 644,   19,   74,   46,    4,   75,   47},
		{ 336,   10,  142,  114,    2,  143,  115},
		{1110,   33,   46,   16,    4,   47,   17},
		{ 952,   28,   50,   22,    6,   51,   23},

		{  27, 1828},
		{ 700,   22,   73,   45,    3,   74,   46},
		{ 360,    8,  152,  122,    4,  153,  123},
		{1200,   12,   45,   15,   28,   46,   16},
		{1020,    8,   53,   23,   26,   54,   24},

		{  28, 1921},
		{ 728,    3,   73,   45,   23,   74,   46},
		{ 390,    3,  147,  117,   10,  148,  118},
		{1260,   11,   45,   15,   31,   46,   16},
		{1050,    4,   54,   24,   31,   55,   25},
		
		{  29, 2051},
		{ 784,   21,   73,   45,    7,   74,   46},
		{ 420,    7,  146,  116,    7,  147,  117},
		{1350,   19,   45,   15,   26,   46,   16},
		{1140,    1,   53,   23,   37,   54,   24},

		{  30, 2185},
		{ 812,   19,   75,   47,   10,   76,   48},
		{ 450,    5,  145,  115,   10,  146,  116},
		{1440,   23,   45,   15,   25,   46,   16},
		{1200,   15,   54,   24,   25,   55,   25},

		{  31, 2323},
		{ 868,    2,   74,   46,   29,   75,   47},
		{ 480,   13,  145,  115,    3,  146,  116},
		{1530,   23,   45,   15,   28,   46,   16},
		{1290,   42,   54,   24,    1,   55,   25},

		{  32, 2465},
		{ 924,   10,   74,   46,   23,   75,   47},
		{ 510,   17,  145,  115,    0,    0,    0},
		{1620,   19,   45,   15,   35,   46,   16},
		{1350,   10,   54,   24,   35,   55,   25},

		{  33, 2611},
		{ 980,   14,   74,   46,   21,   75,   47},
		{ 540,   17,  145,  115,    1,  146,  116},
		{1710,   11,   45,   15,   46,   46,   16},
		{1440,   29,   54,   24,   19,   55,   25},

		{  34, 2761},
		{1036,   14,   74,   46,   23,   75,   47},
		{ 570,   13,  145,  115,    6,  146,  116},
		{1800,   59,   46,   16,    1,   47,   17},
		{1530,   44,   54,   24,    7,   55,   25},

		{  35, 2876},
		{1064,   12,   75,   47,   26,   76,   48},
		{ 570,   12,  151,  121,    7,  152,  122},
		{1890,   22,   45,   15,   41,   46,   16},
		{1590,   39,   54,   24,   14,   55,   25},

		{  36, 3034},
		{1120,    6,   75,   47,   34,   76,   48},
		{ 600,    6,  151,  121,   14,  152,  122},
		{1980,    2,   45,   15,   64,   46,   16},
		{1680,   46,   54,   24,   10,   55,   25},

		{  37, 3196},
		{1204,   29,   74,   46,   14,   75,   47},
		{ 630,   17,  152,  122,    4,  153,  123},
		{2100,   24,   45,   15,   46,   46,   16},
		{1770,   49,   54,   24,   10,   55,   25},

		{  38, 3362},
		{1260,   13,   74,   46,   32,   75,   47},
		{ 660,    4,  152,  122,   18,  153,  123},
		{2220,   42,   45,   15,   32,   46,   16},
		{1860,   48,   54,   24,   14,   55,   25},

		{  39, 3532},
		{1316,   40,   75,   47,    7,   76,   48},
		{ 720,   20,  147,  117,    4,  148,  118},
		{2310,   10,   45,   15,   67,   46,   16},
		{1950,   43,   54,   24,   22,   55,   25},

		{  40, 3706},
		{1372,   18,   75,   47,   31,   76,   48},
		{ 750,   19,  148,  118,    6,  149,  119},
		{2430,   20,   45,   15,   61,   46,   16},
		{2040,   34,   54,   24,   34,   55,   25}

};

#endif // SI_H