/*
 // Copyright (c) 2012-2014 Eliott Paris & Pierre Guillot, CICM, Universite Paris 8.
 // For information on usage and redistribution, and for a DISCLAIMER OF ALL
 // WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifdef JUCE_WRAPPER

#if JUCE_SUPPORT_CARBON
#define Point CarbonDummyPointName
#define Component CarbonDummyCompName
#include <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#undef Point
#undef Component
#else
#include <Cocoa/Cocoa.h>
#endif

void initialiseMac();
void initialiseMac()
{
#if ! JUCE_64BIT
    [NSApplication sharedApplication];
    NSApplicationLoad();
#endif
}

void hideWindow (void* comp);
void hideWindow (void* comp)
{
    NSWindow* pluginWindow = (NSWindow *)comp;
    [pluginWindow setExcludedFromWindowsMenu: YES];
    [pluginWindow setCanHide: YES];
    [pluginWindow setReleasedWhenClosed: YES];
    [pluginWindow setIgnoresMouseEvents: NO];
    [pluginWindow setTitle:(NSString *)("ZAZAZA") ];
}


#endif