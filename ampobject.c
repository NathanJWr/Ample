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
#include "ampobject.h"
#include <stdlib.h>
#include <string.h>
void
amp_object_destroy_basic (AmpObject *obj)
{
  free (obj->value);
  free (obj);
}
AmpObject *
amp_object_create_integer (int val)
{
  struct AmpObject *a = malloc (sizeof (AmpObject));
  a->type = AMP_OBJ_INT;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->value = malloc (sizeof (int));
  *(int *)(a->value) = val;

  return a;
}

AmpObject *
amp_object_create_string (const char *str)
{
  AmpObject *a = malloc (sizeof (AmpObject));
  a->type = AMP_OBJ_STR;
  a->refcount = 1;
  a->dealloc = amp_object_destroy_basic;
  a->value = strdup (str);

  return a;
}
void
obj_inc_refcount (AmpObject *obj)
{
  obj->refcount++;
}
void
obj_dec_refcount (AmpObject *obj)
{
  obj->refcount--;
  if (obj->refcount == 0)
    {
      obj->dealloc (obj);
    }
}