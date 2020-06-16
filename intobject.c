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

AmpObject *
amp_object_create_integer (int val)
{
  AmpObject_Int *a = malloc (sizeof (AmpObject_Int));
  a->type = AMP_OBJ_INT;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->val = val;
  return AMP_OBJECT (a);
}
