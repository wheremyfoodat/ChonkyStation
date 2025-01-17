#pragma once
#include "gte.h"

// GTE
class testGTE {
public:
    gte GTE;
    uint32_t command; // To pass to the GTE
    void assignRegisters(uint32_t* cop2c, uint32_t* cop2d) {
        for (int i = 0; i < 32; i++) {
            GTE.cop2c.raw[i] = cop2c[i];
            GTE.cop2d.raw[i] = cop2d[i];
        }
    }
    virtual void checkResult(uint32_t* resCop2c, uint32_t* resCop2d) {
        for (int i = 0; i < 32; i++) {
            if (GTE.cop2c.raw[i] != resCop2c[i]) {
                printf("%s register mismatch: got 0x%08x expected 0x%08x\n", GTE.cop2cNames[i].c_str(), GTE.cop2c.raw[i], resCop2c[i]);
            }
        }
        for (int i = 0; i < 32; i++) {
            if (GTE.readCop2d(i) != resCop2d[i]) {
                printf("%s register mismatch: got 0x%08x expected 0x%08x\n", GTE.cop2dNames[i].c_str(), GTE.readCop2d(i), resCop2d[i]);
            }
        }
    }
};
class testRTPS : public testGTE {
public:
    uint32_t cop2c[32] = {
        0x00000ffb,
        0xffb7ff44,
        0xf9ca0ebc,
        0x063700ad,
        0x00000eb7,
        0x00000000,
        0xfffffeac,
        0x00001700,
        0x00000000,
        0x00000fa0,
        0x0000f060,
        0x0000f060,
        0x00000000,
        0x00000640,
        0x00000640,
        0x00000640,
        0x0bb80fa0,
        0x0fa00fa0,
        0x0fa00bb8,
        0x0bb80fa0,
        0x00000fa0,
        0x00000000,
        0x00000000,
        0x00000000,
        0x01400000,
        0x00f00000,
        0x00000400,
        0xfffffec8,
        0x01400000,
        0x00000155,
        0x00000100,
        0x00000000,
    };
    uint32_t cop2d[32] = {
        0x00000b50,
        0xfffff4b0,
        0x00e700d5,
        0xfffffe21,
        0x00b90119,
        0xfffffe65,
        0x2094a539,
        0x00000000,
        0x00001000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000020,
    };
    uint32_t resCop2c[32] = {
        0x00000ffb,
        0xffb7ff44,
        0xf9ca0ebc,
        0x063700ad,
        0x00000eb7,
        0x00000000,
        0xfffffeac,
        0x00001700,
        0x00000000,
        0x00000fa0,
        0x0000f060,
        0x0000f060,
        0x00000000,
        0x00000640,
        0x00000640,
        0x00000640,
        0x0bb80fa0,
        0x0fa00fa0,
        0x0fa00bb8,
        0x0bb80fa0,
        0x00000fa0,
        0x00000000,
        0x00000000,
        0x00000000,
        0x01400000,
        0x00f00000,
        0x00000400,
        0xfffffec8,
        0x01400000,
        0x00000155,
        0x00000100,
        0x00000000,
    };
    uint32_t resCop2d[32] = {
        0x00000b50,
        0xfffff4b0,
        0x00e700d5,
        0xfffffe21,
        0x00b90119,
        0xfffffe65,
        0x2094a539,
        0x00000000,
        0x00000e08,
        0x00000bd1,
        0x000002dc,
        0x00000d12,
        0x00000000,
        0x00000000,
        0x01d003ff,
        0x01d003ff,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000d12,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00e08388,
        0x00000bd1,
        0x000002dc,
        0x00000d12,
        0x000068b7,
        0x000068b7,
        0x00000000,
        0x00000020,
    };

