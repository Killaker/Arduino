
#ifndef _Sanctuary_Sanctuary_h
#define _Sanctuary_Sanctuary_h


/*
   Sanctuary.h -- part of the Sanctuary project.
   Declarations for everything that was generated...
   Pat Deegan
   Psychogenic https://www.psychogenic.com

   Copyright (C) 2017 Pat Deegan

   Generated by DruidBuilder [https://devicedruid.com/],
   as part of project "08a0574074914bed8e8d0c039c85c353N4AD2BFZEU",
   aka Sanctuary.

   Druid4Arduino, Device Druid, Druid Builder, the builder
   code brewery and its wizards, SerialUI and supporting
   libraries, as well as the generated parts of this program
   are
              Copyright (C) 2013-2017 Pat Deegan
   [http://psychogenic.com/ | http://flyingcarsandstuff.com/]
   and distributed under the terms of their respective licenses.
   See http://devicedruid.com for details.


   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
   PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE,
   YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
   CORRECTION.

   Keep in mind that there is no warranty and you are solely
   responsible for the use of all these cool tools.

   Play safe, have fun.

*/



/* we need the SerialUI lib */
#include <SerialUI.h>


/* MySUI
   Our SerialUI Instance, through which we can send/receive
   data from users.
*/
extern SUI::SerialUI MySUI;

/* custom global scope definitions: */
#include <SPI.h>

#define NUM_ANIMAL_TYPES 	4
typedef enum {
  TIGER_FOOD = 0,
  MONKEY_FOOD = 1,
  REPTILE_FOOD = 2,
  FISH_FOOD = 3
} FoodType;

void doMaintenanceStuff();

class FoodLevels {
  public:
    FoodLevels() {
      for (uint8_t i = 0; i < NUM_ANIMAL_TYPES; i++) {
        levels[i] = 10 + (10 * i);

      }
    }
    inline int level(FoodType t) {
      return levels[t];
    }

    bool decrement(FoodType t) {
      if ( ! levels[t]) {
        return false;
      }
      levels[t]--;
      return true;
    }
    bool runningLow() {
      for (uint8_t i = 0; i < NUM_ANIMAL_TYPES; i++) {
        if (levels[i] < 4)
        {
          return true;
        }
      }
      return false;
    }
  private:
    int levels[NUM_ANIMAL_TYPES];
};


// automatically generated declarations (from global scope declarations)

extern int SuperDuperCounter;

extern FoodLevels Stock;



/*
   The container for MyInputs, which
   holds all the variables for the various inputs.
*/
typedef struct MyInputsContainerStruct {

  bool WebCam;
  long int SaltTanks;
  long int FreshTanks;
  bool Lighting;
  long int Fountains;
  long int Lamp;
  // constructor to set sane startup vals
  MyInputsContainerStruct() :
    WebCam(false),
    SaltTanks(0),
    FreshTanks(0),
    Lighting(false),
    Fountains(1),
    Lamp(1)
  {}
} MyInputsContainerSt;

extern MyInputsContainerSt MyInputs;


/* MyTracked
   A structure to hold the tracked variables, which will
   automatically update the druid UI when modified by this
   program.
*/
typedef struct MyTrackedVarsContainerStruct {

  unsigned long Counter;
  float Office;
  unsigned long Outter;
  bool LowLev;
  unsigned long TigerFood;
  // constructor to set sane startup vals
  MyTrackedVarsContainerStruct() :
    Counter(0),
    Office(0),
    Outter(0),
    LowLev(false),
    TigerFood(0)
  {}
} MyTrackedVarsContainerSt;

extern MyTrackedVarsContainerSt MyTracked;


// "heartbeat" function, called periodically while connected
void CustomHeartbeatCode();

/* ********* callbacks and validation functions ********* */



/* *** Animals *** */
namespace Animals {

void doStockLevels();

void doFeedAll();

/* *** Animals -> Tigers *** */
namespace Tigers {

void doEmptyLitter();

void doFeed();

} /* namespace Tigers */



/* *** Animals -> Monkeys *** */
namespace Monkeys {

void doFeed();

void WebCamChanged();

} /* namespace Monkeys */



/* *** Animals -> Fishes *** */
namespace Fishes {

void doFeed();

void SaltTanksChanged();

bool FreshTanksIsOk(long int& newVal);

void FreshTanksChanged();

} /* namespace Fishes */



/* *** Animals -> Reptiles *** */
namespace Reptiles {

void doFeed();

void LightingChanged();

void FountainsChanged();

void LampChanged();

} /* namespace Reptiles */

} /* namespace Animals */



/* ***** SetupSerialUI: where we'll be setting up menus and such **** */
bool SetupSerialUI();


#define DIE_HORRIBLY(msg)	for(;;){ MySUI.println(msg); delay(1000); }


#endif
