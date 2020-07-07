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
#include "boolobject.h"
#include <stdlib.h>
static AmpObjectInfo bool_info;
static bool32 bool_info_initialized;
AmpObject *
AmpBoolCreate (bool32 val)
{
  AmpObject_Bool *obj = NULL;
  if (!bool_info_initialized)
    {
      bool_info.type = AMP_OBJECT_BOOL;
      AmpObjectInitializeOperationsToUnsupported (&bool_info.ops);
    }
  obj = malloc (sizeof (AmpObject_Bool));
  obj->info = &bool_info;
  obj->refcount = 1;
  obj->dealloc = AmpObjectDestroyBasic;
  obj->val = val;

  return AMP_OBJECT (obj);
}