    void doTest() {
        command = 0x00080001;
        printf("testing GTE::RTPS...\n");
        assignRegisters(cop2c, cop2d);
        GTE.execute(command, nullptr);
        checkResult(resCop2c, resCop2d);
    }
};
class testRTPT : public testGTE {
public:
	uint32_t cop2c[32] = {
        0x00000ffb,
        0xffb7ff44,
        0xf9ca0ebc,
        0x063700ad,
        0x00000eb7,
        0x00000000,
        0xfffffeac,
        0x00001700,
        0x00000000,
        0x00000fa0,
        0x0000f060,
        0x0000f060,
        0x00000000,
        0x00000640,
        0x00000640,
        0x00000640,
        0x0bb80fa0,
        0x0fa00fa0,
        0x0fa00bb8,
        0x0bb80fa0,
        0x00000fa0,
        0x00000000,
        0x00000000,
        0x00000000,
        0x01400000,
        0x00f00000,
        0x00000400,
        0xfffffec8,
        0x01400000,
        0x00000155,
        0x00000100,
        0x00000000,
	};
	uint32_t cop2d[32] = {
        0x00e70119,
        0xfffffe65,
        0x00e700d5,
        0xfffffe21,
        0x00b90119,
        0xfffffe65,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000020,
    };
	uint32_t resCop2c[32] = {
        0x00000ffb,
        0xffb7ff44,
        0xf9ca0ebc,
        0x063700ad,
        0x00000eb7,
        0x00000000,
        0xfffffeac,
        0x00001700,
        0x00000000,
        0x00000fa0,
        0x0000f060,
        0x0000f060,
        0x00000000,
        0x00000640,
        0x00000640,
        0x00000640,
        0x0bb80fa0,
        0x0fa00fa0,
        0x0fa00bb8,
        0x0bb80fa0,
        0x00000fa0,
        0x00000000,
        0x00000000,
        0x00000000,
        0x01400000,
        0x00f00000,
        0x00000400,
        0xfffffec8,
        0x01400000,
        0x00000155,
        0x00000100,
        0x00000000,
    };
	uint32_t resCop2d[32] = {
        0x00e70119,
        0xfffffe65,
        0x00e700d5,
        0xfffffe21,
        0x00b90119,
        0xfffffe65,
        0x00000000,
        0x00000000,
        0x00001000,
        0x0000012b,
        0xfffffff0,
        0x000015d9,
        0x00f40176,
        0x00f9016b,
        0x00ed0176,
        0x00ed0176,
        0x00000000,
        0x000015eb,
        0x000015aa,
        0x000015d9,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x0106e038,
        0x0000012b,
        0xfffffff0,
        0x000015d9,
        0x00007c02,
        0x00007c02,
        0x00000000,
        0x00000020,
    };

	void doTest() {
        command = 0x00080030;
        printf("testing GTE::RTPT... \n");
        assignRegisters(cop2c, cop2d);
        GTE.execute(command, nullptr);
        checkResult(resCop2c, resCop2d);
        printf("Done.\n");
	}
};
class testNCDS : public testGTE {
public:
    uint32_t cop2c[32] = {
        0x00000ffb,
        0xffb7ff44,
        0xf9ca0ebc,
        0x063700ad,
        0x00000eb7,
        0x00000000,
        0xfffffeac,
        0x00001700,
        0x00000000,
        0x00000fa0,
        0x0000f060,
        0x0000f060,
        0x00000000,
        0x00000640,
        0x00000640,
        0x00000640,
        0x0bb80fa0,
        0x0fa00fa0,
        0x0fa00bb8,
        0x0bb80fa0,
        0x00000fa0,
        0x00000000,
        0x00000000,
        0x00000000,
        0x01400000,
        0x00f00000,
        0x00000400,
        0xfffffec8,
        0x01400000,
        0x00000155,
        0x00000100,
        0x00000000,
    };
    uint32_t cop2d[32] = {
        0x00000b50,
        0xfffff4b0,
        0x00e700d5,
        0xfffffe21,
        0x00b90119,
        0xfffffe65,
        0x2094a539,
        0x00000572,
        0x00001000,
        0x0000012b,
        0xfffffff0,
        0x000015d9,
        0x00f40176,
        0x00f9016b,
        0x00ed0176,
        0x00ed0176,
        0x00000000,
        0x000015eb,
        0x000015aa,
        0x000015d9,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00572786,
        0x0000012b,
        0xfffffff0,
        0x000015d9,
        0x00007c02,
        0x00007c02,
        0x00000000,
        0x00000020,
    };
    uint32_t resCop2c[32] = {
        0x00000ffb,
        0xffb7ff44,
        0xf9ca0ebc,
        0x063700ad,
        0x00000eb7,
        0x00000000,
        0xfffffeac,
        0x00001700,
        0x00000000,
        0x00000fa0,
        0x0000f060,
        0x0000f060,
        0x00000000,
        0x00000640,
        0x00000640,
        0x00000640,
        0x0bb80fa0,
        0x0fa00fa0,
        0x0fa00bb8,
        0x0bb80fa0,
        0x00000fa0,
        0x00000000,
        0x00000000,
        0x00000000,
        0x01400000,
        0x00f00000,
        0x00000400,
        0xfffffec8,
        0x01400000,
        0x00000155,
        0x00000100,
        0x81f00000,
    };
    uint32_t resCop2d[32] = {
        0x00000b50,
        0xfffff4b0,
        0x00e700d5,
        0xfffffe21,
        0x00b90119,
        0xfffffe65,
        0x2094a539,
        0x00000572,
        0x00001000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00f40176,
        0x00f9016b,
        0x00ed0176,
        0x00ed0176,
        0x00000000,
        0x000015eb,
        0x000015aa,
        0x000015d9,
        0x00000000,
        0x00000000,
        0x20000000,
        0x00000000,
        0x00572786,
        0xffffffff,
        0xffffffff,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000020,
    };

