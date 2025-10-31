#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

#include "axmol.h"

/*
 * CLASS: AppDelegateMac : NSObject @ 0x100315D48
 */
class AppDelegate : private ax::Application
{
public:
    /* FUNC: AppDelegateMac::dealloc @ 0x10000205D */
    ~AppDelegate() override;

    void initGfxContextAttrs() override;

    /* FUNC: AppDelegateMac::applicationDidFinishLaunching: @ 0x100001ECA */
    bool applicationDidFinishLaunching() override;
    void applicationDidEnterBackground() override {}
    void applicationWillEnterForeground() override {}
};

#endif  // __APP_DELEGATE_H__
