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
#include "listobject.h"
#include "../bool.h"
#include "../array.h"

#include <stdlib.h>
void
amp_list_dealloc(AmpObject *obj)
{
  ARRAY_FREE (AMP_LIST (obj)->array);
  AmpObjectDestroyBasic (obj);
}

static AmpObjectInfo list_info;
static bool32 list_info_initialized;
AmpObject *
AmpListCreate (AmpObject *array)
{
  AmpObject_List *list = NULL;
  if (!list_info_initialized)
    {
      list_info.type = AMP_OBJECT_LIST;
      AmpObjectInitializeOperationsToUnsupported (&list_info.ops);

      list_info_initialized = true;
    }
  list = calloc (1, sizeof(AmpObject_List));
  list->info = &list_info;
  list->dealloc = amp_list_dealloc;
  list->refcount = 1;
  ARRAY_ADD (list->array, 10);
  return AMP_OBJECT (list);
}
