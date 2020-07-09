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
#include "../ample_errors.h"
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
  printf (ample_error_codes[ERROR_UNSUPPORTED_OPERATION],
          AMP_OBJECT_TYPE_STR[this->info->type],
          AMP_OBJECT_TYPE_STR[var->info->type]);
  exit (EXIT_FAILURE);
}

void AmpObjectInitializeOperationsToUnsupported (AmpOperations *ops)
{
  ops->add = AmpObjectUnsupportedOperation;
  ops->sub = AmpObjectUnsupportedOperation;
  ops->div = AmpObjectUnsupportedOperation;
  ops->mult = AmpObjectUnsupportedOperation;
  ops->equal = AmpObjectUnsupportedOperation;
  ops->not_equal = AmpObjectUnsupportedOperation;
  ops->less_than = AmpObjectUnsupportedOperation;
  ops->greater_than = AmpObjectUnsupportedOperation;
}