    void doTest() {
        command = 0x00080413;
        printf("testing GTE::NCDS... \n");
        assignRegisters(cop2c, cop2d);
        GTE.execute(command, nullptr);
        checkResult(resCop2c, resCop2d);
        printf("Done.\n");
    }
};
class testNCLIP : public testGTE {
public:
    uint32_t cop2c[32] = {
        0x00000ffb,
        0xffb7ff44,
        0xf9ca0ebc,
        0x063700ad,
        0x00000eb7,
        0x00000000,
        0xfffffeac,
        0x00001700,
        0x00000000,
        0x00000fa0,
        0x0000f060,
        0x0000f060,
        0x00000000,
        0x00000640,
        0x00000640,
        0x00000640,
        0x0bb80fa0,
        0x0fa00fa0,
        0x0fa00bb8,
        0x0bb80fa0,
        0x00000fa0,
        0x00000000,
        0x00000000,
        0x00000000,
        0x01400000,
        0x00f00000,
        0x00000400,
        0xfffffec8,
        0x01400000,
        0x00000155,
        0x00000100,
        0x00001000,
    };
    uint32_t cop2d[32] = {
        0x00e70119,
        0xfffffe65,
        0x00e700d5,
        0xfffffe21,
        0x00b90119,
        0xfffffe65,
        0x00000000,
        0x00000000,
        0x00001000,
        0x0000012b,
        0xfffffff0,
        0x000015d9,
        0x00f40176,
        0x00f9016b,
        0x00ed0176,
        0x00ed0176,
        0x00000000,
        0x000015eb,
        0x000015aa,
        0x000015d9,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x0106e038,
        0x0000012b,
        0xfffffff0,
        0x000015d9,
        0x00007c02,
        0x00007c02,
        0x00000000,
        0x00000020,
    };
    uint32_t resCop2c[32] = {
        0x00000ffb,
        0xffb7ff44,
        0xf9ca0ebc,
        0x063700ad,
        0x00000eb7,
        0x00000000,
        0xfffffeac,
        0x00001700,
        0x00000000,
        0x00000fa0,
        0x0000f060,
        0x0000f060,
        0x00000000,
        0x00000640,
        0x00000640,
        0x00000640,
        0x0bb80fa0,
        0x0fa00fa0,
        0x0fa00bb8,
        0x0bb80fa0,
        0x00000fa0,
        0x00000000,
        0x00000000,
        0x00000000,
        0x01400000,
        0x00f00000,
        0x00000400,
        0xfffffec8,
        0x01400000,
        0x00000155,
        0x00000100,
        0x00000000,
    };
    uint32_t resCop2d[32] = {
        0x00e70119,
        0xfffffe65,
        0x00e700d5,
        0xfffffe21,
        0x00b90119,
        0xfffffe65,
        0x00000000,
        0x00000000,
        0x00001000,
        0x0000012b,
        0xfffffff0,
        0x000015d9,
        0x00f40176,
        0x00f9016b,
        0x00ed0176,
        0x00ed0176,
        0x00000000,
        0x000015eb,
        0x000015aa,
        0x000015d9,
        0x00000000,
        0x00000000,
        0x00000000,
        0x00000000,
        0x0000004d,
        0x0000012b,
        0xfffffff0,
        0x000015d9,
        0x00007c02,
        0x00007c02,
        0x00000000,
        0x00000020,
    };

