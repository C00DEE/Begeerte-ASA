#include "mdx12_api.h"

#pragma warning(push)
#pragma warning(disable: 26451)
#pragma warning(disable: 26812)
#pragma warning(disable: 6387)
#pragma warning(pop)

namespace g_Hook {
    void StartAllHooks();
    void StopAllHooks();

    inline bool UWorldTickOK = false;
	inline bool HandleDisconnectOK = false;
	inline bool OutputTextLineOK = false;
	inline bool PostRenderOK = false;
	inline bool PhysicsRotationOK = false;
	inline bool TakeDamageOK = false;
}