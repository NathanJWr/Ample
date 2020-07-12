#ifndef AMPLE_ERRORS_H_
#define AMPLE_ERRORS_H_
typedef enum AmpleErrorCode {
  ERROR_INVALID_NUMBER_OF_ARGUMENTS,
  ERROR_INVALID_CAST,
  ERROR_UNSUPPORTED_OPERATION,
  ERROR_OPERATION_ON_DIFFERENT_TYPES,
  ERROR_INVALID_OBJECT_TYPE,
} AmpleErrorCode;

const char *ample_error_codes[] = {
  "ERROR: Invalid number of arguments for function \"%s\", expected %u \
argument(s) and %u were provided\n",
  "ERROR: Cannot cast object of type %s to %s\n",
  "ERROR: Unsupported operation for variable of type %s operating on type %s\n",
  "ERROR: Cannot perform operation of differing types: %s and %s\n",
  "ERROR: Invalid object of type %s\n",
};
#endif
