/*
 Copyright 2006, 2016 Jeff Verkoeyen. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#ifndef GESTURES_H_
#define GESTURES_H_

enum Directions { TOP, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT, BREAK, INVALID, END };

const Vector dirVectors[] = { Vector(0,200,0), Vector(-141,141,0), Vector(-200,0,0), Vector(-141,-141,0), Vector(0,-200,0), Vector(141,-141,0), Vector(200,0,0), Vector(141,141,0) };

const Directions FireballEasy[] = { TOP, BOTTOM, LEFT, RIGHT, END };
const Directions RainSpellEasy[] = { TOP, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT, TOP, END };
const Directions ProtectEasy[] = { TOPLEFT, BOTTOMRIGHT, TOPRIGHT, BOTTOMLEFT, END };

const Directions FireballNormal[] = { TOP, BOTTOM, LEFT, RIGHT, TOP, BOTTOM, END };
const Directions RainSpellNormal[] = { TOP, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT, TOP, BOTTOMLEFT, BOTTOMRIGHT, TOP, END };
const Directions ProtectNormal[] = { TOPLEFT, BOTTOMRIGHT, TOPRIGHT, BOTTOMLEFT, TOPLEFT, TOP, TOPRIGHT, RIGHT, BOTTOMRIGHT, END };

const Directions FireballHard[] = { TOP, BOTTOM, LEFT, RIGHT, TOP, BOTTOM, BOTTOMLEFT, LEFT, TOPLEFT, TOP, END };
const Directions RainSpellHard[] = { TOP, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT, TOP, BOTTOMLEFT, BOTTOMRIGHT, TOP, BOTTOM, TOP, BOTTOM, END };
const Directions ProtectHard[] = { TOPLEFT, BOTTOMRIGHT, TOPRIGHT, BOTTOMLEFT, TOPLEFT, TOP, TOPRIGHT, RIGHT, BOTTOMRIGHT, BOTTOM, BOTTOMLEFT, LEFT, TOPLEFT, END };

const Directions FireballCrazy[] = { TOP, BOTTOM, LEFT, RIGHT, TOP, BOTTOM, BOTTOMLEFT, LEFT, TOPLEFT, TOP, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, END };
const Directions RainSpellCrazy[] = { TOP, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT, TOP, BOTTOMLEFT, BOTTOMRIGHT, TOP, BOTTOM, TOP, BOTTOM, LEFT, BOTTOMLEFT, BOTTOM, END };
const Directions ProtectCrazy[] = { TOPLEFT, BOTTOMRIGHT, TOPRIGHT, BOTTOMLEFT, TOPLEFT, TOP, TOPRIGHT, RIGHT, BOTTOMRIGHT, BOTTOM, BOTTOMLEFT, LEFT, TOPLEFT, BOTTOMRIGHT, TOPRIGHT, BOTTOMLEFT, TOPLEFT, END };


extern const Directions* FireballSpells[];
extern const Directions* RainSpellSpells[];
extern const Directions* ProtectSpells[];
extern Directions* Fireball;
extern Directions* RainSpell;
extern Directions* ProtectSpell;

#endif