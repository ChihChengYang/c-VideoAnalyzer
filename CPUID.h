#ifndef CPUID_H_
#define CPUID_H_

//To control show what instruction is used. 1=ON
#define SHOW_USING_INSTRUCTION 0

int MMX_support, SSE_support, SSE2_support, SSE3_support, SSSE3_support;
int HTT, VMX, TM, PAE, IA64, EST, FXSR, FPU, TSC, APIC;
int using_SSE, using_C, using_SSE2;  //globel

extern void check_CPUID_SIMD(void);
extern void get_CPUID_Brand(void);
extern void get_CPUID_BrandInfo(void);

#endif