    void doTest() {
        command = 0x00000006;
        printf("testing GTE::NCLIP...\n");
        assignRegisters(cop2c, cop2d);
        GTE.execute(command, nullptr);
        checkResult(resCop2c, resCop2d);
        printf("Done.\n");
    }
};
class testMVMVA : public testGTE {
public:
    uint32_t cop2c[32] = {
        0xff35cdf4,
        0xf8acd6a6,
        0x1954aa70,
        0xae7b5062,
        0x00000c63,
        0xcad4cc39,
        0xb9c11958,
        0xa942b312,
        0xaf436779,
        0x3c2d507a,
        0x95f99741,
        0x72413224,
        0x0000499d,
        0x0a37d280,
        0xdbe8feec,
        0x2395909a,
        0x47364c98,
        0x795c2ed7,
        0x637e48f4,
        0x89557da5,
        0xffff997a,
        0x690eb551,
        0x3dfb368e,
        0x2bbe355f,
        0xb07c9d22,
        0x030c876b,
        0x00003b7d,
        0x0000765a,
        0x228c2901,
        0xffffe86f,
        0xffffaf93,
        0xc741f000,
    };
    uint32_t cop2d[32] = {
        0x91d5c574,
        0xffffdf9c,
        0xcea213bc,
        0x0000143e,
        0x2360a947,
        0x00003248,
        0x1747e72e,
        0x0000cc08,
        0x0000381d,
        0xffffe2ff,
        0xffffe0f8,
        0xffffe1b6,
        0x9da7438d,
        0xff60f0ed,
        0xbf5961ab,
        0xbf5961ab,
        0x0000b1c1,
        0x0000dda6,
        0x0000ce75,
        0x0000b2d1,
        0xdb01b77a,
        0x19cd28cd,
        0x1a75d97a,
        0xe91dc0ad,
        0x764e464f,
        0x4aa5a1e5,
        0x3b1a1977,
        0x39fb3f5f,
        0xfe8de0c9,
        0x00000007,
    };
    uint32_t resCop2c[32] = {
        0xff35cdf4,
        0xf8acd6a6,
        0x1954aa70,
        0xae7b5062,
        0x00000c63,
        0xcad4cc39,
        0xb9c11958,
        0xa942b312,
        0xaf436779,
        0x3c2d507a,
        0x95f99741,
        0x72413224,
        0x0000499d,
        0x0a37d280,
        0xdbe8feec,
        0x2395909a,
        0x47364c98,
        0x795c2ed7,
        0x637e48f4,
        0x89557da5,
        0xffff997a,
        0x690eb551,
        0x3dfb368e,
        0x2bbe355f,
        0xb07c9d22,
        0x030c876b,
        0x00003b7d,
        0x0000765a,
        0x228c2901,
        0xffffe86f,
        0xffffaf93,
        0x81c00000,
    };
    uint32_t resCop2d[32] = {
        0x91d5c574,
        0xffffdf9c,
        0xcea213bc,
        0x0000143e,
        0x2360a947,
        0x00003248,
        0x1747e72e,
        0x0000cc08,
        0x0000381d,
        0xffff8000,
        0xffff8000,
        0xffff8000,
        0x9da7438d,
        0xff60f0ed,
        0xbf5961ab,
        0xbf5961ab,
        0x0000b1c1,
        0x0000dda6,
        0x0000ce75,
        0x0000b2d1,
        0xdb01b77a,
        0x19cd28cd,
        0x1a75d97a,
        0xe91dc0ad,
        0x764e464f,
        0xcad5dc86,
        0xb9c34e06,
        0xa943a529,
        0xfe8de0c9,
        0x00000007,
    };

    void doTest() {
        command = 0x00080012;
        printf("testing GTE::MVMVA... \n");
        assignRegisters(cop2c, cop2d);
        GTE.execute(command, nullptr);
        checkResult(resCop2c, resCop2d);
        printf("Done.\n");
    }
};
class testDPCS : public testGTE {
public:
    uint32_t cop2c[32] = {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xfffff000,
    };
    uint32_t cop2d[32] = {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0x0000ffff,
        0xffffffff,
        0x00000f80,
        0x00000f80,
        0x00000f80,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0x0000ffff,
        0x0000ffff,
        0x0000ffff,
        0x0000ffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0x00007fff,
        0x00007fff,
        0xffffffff,
        0x00000020,
    };
    uint32_t resCop2c[32] = {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
    };
    uint32_t resCop2d[32] = {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0x0000ffff,
        0xffffffff,
        0x00000ff0,
        0x00000ff0,
        0x00000ff0,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0x0000ffff,
        0x0000ffff,
        0x0000ffff,
        0x0000ffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0x00000ff0,
        0x00000ff0,
        0x00000ff0,
        0x00007fff,
        0x00007fff,
        0xffffffff,
        0x00000020,
    };

