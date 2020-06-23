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
#include "intobject.h"
#include <stdlib.h>
AmpObject *
amp_integer_add (AmpObject *this, AmpObject *val)
{
  int add = AMP_INTEGER (this)->val + AMP_INTEGER (val)->val;
  AmpObject *obj = amp_object_create_integer (add);
  return obj;
}
AmpObject *
amp_integer_sub (AmpObject *this, AmpObject *val)
{
  int sub = AMP_INTEGER (this)->val - AMP_INTEGER (val)->val;
  AmpObject *obj = amp_object_create_integer (sub);
  return obj;
}

AmpObject *
amp_integer_div (AmpObject *this, AmpObject *val)
{
  int div = AMP_INTEGER (this)->val / AMP_INTEGER (val)->val;
  AmpObject *obj = amp_object_create_integer (div);
  return obj;
}

AmpObject *
amp_integer_mul (AmpObject *this, AmpObject *val)
{
  int mult = AMP_INTEGER (this)->val * AMP_INTEGER (val)->val;
  AmpObject *obj = amp_object_create_integer (mult);
  return obj;
}

static AmpObjectInfo int_info;
static bool32 int_info_initialized;
AmpObject *
amp_object_create_integer (int val)
{
  AmpObject_Int *a = NULL;
  /* fill out type info */
  if (!int_info_initialized)
    {
      int_info.type = AMP_OBJ_INT;
      initiailize_ops_to_unsupported (&int_info.ops);
      int_info.ops.add = amp_integer_add;
      int_info.ops.sub = amp_integer_sub;
      int_info.ops.div = amp_integer_div;
      int_info.ops.mult = amp_integer_mul;
    }

  a = malloc (sizeof (AmpObject_Int));
  a->info = &int_info;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->val = val;

  return AMP_OBJECT (a);
}
