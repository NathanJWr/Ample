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
#include <stdio.h>
void
amp_object_destroy_basic (AmpObject *obj)
{
  free (obj);
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

AmpObject *amp_unssuported_operation(AmpObject *this, AmpObject *var)
{
  printf ("unsupported operation for variable of type %d\n", this->info->type);
  exit (1);
}

void initiailize_ops_to_unsupported (AmpOperations *ops)
{
  ops->add = amp_unssuported_operation;
  ops->sub = amp_unssuported_operation;
  ops->div = amp_unssuported_operation;
  ops->mult = amp_unssuported_operation;
}
