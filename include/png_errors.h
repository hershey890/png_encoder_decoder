#pragma once

/************************************************************************
 * TYPEDEFS
 ************************************************************************/
enum PNG_ERRORS {
    EFILE=1,
    ESIGNATURE,
    ECHUNK,
    EIHDR,
    EPLTE,
    EIDAT,
    EIEND,
    ECHUNK_INVALID,
};
