/*
    This file is part of Ample.

    Ample is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ample is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ample.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef BOOL_OBJECT_H_
#define BOOL_OBJECT_H_
#include "ampobject.h"
#include "bool.h"
typedef struct AmpObject_Bool {
  AMP_OBJECT_HEADER;
  bool32 val;
} AmpObject_Bool;
#define AMP_BOOL(obj) ((AmpObject_Bool *)(obj))
AmpObject *AmpBoolCreate (bool32 val);
#endif
