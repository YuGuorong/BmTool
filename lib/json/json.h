#ifndef JSON_JSON_H_INCLUDED
# define JSON_JSON_H_INCLUDED

# include "autolink.h"
# include "value.h"
# include "reader.h"
# include "writer.h"
# include "features.h"
#pragma once

#ifdef _DEBUG
#pragma comment(lib, "lib\\json\\json_vc71_libmtd.lib")
#else
#pragma comment(lib, "lib\\json\\json_vc71_libmt.lib")
#endif

#endif // JSON_JSON_H_INCLUDED
