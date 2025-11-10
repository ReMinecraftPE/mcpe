#include "common/utility/JsonParser.hpp"
#include "StencilOp.hpp"

using namespace mce;

std::map<std::string, StencilOp> _CreateStencilOpMap()
{
	std::map<std::string, StencilOp> m;
    m["Keep"]    = STENCIL_OP_KEEP;
    m["Replace"] = STENCIL_OP_REPLACE;
	return m;
}
const std::map<std::string, StencilOp> _stencilOpMap = _CreateStencilOpMap();

template <>
bool mce::parse<StencilOp>(const rapidjson::Value& root, const std::string& name, StencilOp& out)
{
    return parse<StencilOp>(root, name, _stencilOpMap, out);
}
