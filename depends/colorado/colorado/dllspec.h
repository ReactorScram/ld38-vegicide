#ifndef DLLSPEC_H
#define DLLSPEC_H

#ifdef _WIN32 // || _WIN64
    #define Q_DECL_EXPORT __declspec(dllexport)
    #define Q_DECL_IMPORT __declspec(dllimport)
#else
    #define Q_DECL_EXPORT
    #define Q_DECL_IMPORT
#endif

#ifdef COLORADO_MAKING_WINDOWS_LIB
	#define COLORADO_DLLSPEC Q_DECL_EXPORT
#else
	#define COLORADO_DLLSPEC 
#endif

#endif // DLLSPEC_H
