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
AmpObjectDestroyBasic (AmpObject *obj)
{
  free (obj);
}

void
AmpObjectIncrementRefcount (AmpObject *obj)
{
  obj->refcount++;
}
void
AmpObjectDecrementRefcount (AmpObject *obj)
{
  obj->refcount--;
  if (obj->refcount == 0)
    {
      obj->dealloc (obj);
    }
}

AmpObject *AmpObjectUnsupportedOperation (AmpObject *this, AmpObject *var)
{
  printf ("unsupported operation for variable of type %d\n", this->info->type);
  exit (1);
}

void AmpObjectInitializeOperationsToUnsupported (AmpOperations *ops)
{
  ops->add = AmpObjectUnsupportedOperation;
  ops->sub = AmpObjectUnsupportedOperation;
  ops->div = AmpObjectUnsupportedOperation;
  ops->mult = AmpObjectUnsupportedOperation;
}
