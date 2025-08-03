%module backend

%include <stdint.i>

%{
#include "../c_backend/functions/functions.h"
#include "../c_backend/functions/bFunctions8/bFunctions8.h"
#include "../c_backend/functions/bFunctions16/bFunctions16.h"
#include "../c_backend/functions/bFunctions32/bFunctions32.h"
#include "../c_backend/functions/bFunctions64/bFunctions64.h"
#include "../c_backend/functions/jumps/jumps.h"
#include "../c_backend/raw_vars/raw_vars.h"
#include "../c_backend/backend.h"
%}

%include "../c_backend/functions/functions.h"
%include "../c_backend/functions/bFunctions8/bFunctions8.h"
%include "../c_backend/functions/bFunctions16/bFunctions16.h"
%include "../c_backend/functions/bFunctions32/bFunctions32.h"
%include "../c_backend/functions/bFunctions64/bFunctions64.h"
%include "../c_backend/functions/jumps/jumps.h"
%include "../c_backend/raw_vars/raw_vars.h"
%include "../c_backend/backend.h"

