#include <xtl.h>
#include <vector>
#include <map>

class GlShaderManager
{
public:
	GlShaderManager();

	void ClearCache();
	void ApplyShader();
	void SetTexenv(int tmu, int env);

private:	
	DWORD Dirty;
	D3DVertexDeclaration*  v_decl;

	char * PsGenerator();
	char * VsGenerator();
	
	union {
		DWORD u32;
		BYTE u8[4];
	} Texenv;

	int texture_enabled;

	typedef std::map<DWORD, D3DPixelShader *> PSCache;
	PSCache psCache;

	typedef std::map<DWORD, D3DVertexShader *> VSCache;
	VSCache vsCache;
};

extern GlShaderManager shader;