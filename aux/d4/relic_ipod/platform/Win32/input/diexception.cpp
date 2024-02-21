#include "diexception.h"
#include "dxconstants.h"

#include "core/exception.h"

void _throw_di_exception(HRESULT r)
{
    const char *message = "<unknown>";
    switch (r)
    {
	case DIERR_INVALIDPARAM:          message = "DIERR_INVALIDPARAM";         break;
	case DIERR_NOTEXCLUSIVEACQUIRED:  message = "DIERR_NOTEXCLUSIVEACQUIRED"; break;
    case DIERR_NOTINITIALIZED:        message = "DIERR_NOTINITIALIZED";       break;
    case DIERR_OBJECTNOTFOUND:        message = "DIERR_OBJECTNOTFOUND";       break;
    case DIERR_UNSUPPORTED:           message = "DIERR_UNSUPPORTED";          break;
	}
    _throw_exception("DirectInput error %p : %s\n", r, message);
}