    void doTest() {
        command = 0x00080010;
        printf("testing GTE::DPCS... \n");
        assignRegisters(cop2c, cop2d);
        GTE.execute(command, nullptr);
        checkResult(resCop2c, resCop2d);
        printf("Done.\n");
    }
};
class testNCT : public testGTE {
public:
    uint32_t cop2c[32] = {
        0xff35cdf4,
        0xf8acd6a6,
        0x1954aa70,
        0xae7b5062,
        0x00000c63,
        0xcad4cc39,
        0xb9c11958,
        0xa942b312,
        0xaf436779,
        0x3c2d507a,
        0x95f99741,
        0x72413224,
        0x0000499d,
        0x0a37d280,
        0xdbe8feec,
        0x2395909a,
        0x47364c98,
        0x795c2ed7,
        0x637e48f4,
        0x89557da5,
        0xffff997a,
        0x690eb551,
        0x3dfb368e,
        0x2bbe355f,
        0x307c9d22,
        0x030c876b,
        0x00003b7d,
        0x0000765a,
        0x228c2901,
        0xffffe86f,
        0xffffaf93,
        0xc741f000,
    };
    uint32_t cop2d[32] = {
        0x91d5c574,
        0xffffdf9c,
        0xcea213bc,
        0x0000143e,
        0x2360a947,
        0x00003248,
        0x1747e72e,
        0x0000cc08,
        0x0000381d,
        0xffffe2ff,
        0xffffe0f8,
        0xffffe1b6,
        0x9da7438d,
        0xff60f0ed,
        0xbf5961ab,
        0xbf5961ab,
        0x0000b1c1,
        0x0000dda6,
        0x0000ce75,
        0x0000b2d1,
        0xdb01b77a,
        0x19cd28cd,
        0x1a75d97a,
        0xe91dc0ad,
        0x764e464f,
        0x4aa5a1e5,
        0x3b1a1977,
        0x39fb3f5f,
        0x00000000,
        0x00000000,
        0xfe8de0c9,
        0x00000007,
    };
    uint32_t resCop2c[32] = {
        0xff35cdf4,
        0xf8acd6a6,
        0x1954aa70,
        0xae7b5062,
        0x00000c63,
        0xcad4cc39,
        0xb9c11958,
        0xa942b312,
        0xaf436779,
        0x3c2d507a,
        0x95f99741,
        0x72413224,
        0x0000499d,
        0x0a37d280,
        0xdbe8feec,
        0x2395909a,
        0x47364c98,
        0x795c2ed7,
        0x637e48f4,
        0x89557da5,
        0xffff997a,
        0x690eb551,
        0x3dfb368e,
        0x2bbe355f,
        0x307c9d22,
        0x030c876b,
        0x00003b7d,
        0x0000765a,
        0x228c2901,
        0xffffe86f,
        0xffffaf93,
        0x81f80000,
    };
    uint32_t resCop2d[32] = {
        0x91d5c574,
        0xffffdf9c,
        0xcea213bc,
        0x0000143e,
        0x2360a947,
        0x00003248,
        0x1747e72e,
        0x0000cc08,
        0x0000381d,
        0x00007fff,
        0xffff8000,
        0x00007fff,
        0x9da7438d,
        0xff60f0ed,
        0xbf5961ab,
        0xbf5961ab,
        0x0000b1c1,
        0x0000dda6,
        0x0000ce75,
        0x0000b2d1,
        0x17ff00ff,
        0x17ff00ff,
        0x17ff00ff,
        0xe91dc0ad,
        0x764e464f,
        0x0a34aac5,
        0xdbe60855,
        0x239224a0,
        0x00007c1f,
        0x00007c1f,
        0xfe8de0c9,
        0x00000007,
    };

    void doTest() {
        command = 0x00080020;
        printf("testing GTE::NCT... \n");
        assignRegisters(cop2c, cop2d);
        GTE.execute(command, nullptr);
        checkResult(resCop2c, resCop2d);
        printf("Done.\n");
    }
};