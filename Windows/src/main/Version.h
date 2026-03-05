//Version.h
#ifndef _VERSION_H_
#define _VERSION_H_

//Version defined
#define VERSION_Major	1
#define VERSION_Minor	16
// #define VERSION_Release	2
// #define VERSION_Build	0

#define _Stringizing(v) #v
#define _VerJoin(a, b/*, c, d*/)  _Stringizing(a.b/*.c.d*/)

//version string: a.b.c.d
#define STR_VERSION		_VerJoin(VERSION_Major,VERSION_Minor/*,VERSION_Release,VERSION_Build*/)

//build time string: Dec 1 2021 00:00:00
#define STR_BuilDateTime	__DATE__ " " __TIME__



#endif	//_VERSION_H_
