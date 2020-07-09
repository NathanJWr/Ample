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
#include "numobject.h"
#include "boolobject.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
AmpObject *
amp_integer_add (AmpObject *this, AmpObject *val)
{
  double add = AMP_NUMBER (this)->val + AMP_NUMBER (val)->val;
  AmpObject *obj = AmpNumberCreate (add);
  return obj;
}
AmpObject *
amp_integer_sub (AmpObject *this, AmpObject *val)
{
  double sub = AMP_NUMBER (this)->val - AMP_NUMBER (val)->val;
  AmpObject *obj = AmpNumberCreate (sub);
  return obj;
}

AmpObject *
amp_integer_div (AmpObject *this, AmpObject *val)
{
  double div = AMP_NUMBER (this)->val / AMP_NUMBER (val)->val;
  AmpObject *obj = AmpNumberCreate (div);
  return obj;
}

AmpObject *
amp_integer_mul (AmpObject *this, AmpObject *val)
{
  double mult = AMP_NUMBER (this)->val * AMP_NUMBER (val)->val;
  AmpObject *obj = AmpNumberCreate (mult);
  return obj;
}

AmpObject *
amp_integer_equal (AmpObject *this, AmpObject *val)
{
  bool32 equal;
  AmpObject *obj;
  double val1 = AMP_NUMBER (this)->val;
  double val2 = AMP_NUMBER (val)->val;

  equal = fabs (val1 - val2) < DBL_EPSILON;
  obj = AmpBoolCreate (equal);
  return obj;
}

AmpObject *
amp_integer_not_equal (AmpObject *this, AmpObject *val)
{
  AmpObject_Bool *equal = AMP_BOOL (amp_integer_equal (this, val)); 
  equal->val = !equal->val;
  return AMP_OBJECT (equal);
}

static AmpObjectInfo int_info;
static bool32 int_info_initialized;
AmpObject *
AmpNumberCreate (double val)
{
  AmpObject_Num *a = NULL;
  /* fill out type info */
  if (!int_info_initialized)
    {
      int_info.type = AMP_OBJECT_NUMBER;
      AmpObjectInitializeOperationsToUnsupported (&int_info.ops);
      int_info.ops.add = amp_integer_add;
      int_info.ops.sub = amp_integer_sub;
      int_info.ops.div = amp_integer_div;
      int_info.ops.mult = amp_integer_mul;
      int_info.ops.equal = amp_integer_equal;
      int_info.ops.not_equal = amp_integer_not_equal;
    }

  a = malloc (sizeof (AmpObject_Num));
  a->info = &int_info;
  a->refcount = 1;
  a->dealloc = AmpObjectDestroyBasic;
  a->val = val;

  return AMP_OBJECT (a);
}
