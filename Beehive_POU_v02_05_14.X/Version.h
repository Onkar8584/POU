/*
================================================================================
File name:    Version.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  Version.h contains the macro definitions for each sub divisions of the version

Class Methods:
 None

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
0.1.0.0     10-21-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/


#ifndef _VERSION_H
#define	_VERSION_H

// Parts of the version
#define MAJOR               2
#define MINOR               5
#define INCREMENTAL         14//13//11//10//8//7//3   //2
#define INTERNAL            4   //0

// To convert the parameter into string
#define _STRING(x)          #x
#define STRING(x)           _STRING(x)

#define VERSION_STRING      STRING(v.MAJOR.MINOR.INCREMENTAL)


#endif	/* _VERSION_H */

