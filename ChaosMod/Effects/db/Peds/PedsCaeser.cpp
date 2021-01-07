#include <stdafx.h>

/*
* Effect by kolyaventuri
*/

static int NUM = 23; // Historically this should be 60 but that seems a bit much

static void OnStart() {

}

static RegisterEffect registerEffect(EFFECT_MISC_CAESER, OnStart);