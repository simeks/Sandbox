// Copyright 2008-2014 Simon Ekström

#ifndef _MACROS_H
#define _MACROS_H

#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=nullptr; } }
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }


#endif // _MACROS_H
